#pragma once

namespace VirtualDesktops {


class ITopLevelWindow
{
public:
	HWND window;
};


class WindowChangedDesktopEventArgs
{
	ITopLevelWindow* Window;
};


class VirtualDesktopManager
{
public:
	static std::shared_ptr<VirtualDesktopManager> GetDefault();
};




}
