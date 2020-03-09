#include "pch.h"
#include "VirtualDesktopChangedEventArgs.h"
#include "VirtualDesktopChangedEventArgs.g.cpp"


namespace winrt::VirtualDesktops::implementation
{
    VirtualDesktops::VirtualDesktop VirtualDesktopChangedEventArgs::OriginalDesktop()
    {
        throw hresult_not_implemented();
    }
    void VirtualDesktopChangedEventArgs::OriginalDesktop(VirtualDesktops::VirtualDesktop const& value)
    {
        throw hresult_not_implemented();
    }
    VirtualDesktops::VirtualDesktop VirtualDesktopChangedEventArgs::NewDesktop()
    {
        throw hresult_not_implemented();
    }
    void VirtualDesktopChangedEventArgs::NewDesktop(VirtualDesktops::VirtualDesktop const& value)
    {
        throw hresult_not_implemented();
    }
}
