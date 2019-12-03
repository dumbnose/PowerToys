#include "pch.h"
#include "VirtualDesktopAddedEventArgs.h"
#include "VirtualDesktopAddedEventArgs.g.cpp"


namespace winrt::VirtualDesktops::implementation
{
    VirtualDesktops::VirtualDesktop VirtualDesktopAddedEventArgs::Desktop()
    {
        throw hresult_not_implemented();
    }
    void VirtualDesktopAddedEventArgs::Desktop(VirtualDesktops::VirtualDesktop const& value)
    {
        throw hresult_not_implemented();
    }
}
