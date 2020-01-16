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
			"aumid1": {
				"window title 1": {
					"VirtualDesktopId": "{275D7711-5032-4C7B-9F76-F4195049AF13}",
					"LastSeen": "timestamp"
				},
				"window title 2" : {
					"VirtualDesktopId": "{275D7711-5032-4C7B-9F76-F4195049AF13}",
					"LastSeen" : "timestamp"
				},
			},
			"aumid2" : {
				"window title 1": {
					"VirtualDesktopId": "{275D7711-5032-4C7B-9F76-F4195049AF13}",
						"LastSeen" : "timestamp"
				},
					"window title 2" : {
						"VirtualDesktopId": "{275D7711-5032-4C7B-9F76-F4195049AF13}",
							"LastSeen" : "timestamp"
					},
			},
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
ViewToVirtualDesktopMapper::GetVirtualDesktopId(std::wstring_view aumid, std::wstring_view window_title)
{
	// Get Virtual Desktop node
	auto titleObject = LookupVirtualDesktopObject(aumid, window_title);
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
ViewToVirtualDesktopMapper::SetVirtualDesktopId(std::wstring_view aumid, std::wstring_view window_title, std::wstring_view virtualDesktopId)
{
	// Get Virtual Desktop node
	auto virtualDesktopObject = CreateVirtualDesktopObject(aumid, window_title);

	// Set Virtual Desktop Id
	virtualDesktopObject.SetNamedValue(VirtualDesktopIdValueName_, JsonValue::CreateStringValue(virtualDesktopId));
	virtualDesktopObject.SetNamedValue(LastSeenValueName_, JsonValue::CreateStringValue(CurrentTimeAsString()));
}


JsonObject
ViewToVirtualDesktopMapper::LookupVirtualDesktopObject(std::wstring_view aumid, std::wstring_view window_title)
{
	// Get Aumid node
	if (!mappings_.HasKey(aumid)) return nullptr;
	auto aumidObject = mappings_.GetNamedObject(aumid);

	// Get Window Title node
	if (!aumidObject.HasKey(window_title)) return nullptr;
	return aumidObject.GetNamedObject(window_title);
}


JsonObject
ViewToVirtualDesktopMapper::CreateVirtualDesktopObject(std::wstring_view aumid, std::wstring_view window_title)
{
	// Get/Create Aumid node
	if (!mappings_.HasKey(aumid)) {
		mappings_.SetNamedValue(aumid, JsonObject());
	}
	auto aumidObject = mappings_.GetNamedObject(aumid);

	// Get/Create Title node
	if (!aumidObject.HasKey(window_title)) {
		aumidObject.SetNamedValue(window_title, JsonObject());
	}
	auto titleObject = aumidObject.GetNamedObject(window_title);

	return titleObject;
}


void ViewToVirtualDesktopMapper::DiscardOldMappings()
{
	for (auto& aumidNode : mappings_) {
		auto aumidObject = aumidNode.Value().GetObjectW();
		for(auto& titleMapping : aumidObject) {
			auto mappingObject = titleMapping.Value().GetObjectW();
			std::wstring_view timeStr = mappingObject.GetNamedString(LastSeenValueName_);

			auto time = ParseTime(timeStr);
		}
	}
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

