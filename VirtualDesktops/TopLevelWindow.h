#pragma once
#include "TopLevelWindow.g.h"


namespace winrt::VirtualDesktops::implementation
{
    struct TopLevelWindow : TopLevelWindowT<TopLevelWindow>
    {
        TopLevelWindow() = default;

        int32_t HWnd();
        void HWnd(int32_t value);
    };
}
