#pragma once
#include "VirtualDesktopRemovedEventArgs.g.h"


namespace winrt::VirtualDesktops::implementation
{
    struct VirtualDesktopRemovedEventArgs : VirtualDesktopRemovedEventArgsT<VirtualDesktopRemovedEventArgs>
    {
        VirtualDesktopRemovedEventArgs() = default;

        VirtualDesktops::VirtualDesktop RemovedDesktop();
        void RemovedDesktop(VirtualDesktops::VirtualDesktop const& value);
    };
}
