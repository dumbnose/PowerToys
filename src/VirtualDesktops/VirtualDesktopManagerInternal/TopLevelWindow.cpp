#include "pch.h"
#include "TopLevelWindow.hpp"


TopLevelWindow::TopLevelWindow(IApplicationView* view)
{
	view->AddRef();
	view_.attach(view);
}


winrt::com_ptr<IApplicationView> TopLevelWindow::View()
{
	return view_;
}


std::wstring TopLevelWindow::GetAppUserModelId() const
{
	if (view_ == nullptr) return L"";

	wil::unique_cotaskmem_string aumid;
	HRESULT hr = view_->GetAppUserModelId(&aumid);

	// There is a race condition where the Window may be gone by the time this is called.  Return "" for this case.
	//if (FAILED(hr)) throw windows_exception(__FUNCTION__ ": GetAppUserModelId() failed", hr);
	if (FAILED(hr)) return L"";

	std::wstring appUserModelId = aumid.get();

	return appUserModelId;
}


std::wstring TopLevelWindow::GetVirtualDesktopId() const
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


// Gets the HWND for the application view, if it is a Win32 one
HWND TopLevelWindow::GetHwnd() const
{
	HWND hwnd = 0;
	winrt::com_ptr<IWin32ApplicationView> win32AppView = view_.try_as<IWin32ApplicationView>();
	if (win32AppView != nullptr) win32AppView->GetWindow(&hwnd);

	return hwnd;
}
