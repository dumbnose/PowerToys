#pragma once
#include "VirtualDesktopChangedEventArgs.g.h"


namespace winrt::VirtualDesktops::implementation
{
    struct VirtualDesktopChangedEventArgs : VirtualDesktopChangedEventArgsT<VirtualDesktopChangedEventArgs>
    {
        VirtualDesktopChangedEventArgs() = default;

        VirtualDesktops::VirtualDesktop OriginalDesktop();
        void OriginalDesktop(VirtualDesktops::VirtualDesktop const& value);
        VirtualDesktops::VirtualDesktop NewDesktop();
        void NewDesktop(VirtualDesktops::VirtualDesktop const& value);
    };
}
