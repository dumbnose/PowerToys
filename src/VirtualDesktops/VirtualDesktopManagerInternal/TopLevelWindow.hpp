#pragma once

class TopLevelWindow
{
	winrt::com_ptr<IApplicationView> view_;

public:

	TopLevelWindow(IApplicationView* view)
	{
		view->AddRef();
		view_.attach(view);
	}

	winrt::com_ptr<IApplicationView> View()
	{
		return view_;
	}

	std::wstring GetAppUserModelId() const
	{
		if (view_ == nullptr) return L"";

		wil::unique_cotaskmem_string aumid;
		HRESULT hr = view_->GetAppUserModelId(&aumid);
		if (FAILED(hr)) throw windows_exception(__FUNCTION__ ": GetAppUserModelId() failed", hr);

		std::wstring appUserModelId = aumid.get();

		return appUserModelId;
	}

	std::wstring GetVirtualDesktopId() const
	{
		if (view_ == nullptr) return L"";

		GUID vdId = { 0 };
		HRESULT hr = view_->GetVirtualDesktopId(&vdId);
		if (FAILED(hr)) throw windows_exception(__FUNCTION__ ": GetVirtualDesktopId() failed", hr);

		wchar_t guidBuffer[40] = { 0 };
		int len = StringFromGUID2(vdId, guidBuffer, ARRAYSIZE(guidBuffer));
		std::wstring vdIdStr = guidBuffer;

		return vdIdStr;
	}
};


