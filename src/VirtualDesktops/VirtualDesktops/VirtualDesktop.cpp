#include "pch.h"
#include "VirtualDesktop.h"
#include "VirtualDesktop.g.cpp"


namespace winrt::VirtualDesktops::implementation
{
    hstring VirtualDesktop::Id()
    {
        throw hresult_not_implemented();
    }
    void VirtualDesktop::Id(hstring const& value)
    {
        throw hresult_not_implemented();
    }
    bool VirtualDesktop::IsWindowVisible(VirtualDesktops::TopLevelWindow const& window)
    {
        throw hresult_not_implemented();
    }
    bool VirtualDesktop::IsWindowPresent(VirtualDesktops::TopLevelWindow const& window)
    {
        throw hresult_not_implemented();
    }
    bool VirtualDesktop::IsApplicationPresent(VirtualDesktops::ApplicationId const& app)
    {
        throw hresult_not_implemented();
    }
    Windows::Foundation::Collections::IVectorView<VirtualDesktops::ApplicationId> VirtualDesktop::Applications()
    {
        throw hresult_not_implemented();
    }
    void VirtualDesktop::Applications(Windows::Foundation::Collections::IVectorView<VirtualDesktops::ApplicationId> const& value)
    {
        throw hresult_not_implemented();
    }
    Windows::Foundation::Collections::IVectorView<VirtualDesktops::TopLevelWindow> VirtualDesktop::Windows()
    {
        throw hresult_not_implemented();
    }
    void VirtualDesktop::Windows(Windows::Foundation::Collections::IVectorView<VirtualDesktops::TopLevelWindow> const& value)
    {
        throw hresult_not_implemented();
    }
}
