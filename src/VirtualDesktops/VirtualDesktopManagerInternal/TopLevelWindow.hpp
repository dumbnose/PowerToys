#pragma once

class TopLevelWindow
{
	winrt::com_ptr<IApplicationView> view_;

public:

	TopLevelWindow(IApplicationView* view);
	winrt::com_ptr<IApplicationView> View();
	std::wstring GetAppUserModelId() const;
	std::wstring GetVirtualDesktopId() const;
	HWND GetHwnd() const;
};
