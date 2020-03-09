#pragma once
#include "VirtualDesktopAddedEventArgs.g.h"


namespace winrt::VirtualDesktops::implementation
{
    struct VirtualDesktopAddedEventArgs : VirtualDesktopAddedEventArgsT<VirtualDesktopAddedEventArgs>
    {
        VirtualDesktopAddedEventArgs() = default;

        VirtualDesktops::VirtualDesktop Desktop();
        void Desktop(VirtualDesktops::VirtualDesktop const& value);
    };
}
