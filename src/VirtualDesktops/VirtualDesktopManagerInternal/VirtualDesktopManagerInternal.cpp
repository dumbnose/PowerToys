// VirtualDesktopManagerInternal.cpp : Defines the functions for the static library.
//

#include "pch.h"
#include "VirtualDesktopManagerInternal.h"


VirtualDesktopManagerInternal::VirtualDesktopManagerInternal()
{
	InitializeWindowManagerComObjects();
	InitializeEventHandlers();
}


VirtualDesktopManagerInternal::~VirtualDesktopManagerInternal()
{
	UninitializeEventHandlers();
}

void VirtualDesktopManagerInternal::InitializeWindowManagerComObjects()
{
	HRESULT hr = ::CoInitializeEx(nullptr, COINIT_MULTITHREADED);

	hr = ::CoCreateInstance(CLSID_ImmersiveShell, nullptr, CLSCTX_LOCAL_SERVER, __uuidof(immersiveShellServiceProvider_), immersiveShellServiceProvider_.put_void());
	if (FAILED(hr)) throw windows_exception(__FUNCTION__ ": CoCreateInstance failed", hr);

	hr = immersiveShellServiceProvider_->QueryService(__uuidof(IVirtualDesktopManager), desktopManager_.put());
	if (FAILED(hr)) throw windows_exception(__FUNCTION__ ": QueryService(IVirtualDesktopManager) failed", hr);

	hr = immersiveShellServiceProvider_->QueryService(CLSID_VirtualDesktopManagerInternal, __uuidof(desktopManagerInternal_), desktopManagerInternal_.put_void());
	if (FAILED(hr)) throw windows_exception(__FUNCTION__ ": QueryService(CLSID_VirtualDesktopManagerInternal) failed", hr);

	//hr = immersiveShellServiceProvider_->QueryService(__uuidof(viewCollection_), &viewCollection_);
	//if (FAILED(hr)) throw windows_exception(__FUNCTION__ ": QueryService(IApplicationViewCollection) failed", hr);

	hr = immersiveShellServiceProvider_->QueryService(CLSID_VirtualDesktopPinnedApps, __uuidof(pinnedApps_), pinnedApps_.put_void());
	if (FAILED(hr)) throw windows_exception(__FUNCTION__ ": QueryService(CLSID_VirtualDesktopPinnedApps) failed", hr);
}

//
// Virtual Desktop Notifications
//
VirtualDesktopManagerInternal::VirtualDesktopNotifier::VirtualDesktopNotifier(VirtualDesktopManagerInternal* vdmi,
																			  CurrentDesktopChanged_T* desktop_changed_event) :
	vdmi_(vdmi),
	desktop_changed_event_(desktop_changed_event)
{

}

HRESULT STDMETHODCALLTYPE 
VirtualDesktopManagerInternal::VirtualDesktopNotifier::VirtualDesktopCreated(IVirtualDesktop* pDesktop)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE 
VirtualDesktopManagerInternal::VirtualDesktopNotifier::VirtualDesktopDestroyBegin(IVirtualDesktop* pDesktopDestroyed, IVirtualDesktop* pDesktopFallback)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE 
VirtualDesktopManagerInternal::VirtualDesktopNotifier::VirtualDesktopDestroyFailed(IVirtualDesktop* pDesktopDestroyed, IVirtualDesktop* pDesktopFallback)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE 
VirtualDesktopManagerInternal::VirtualDesktopNotifier::VirtualDesktopDestroyed(IVirtualDesktop* pDesktopDestroyed, IVirtualDesktop* pDesktopFallback)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE 
VirtualDesktopManagerInternal::VirtualDesktopNotifier::ViewVirtualDesktopChanged(IApplicationView* view)
{
	WindowChangedDesktopEventArgs args{ {view} };
	//WindowChangedDesktops->raise(*vdmi_, args);

	return S_OK;
}

HRESULT STDMETHODCALLTYPE 
VirtualDesktopManagerInternal::VirtualDesktopNotifier::CurrentVirtualDesktopChanged(IVirtualDesktop* oldDesktop, IVirtualDesktop* newDesktop)
{
	auto oldVirtualDesktop = VirtualDesktop(oldDesktop);
	auto newVirtualDesktop = VirtualDesktop(newDesktop);
	VirtualDesktopChangedEventArgs args{ oldVirtualDesktop, newVirtualDesktop };
	desktop_changed_event_->raise(*vdmi_, args);

	return S_OK;
}



void VirtualDesktopManagerInternal::InitializeEventHandlers()
{
	HRESULT hr = immersiveShellServiceProvider_->QueryService(CLSID_IVirtualNotificationService, __uuidof(desktopNotificationService_), desktopNotificationService_.put_void());
	if (FAILED(hr)) throw windows_exception(__FUNCTION__ ": QueryService(CLSID_IVirtualNotificationService) failed", hr);

	notifier_ = winrt::make<VirtualDesktopNotifier>(this, &CurrentDesktopChanged);
	desktopNotificationService_->Register(notifier_.get(), &notificationRegistrationCookie_);
}

void VirtualDesktopManagerInternal::UninitializeEventHandlers()
{
	if (notificationRegistrationCookie_ == 0) return;

	desktopNotificationService_->Unregister(notificationRegistrationCookie_);
}
