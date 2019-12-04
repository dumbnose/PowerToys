#include "pch.h"
#include "VirtualDesktopRemovedEventArgs.h"
#include "VirtualDesktopRemovedEventArgs.g.cpp"


namespace winrt::VirtualDesktops::implementation
{
    VirtualDesktops::VirtualDesktop VirtualDesktopRemovedEventArgs::RemovedDesktop()
    {
        throw hresult_not_implemented();
    }
    void VirtualDesktopRemovedEventArgs::RemovedDesktop(VirtualDesktops::VirtualDesktop const& value)
    {
        throw hresult_not_implemented();
    }
}
