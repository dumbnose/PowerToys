#include "pch.h"
#include "ViewToVirtualDesktopMapper.hpp"
#include "../../modules/fancyzones/lib/RegistryHelpers.h"
#include <dumbnose/registry/key.hpp>


using namespace winrt;
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
	winrt::init_apartment();
	//auto start = std::chrono::system_clock::now();


}


ViewToVirtualDesktopMapper::~ViewToVirtualDesktopMapper()
{
	winrt::uninit_apartment();
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


void
ViewToVirtualDesktopMapper::SetVirtualDesktopId(std::wstring_view aumid, std::wstring_view window_title, std::wstring_view virtualDesktopId)
{
	// Get Virtual Desktop node
	auto virtualDesktopObject = CreateVirtualDesktopObject(aumid, window_title);

	// SEt Virtual Desktop Id
	virtualDesktopObject.SetNamedValue(virtualDesktopId_, JsonValue::CreateStringValue(virtualDesktopId));
}


JsonObject
ViewToVirtualDesktopMapper::LookupVirtualDesktopObject(std::wstring_view aumid, std::wstring_view window_title)
{
	// Get Aumid node
	auto aumidObject = mappings_.GetNamedObject(aumid);
	if (aumidObject == nullptr) return nullptr;

	// Get Window Title node
	return aumidObject.GetNamedObject(window_title);
}


JsonObject
ViewToVirtualDesktopMapper::CreateVirtualDesktopObject(std::wstring_view aumid, std::wstring_view window_title)
{
	// Get/Create Aumid node
	auto aumidObject = mappings_.GetNamedObject(aumid);
	if (aumidObject == nullptr) {
		JsonObject newAumidObject;
		mappings_.SetNamedValue(aumid, newAumidObject);
		aumidObject = newAumidObject;
	}

	// Get/Create Title node
	auto titleObject = mappings_.GetNamedObject(window_title);
	if (titleObject == nullptr) {
		JsonObject newTitleObject;
		mappings_.SetNamedValue(window_title, newTitleObject);
		titleObject = newTitleObject;
	}

	return titleObject;
}


void
ViewToVirtualDesktopMapper::LoadPreviousVirtualDesktopMappings()
{
	auto settingsKey = dumbnose::registry::key::hkcu().create(RegistryHelpers::REG_SETTINGS);
	auto key = settingsKey.create(keyRoot_);
	auto state = key.get_value_as_string(stateName_);

	mappings_ = JsonObject::Parse(state);
}


void 
ViewToVirtualDesktopMapper::SaveVirtualDesktopMappings()
{
	auto state = mappings_.Stringify();

	auto settingsKey = dumbnose::registry::key::hkcu().create(RegistryHelpers::REG_SETTINGS);
	auto key = settingsKey.create(keyRoot_);
	key.set_value_as_string(stateName_, state);

	//key.save_enum_values_as_strings(viewToVirtualDesktop_);
}
