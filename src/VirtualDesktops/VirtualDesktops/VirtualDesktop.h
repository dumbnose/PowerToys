#pragma once
#include "VirtualDesktop.g.h"


namespace winrt::VirtualDesktops::implementation
{
    struct VirtualDesktop : VirtualDesktopT<VirtualDesktop>
    {
        VirtualDesktop() = default;

        hstring Id();
        void Id(hstring const& value);
        bool IsWindowVisible(VirtualDesktops::TopLevelWindow const& window);
        bool IsWindowPresent(VirtualDesktops::TopLevelWindow const& window);
        bool IsApplicationPresent(VirtualDesktops::ApplicationId const& app);
        Windows::Foundation::Collections::IVectorView<VirtualDesktops::ApplicationId> Applications();
        void Applications(Windows::Foundation::Collections::IVectorView<VirtualDesktops::ApplicationId> const& value);
        Windows::Foundation::Collections::IVectorView<VirtualDesktops::TopLevelWindow> Windows();
        void Windows(Windows::Foundation::Collections::IVectorView<VirtualDesktops::TopLevelWindow> const& value);
    };
}
