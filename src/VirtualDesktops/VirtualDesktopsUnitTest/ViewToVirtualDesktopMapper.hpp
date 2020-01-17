#pragma once

#include <ctime>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Data.Json.h>


class ViewToVirtualDesktopMapper
{
public:
	ViewToVirtualDesktopMapper();
	~ViewToVirtualDesktopMapper();

	std::optional<std::wstring_view> GetVirtualDesktopId(std::wstring_view aumid, std::wstring_view windowTitle);
	void SetVirtualDesktopId(std::wstring_view aumid, std::wstring_view windowTitle, std::wstring_view virtualDesktopId);

	// State persistence (explicit so host can control timing)
	void LoadMappings();
	void CheckpointMappings();

protected:
	winrt::Windows::Data::Json::JsonObject LookupVirtualDesktopObject(std::wstring_view aumid, std::wstring_view windowTitle);
	winrt::Windows::Data::Json::JsonObject CreateVirtualDesktopObject(std::wstring_view aumid, std::wstring_view windowTitle);

	// Retirement
	const double MonthInSeconds_ = 31 /*days*/ * 24 /*hours*/ * 60 /*minutes*/ * 60 /*seconds*/;
	void DiscardOldMappings();
	void RemoveOldMapping(std::wstring_view aumid, std::wstring_view windowTitle);

	// Registry names
	const wchar_t* RegistryRootKeyName_ = L"VirtualDesktopManagerUnitTests";
	const wchar_t* StateRootKeyName_ = L"State.v1";

	// Json Object names
	const wchar_t* VirtualDesktopIdValueName_ = L"VirtualDesktopId";
	const wchar_t* LastSeenValueName_ = L"LastSeen";

private:
	// Maps Aumid --> WindowTitle --> VirtualDesktopId (and tracks last time it was seen)
	winrt::Windows::Data::Json::JsonObject mappings_;
	std::time_t lastCheckpoint_;
};
