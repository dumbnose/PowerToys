#include "pch.h"
#include "ViewToVirtualDesktopMapper.hpp"
#include <iostream>
#include <iomanip>
#include "../../modules/fancyzones/lib/RegistryHelpers.h"
#include <dumbnose/registry/key.hpp>


using namespace winrt::Windows::Data::Json;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Foundation::Collections;


/*------------------------------------------------------------------------------------------------------

	This is the format of the data:
	
{
    "Microsoft.Office.POWERPNT.EXE.15": {
        "Safer Azure & MTR - Mn.pptx - PowerPoint": {
            "VirtualDesktopId": "{28D79379-3A79-493E-AE71-EB4CF1729A91}",
            "LastSeen": "01/27/20"
        }
    },
    "Microsoft.Office.WINWORD.EXE.15": {
        "Azure Quality Strategy.docx - Word": {
            "VirtualDesktopId": "{F8C31334-443B-4FCE-AF59-F118550CC082}",
            "LastSeen": "01/27/20"
        },
        "Accelerating OS Updates in Azure Infrastructure fleet.docx  -  Read-Only - Word": {
            "VirtualDesktopId": "{F8C31334-443B-4FCE-AF59-F118550CC082}",
            "LastSeen": "01/27/20"
        }
    },
    "MSEdgeBeta": {
        "Azure Global - JEDI Service Mapping and 14 more pages - Work - Microsoft Edge": {
            "VirtualDesktopId": "{98F5D965-5D5D-4F14-98F6-EE9AB82DAA98}",
            "LastSeen": "01/27/20"
        }
    },
    "MSEdgeBeta.UserData.Profile1": {
        "(2) John Sheehan - Stranger Things or the complete... - Kimberly Kelley Kapustein and 8 more pages - Personal - Microsoft Edge": {
            "VirtualDesktopId": "{98F5D965-5D5D-4F14-98F6-EE9AB82DAA98}",
            "LastSeen": "01/27/20"
        }
    },
    "Microsoft.Office.OUTLOOK.EXE.15": {
        "Inbox - John.Sheehan@microsoft.com - Outlook": {
            "VirtualDesktopId": "{28D79379-3A79-493E-AE71-EB4CF1729A91}",
            "LastSeen": "01/27/20"
        }
    },
    "Microsoft.WindowsTerminal_8wekyb3d8bbwe!App": {
        "cmd": {
            "VirtualDesktopId": "{EA22D094-B06A-499E-BA3F-BC5FF5E822B1}",
            "LastSeen": "01/27/20"
        }
    },
    "{F38BF404-1D43-42F2-9305-67DE0B28FC23}\\regedit.exe": {
        "Registry Editor": {
            "VirtualDesktopId": "{275D7711-5032-4C7B-9F76-F4195049AF13}",
            "LastSeen": "01/27/20"
        }
    }
}
------------------------------------------------------------------------------------------------------*/


ViewToVirtualDesktopMapper::ViewToVirtualDesktopMapper() :
								lastCheckpoint_(std::time(nullptr))
{
}


ViewToVirtualDesktopMapper::~ViewToVirtualDesktopMapper()
{
}


std::optional<std::wstring_view> 
ViewToVirtualDesktopMapper::GetVirtualDesktopId(std::wstring_view aumid, std::wstring_view windowTitle)
{
	// Get Virtual Desktop node
	auto titleObject = LookupVirtualDesktopObject(aumid, windowTitle);

	// @todo:  If we don't have a mappping for this windowTitle, it might make sense to default to a Virtual Desktop
	//         for this aumid with a different windowTitle, so it is at least grouped with similar windows.
	if(titleObject == nullptr) return {};

	// Get Virtual Desktop Id
	return titleObject.GetNamedString(VirtualDesktopIdValueName_);
}


// Localized helper
std::wstring CurrentTimeAsString()
{
	std::time_t t = std::time(nullptr);
	std::tm tm;
	auto result = gmtime_s(&tm, &t);
	if (result != 0) throw windows_exception(__FUNCTION__ ": gmtime_s() failed", result);
	std::wstringstream time;
	time << std::put_time(&tm, L"%D");

	return time.str();
}


std::time_t ParseTime(std::wstring_view timeStr)
{
	std::tm tm{};
	std::wistringstream ss(timeStr.data());
	ss >> std::get_time(&tm, L"%D");
	if (ss.fail()) throw windows_exception(__FUNCTION__ ": std::get_time() failed.");
	
	return std::mktime(&tm);
}


