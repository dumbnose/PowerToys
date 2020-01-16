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

	std::optional<std::wstring_view> GetVirtualDesktopId(std::wstring_view aumid, std::wstring_view window_title);
	void SetVirtualDesktopId(std::wstring_view aumid, std::wstring_view window_title, std::wstring_view virtualDesktopId);

	// State persistence (explicit so host can control timing)
	void LoadMappings();
	void CheckpointMappings();

protected:
	winrt::Windows::Data::Json::JsonObject LookupVirtualDesktopObject(std::wstring_view aumid, std::wstring_view window_title);
	winrt::Windows::Data::Json::JsonObject CreateVirtualDesktopObject(std::wstring_view aumid, std::wstring_view window_title);

	void DiscardOldMappings();

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
