#pragma once

struct TopLevelWindow
{
	IApplicationView* View;

	std::wstring GetAppUserModelId() const
	{
		if (View == nullptr) return L"";

		PWSTR aumid = nullptr;
		HRESULT hr = View->GetAppUserModelId(&aumid);
		if (FAILED(hr)) throw windows_exception(__FUNCTION__ ": GetAppUserModelId() failed", hr);

		std::wstring appUserModelId = aumid;

		CoTaskMemFree(aumid);

		return appUserModelId;
	}

	std::wstring GetVirtualDesktopId() const
	{
		if (View == nullptr) return L"";

		GUID vdId = { 0 };
		HRESULT hr = View->GetVirtualDesktopId(&vdId);
		if (FAILED(hr)) throw windows_exception(__FUNCTION__ ": GetVirtualDesktopId() failed", hr);

		wchar_t guidBuffer[40] = { 0 };
		int len = StringFromGUID2(vdId, guidBuffer, ARRAYSIZE(guidBuffer));
		std::wstring vdIdStr = guidBuffer;

		return vdIdStr;
	}
};


