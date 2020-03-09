#pragma once
#include "WindowChangedDesktopEventArgs.g.h"


namespace winrt::VirtualDesktops::implementation
{
    struct WindowChangedDesktopEventArgs : WindowChangedDesktopEventArgsT<WindowChangedDesktopEventArgs>
    {
        WindowChangedDesktopEventArgs() = default;

        VirtualDesktops::TopLevelWindow Window();
        void Window(VirtualDesktops::TopLevelWindow const& value);
    };
}
