#include "pch.h"
#include "VirtualDesktopManager.h"
#include "VirtualDesktopManager.g.cpp"
#include "VirtualDesktopsInternal.h"


namespace winrt::VirtualDesktops::implementation
{
	VirtualDesktopManager::VirtualDesktopManager()
	{
		HRESULT hr = ::CoInitializeEx(NULL, COINIT_MULTITHREADED);
		if (FAILED(hr)) throw hresult_error(hr);

		IServiceProvider* pServiceProvider = nullptr;
		IVirtualDesktopManagerInternal* pDesktopManagerInternal = nullptr;
		IVirtualDesktopManager* pDesktopManager = nullptr;
		IApplicationViewCollection* viewCollection = nullptr;
		IVirtualDesktopPinnedApps* pinnedApps = nullptr;
		IVirtualDesktopNotificationService* pDesktopNotificationService = nullptr;

		hr = ::CoCreateInstance(CLSID_ImmersiveShell, nullptr, CLSCTX_LOCAL_SERVER, IID_IServiceProvider, (PVOID*)&pServiceProvider);
		if(FAILED(hr)) throw hresult_error(hr);

		//pServiceProvider->QueryService(IID_PPV_ARGS(&viewCollection));

		//pServiceProvider->QueryService(__uuidof(IVirtualDesktopManager), &pDesktopManager);

		//pServiceProvider->QueryService(CLSID_VirtualDesktopPinnedApps, IID_PPV_ARGS(&pinnedApps));

		//pServiceProvider->QueryService(CLSID_VirtualDesktopManagerInternal, IID_PPV_ARGS(&pDesktopManagerInternal));
	}

    winrt::event_token VirtualDesktopManager::CurrentDesktopChanged(Windows::Foundation::EventHandler<VirtualDesktops::VirtualDesktopChangedEventArgs> const& handler)
    {
        throw hresult_not_implemented();
    }
    void VirtualDesktopManager::CurrentDesktopChanged(winrt::event_token const& token) noexcept
    {
        throw hresult_not_implemented();
    }
    winrt::event_token VirtualDesktopManager::VirtualDesktopAdded(Windows::Foundation::EventHandler<VirtualDesktops::VirtualDesktopAddedEventArgs> const& handler)
    {
        throw hresult_not_implemented();
    }
    void VirtualDesktopManager::VirtualDesktopAdded(winrt::event_token const& token) noexcept
    {
        throw hresult_not_implemented();
    }
    winrt::event_token VirtualDesktopManager::VirtualDesktopRemoved(Windows::Foundation::EventHandler<VirtualDesktops::VirtualDesktopRemovedEventArgs> const& handler)
    {
        throw hresult_not_implemented();
    }
    void VirtualDesktopManager::VirtualDesktopRemoved(winrt::event_token const& token) noexcept
    {
        throw hresult_not_implemented();
    }
    winrt::event_token VirtualDesktopManager::WindowChangedDesktops(Windows::Foundation::EventHandler<VirtualDesktops::WindowChangedDesktopEventArgs> const& handler)
    {
        throw hresult_not_implemented();
    }
    void VirtualDesktopManager::WindowChangedDesktops(winrt::event_token const& token) noexcept
    {
        throw hresult_not_implemented();
    }
    Windows::Foundation::Collections::IVectorView<VirtualDesktops::VirtualDesktop> VirtualDesktopManager::VirtualDesktops()
    {
        throw hresult_not_implemented();
    }
    void VirtualDesktopManager::VirtualDesktops(Windows::Foundation::Collections::IVectorView<VirtualDesktops::VirtualDesktop> const& value)
    {
        throw hresult_not_implemented();
    }
    VirtualDesktops::VirtualDesktop VirtualDesktopManager::CurrentDesktop()
    {
        throw hresult_not_implemented();
    }
    void VirtualDesktopManager::CurrentDesktop(VirtualDesktops::VirtualDesktop const& value)
    {
        throw hresult_not_implemented();
    }
    VirtualDesktops::VirtualDesktop VirtualDesktopManager::GetAdjacentDesktop(VirtualDesktops::VirtualDesktopAdjacentDirection const& direction)
    {
        throw hresult_not_implemented();
    }
    VirtualDesktops::VirtualDesktop VirtualDesktopManager::GetAdjacentDesktop(VirtualDesktops::VirtualDesktopAdjacentDirection const& direction, VirtualDesktops::VirtualDesktop const& desktopReference)
    {
        throw hresult_not_implemented();
    }
    bool VirtualDesktopManager::TrySwitchToDesktop(VirtualDesktops::VirtualDesktop const& newDesktop)
    {
        throw hresult_not_implemented();
    }
    VirtualDesktops::VirtualDesktop VirtualDesktopManager::AddDesktop(VirtualDesktops::VirtualDesktopAdjacentDirection const& direction)
    {
        throw hresult_not_implemented();
    }
    VirtualDesktops::VirtualDesktop VirtualDesktopManager::AddDesktop(VirtualDesktops::VirtualDesktopAdjacentDirection const& direction, VirtualDesktops::VirtualDesktop const& desktopReference)
    {
        throw hresult_not_implemented();
    }
    bool VirtualDesktopManager::CanRemoveDesktop(VirtualDesktops::VirtualDesktop const& desktop)
    {
        throw hresult_not_implemented();
    }
    bool VirtualDesktopManager::TryRemoveDesktop(VirtualDesktops::VirtualDesktop const& desktop, VirtualDesktops::VirtualDesktop const& newTargetDesktop)
    {
        throw hresult_not_implemented();
    }
    VirtualDesktops::VirtualDesktop VirtualDesktopManager::GetById(hstring const& virtualDesktopId)
    {
        throw hresult_not_implemented();
    }
    void VirtualDesktopManager::MoveWindowToDesktop(VirtualDesktops::TopLevelWindow const& window, VirtualDesktops::VirtualDesktop const& desktop)
    {
        throw hresult_not_implemented();
    }
    void VirtualDesktopManager::MoveApplicationToDesktop(VirtualDesktops::ApplicationId const& appId, VirtualDesktops::VirtualDesktop const& desktop)
    {
        throw hresult_not_implemented();
    }
    bool VirtualDesktopManager::PinApplication(VirtualDesktops::ApplicationId const& appId)
    {
        throw hresult_not_implemented();
    }
    bool VirtualDesktopManager::PinWindow(VirtualDesktops::TopLevelWindow const& window)
    {
        throw hresult_not_implemented();
    }
    bool VirtualDesktopManager::UnpinApplication(VirtualDesktops::ApplicationId const& appId)
    {
        throw hresult_not_implemented();
    }
    bool VirtualDesktopManager::UnpinWindow(VirtualDesktops::TopLevelWindow const& window)
    {
        throw hresult_not_implemented();
    }
    bool VirtualDesktopManager::IsApplicationPinned(VirtualDesktops::ApplicationId const& appId)
    {
        throw hresult_not_implemented();
    }
    bool VirtualDesktopManager::IsWindowPinned(VirtualDesktops::TopLevelWindow const& window)
    {
        throw hresult_not_implemented();
    }
}
