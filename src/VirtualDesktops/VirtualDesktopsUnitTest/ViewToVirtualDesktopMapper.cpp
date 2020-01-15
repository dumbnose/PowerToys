#include "pch.h"
#include "ViewToVirtualDesktopMapper.hpp"
#include <iostream>
#include <ctime>
#include <iomanip>
#include "../../modules/fancyzones/lib/RegistryHelpers.h"
#include <dumbnose/registry/key.hpp>


using namespace winrt::Windows::Data::Json;


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


ViewToVirtualDesktopMapper::ViewToVirtualDesktopMapper()
{
	//auto start = std::chrono::system_clock::now();
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
	return titleObject.GetNamedString(virtualDesktopId_);
}


std::wstring CurrentTimeAsString()
{
	std::time_t t = std::time(nullptr);
	std::tm tm;
	auto result = gmtime_s(&tm, &t);
	if (result != 0) throw windows_exception(__FUNCTION__ ": gmtime_s() failed", result);
	std::wstringstream time;
	time << std::put_time(&tm, L"%c %Z");

	return time.str();
}


void
ViewToVirtualDesktopMapper::SetVirtualDesktopId(std::wstring_view aumid, std::wstring_view window_title, std::wstring_view virtualDesktopId)
{
	// Get Virtual Desktop node
	auto virtualDesktopObject = CreateVirtualDesktopObject(aumid, window_title);

	// Set Virtual Desktop Id
	virtualDesktopObject.SetNamedValue(virtualDesktopId_, JsonValue::CreateStringValue(virtualDesktopId));
	virtualDesktopObject.SetNamedValue(lastSeen_, JsonValue::CreateStringValue(CurrentTimeAsString()));
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


void
ViewToVirtualDesktopMapper::LoadMappings()
{
	try {
		auto settingsKey = dumbnose::registry::key::hkcu().create(RegistryHelpers::REG_SETTINGS);
		auto key = settingsKey.create(keyRoot_);
		auto state = key.get_value_as_string(stateName_);
		if (!state) return; // No state to parse

		mappings_ = JsonObject::Parse(*state);

	} catch (std::exception& ex) { // If the value is corrupt for some reason, ignore and start fresh
		OutputDebugStringA(ex.what());
	}
}


void 
ViewToVirtualDesktopMapper::CheckpointMappings()
{
	try {
		auto state = mappings_.Stringify();

		auto settingsKey = dumbnose::registry::key::hkcu().create(RegistryHelpers::REG_SETTINGS);
		auto key = settingsKey.create(keyRoot_);
		key.set_value_as_string(stateName_, state);
	} catch (std::exception& ex) { // If it can't save, do not crash the process, as other things run in it
		OutputDebugStringA(ex.what());
	}
}
