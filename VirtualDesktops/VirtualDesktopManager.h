#pragma once

#include "VirtualDesktopManager.g.h"

namespace winrt::VirtualDesktops::implementation
{
    struct VirtualDesktopManager : VirtualDesktopManagerT<VirtualDesktopManager>
    {
		VirtualDesktopManager() = default;

        int32_t MyProperty();
        void MyProperty(int32_t value);
    };
}

namespace winrt::VirtualDesktops::factory_implementation
{
    struct VirtualDesktopManager : VirtualDesktopManagerT<VirtualDesktopManager, implementation::VirtualDesktopManager>
    {
    };
}