void
ViewToVirtualDesktopMapper::SetVirtualDesktopId(std::wstring_view aumid, std::wstring_view windowTitle, std::wstring_view virtualDesktopId)
{
	// Get Virtual Desktop node
	auto virtualDesktopObject = CreateVirtualDesktopObject(aumid, windowTitle);

	// Set Virtual Desktop Id
	virtualDesktopObject.SetNamedValue(VirtualDesktopIdValueName_, JsonValue::CreateStringValue(virtualDesktopId));
	virtualDesktopObject.SetNamedValue(LastSeenValueName_, JsonValue::CreateStringValue(CurrentTimeAsString()));
}


JsonObject
ViewToVirtualDesktopMapper::LookupVirtualDesktopObject(std::wstring_view aumid, std::wstring_view windowTitle)
{
	// Get Aumid node
	if (!mappings_.HasKey(aumid)) return nullptr;
	auto aumidObject = mappings_.GetNamedObject(aumid);

	// Get Window Title node
	if (!aumidObject.HasKey(windowTitle)) return nullptr;
	return aumidObject.GetNamedObject(windowTitle);
}


JsonObject
ViewToVirtualDesktopMapper::CreateVirtualDesktopObject(std::wstring_view aumid, std::wstring_view windowTitle)
{
	// Get/Create Aumid node
	if (!mappings_.HasKey(aumid)) {
		mappings_.SetNamedValue(aumid, JsonObject());
	}
	auto aumidObject = mappings_.GetNamedObject(aumid);

	// Get/Create Title node
	if (!aumidObject.HasKey(windowTitle)) {
		aumidObject.SetNamedValue(windowTitle, JsonObject());
	}
	auto titleObject = aumidObject.GetNamedObject(windowTitle);

	return titleObject;
}

void ViewToVirtualDesktopMapper::CheckpointIfIntervalElapsed()
{
	auto now = std::time(nullptr);
	auto diffInSeconds = std::difftime(now, lastCheckpoint_);

}


void ViewToVirtualDesktopMapper::DiscardOldMappings()
{
	JsonObject clone = JsonObject::Parse(mappings_.Stringify()); // Need a copy so we can modify the original

	auto now = std::time(nullptr);
	for (auto& aumidNode : clone) {
		auto aumidObject = aumidNode.Value().GetObjectW();
		for(auto& titleNode : aumidObject) {
			auto titleObject = titleNode.Value().GetObjectW();
			std::wstring_view timeStr = titleObject.GetNamedString(LastSeenValueName_);

			auto lastSeenTime = ParseTime(timeStr);
			auto diffInSeconds = std::difftime(now, lastSeenTime);
			if (diffInSeconds > UnusedMappingLifetimeInSeconds_) RemoveOldMapping(aumidNode.Key(), titleNode.Key());
		}
	}
}


void 
ViewToVirtualDesktopMapper::RemoveOldMapping(std::wstring_view aumid, std::wstring_view windowTitle)
{
	auto aumidObject = mappings_.GetNamedObject(aumid);

	// Remove the title mapping 
	aumidObject.Remove(windowTitle);
	if (aumidObject.Size() > 0) return;

	// Remove aumid mapping if it is empty
	mappings_.Remove(aumid);
}


// @todo:   Discard mappings for windows that haven't been seen in more than a month, so that the data doesn't 
//			grow forever.
void
ViewToVirtualDesktopMapper::LoadMappings()
{
	try {
		auto settingsKey = dumbnose::registry::key::hkcu().create(RegistryHelpers::REG_SETTINGS);
		auto key = settingsKey.create(RegistryRootKeyName_);
		auto state = key.get_value_as_string(StateRootKeyName_);
		if (!state) return; // No state to parse

		mappings_ = JsonObject::Parse(*state);

		lastCheckpoint_ = std::time(nullptr);

		DiscardOldMappings();

	} catch (std::exception& ex) { // If the value is corrupt for some reason, ignore and start fresh
		OutputDebugStringA(ex.what());
	}
}


// @todo:   Checkpoint every couple of hours so that a crash doesn't lose all data.  Maybe just check when updating
//			a mapping, rather than schedule some background job.
void 
ViewToVirtualDesktopMapper::CheckpointMappings()
{
	try {
		auto state = mappings_.Stringify();

		auto settingsKey = dumbnose::registry::key::hkcu().create(RegistryHelpers::REG_SETTINGS);
		auto key = settingsKey.create(RegistryRootKeyName_);
		key.set_value_as_string(StateRootKeyName_, state);
	} catch (std::exception& ex) { // If it can't save, do not crash the process, as other things run in it
		OutputDebugStringA(ex.what());
	}
}

