#include "pch.h"
#include "WindowChangedDesktopEventArgs.h"
#include "WindowChangedDesktopEventArgs.g.cpp"


namespace winrt::VirtualDesktops::implementation
{
    VirtualDesktops::TopLevelWindow WindowChangedDesktopEventArgs::Window()
    {
        throw hresult_not_implemented();
    }
    void WindowChangedDesktopEventArgs::Window(VirtualDesktops::TopLevelWindow const& value)
    {
        throw hresult_not_implemented();
    }
}
