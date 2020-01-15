#pragma once

#include <winrt/Windows.Data.Json.h>


class ViewToVirtualDesktopMapper
{
public:
	ViewToVirtualDesktopMapper();
	~ViewToVirtualDesktopMapper();

	std::optional<std::wstring_view> GetVirtualDesktopId(std::wstring_view aumid, std::wstring_view window_title);
	void SetVirtualDesktopId(std::wstring_view aumid, std::wstring_view window_title, std::wstring_view virtualDesktopId);

protected:
	winrt::Windows::Data::Json::JsonObject LookupVirtualDesktopObject(std::wstring_view aumid, std::wstring_view window_title);
	winrt::Windows::Data::Json::JsonObject CreateVirtualDesktopObject(std::wstring_view aumid, std::wstring_view window_title);

	// State persistence
	void LoadPreviousVirtualDesktopMappings();
	void SaveVirtualDesktopMappings();

	// Registry names
	const wchar_t* keyRoot_ = L"VirtualDesktopManagerUnitTests";
	const wchar_t* stateName_ = L"State.v1";

	// Json Object names
	const wchar_t* virtualDesktopId_ = L"VirtualDesktopId";
	const wchar_t* lastSeen_ = L"LastSeen";

	struct ViewToVirtualDesktopMapping
	{
		std::wstring VirtualDesktopId;
		std::chrono::system_clock::time_point LastSeen;
	};

private:
	// Maps Aumid --> WindowTitle --> VirtualDesktopId (and tracks last time it was seen)
	winrt::Windows::Data::Json::JsonObject mappings_;
};
