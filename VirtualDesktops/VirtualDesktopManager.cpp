#include "pch.h"
#include "VirtualDesktopManager.h"
#include "VirtualDesktopManager.g.cpp"

namespace winrt::VirtualDesktops::implementation
{
    int32_t VirtualDesktopManager::MyProperty()
    {
        throw hresult_not_implemented();
    }

    void VirtualDesktopManager::MyProperty(int32_t /* value */)
    {
        throw hresult_not_implemented();
    }
}
