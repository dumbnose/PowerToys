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


void VirtualDesktopManagerInternal::InitializeEventHandlers()
{
	HRESULT hr = immersiveShellServiceProvider_->QueryService(CLSID_IVirtualNotificationService, __uuidof(desktopNotificationService_), desktopNotificationService_.put_void());
	if (FAILED(hr)) throw windows_exception(__FUNCTION__ ": QueryService(CLSID_IVirtualNotificationService) failed", hr);

	notifier_ = winrt::make<VirtualDesktopNotifier>(this, &CurrentDesktopChanged, &VirtualDesktopAdded, &VirtualDesktopRemoved, &WindowChangedDesktops);
	desktopNotificationService_->Register(notifier_.get(), &notificationRegistrationCookie_);
}

void VirtualDesktopManagerInternal::UninitializeEventHandlers()
{
	if (notificationRegistrationCookie_ == 0) return;

	desktopNotificationService_->Unregister(notificationRegistrationCookie_);
}


//
// Virtual Desktop Notifications
//
VirtualDesktopManagerInternal::VirtualDesktopNotifier::VirtualDesktopNotifier(
		VirtualDesktopManagerInternal* vdmi,
		CurrentDesktopChanged_T* desktopChanged,
		VirtualDesktopAdded_T* virtualDesktopAdded,
		VirtualDesktopRemoved_T* virtualDesktopRemoved,
		WindowChangedDesktops_T* windowChangedDesktops)	:
	vdmi_(vdmi),
	currentDesktopChanged_(desktopChanged),
	virtualDesktopAdded_(virtualDesktopAdded),
	virtualDesktopRemoved_(virtualDesktopRemoved),
	windowChangedDesktops_(windowChangedDesktops)
{
}

HRESULT STDMETHODCALLTYPE 
VirtualDesktopManagerInternal::VirtualDesktopNotifier::VirtualDesktopCreated(IVirtualDesktop* createdDesktop)
{
	auto createdVirtualDesktop = VirtualDesktop(createdDesktop);
	VirtualDesktopAddedEventArgs args{ createdVirtualDesktop };
	virtualDesktopAdded_->raise(*vdmi_, args);

	return S_OK;
}

// Don't care about beginning of event, just if it succeeds
HRESULT STDMETHODCALLTYPE 
VirtualDesktopManagerInternal::VirtualDesktopNotifier::VirtualDesktopDestroyBegin(IVirtualDesktop* /*destroyedDesktop*/, IVirtualDesktop* /*fallbackDesktop*/)
{
	return S_OK;
}

// Don't care about failed event, just if it succeeds
HRESULT STDMETHODCALLTYPE
VirtualDesktopManagerInternal::VirtualDesktopNotifier::VirtualDesktopDestroyFailed(IVirtualDesktop* /*destroyedDesktop*/, IVirtualDesktop* /*fallbackDesktop*/)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE 
VirtualDesktopManagerInternal::VirtualDesktopNotifier::VirtualDesktopDestroyed(IVirtualDesktop* destroyedDesktop, IVirtualDesktop* /*fallbackDesktop*/)
{
	auto destroyedVirtualDesktop = VirtualDesktop(destroyedDesktop);
	VirtualDesktopRemovedEventArgs args{ destroyedVirtualDesktop };
	virtualDesktopRemoved_->raise(*vdmi_, args);

	return S_OK;
}

HRESULT STDMETHODCALLTYPE 
VirtualDesktopManagerInternal::VirtualDesktopNotifier::ViewVirtualDesktopChanged(IApplicationView* view)
{
	WindowChangedDesktopEventArgs args{ {view} };
	windowChangedDesktops_->raise(*vdmi_, args);

	return S_OK;
}

HRESULT STDMETHODCALLTYPE 
VirtualDesktopManagerInternal::VirtualDesktopNotifier::CurrentVirtualDesktopChanged(IVirtualDesktop* oldDesktop, IVirtualDesktop* newDesktop)
{
	auto oldVirtualDesktop = VirtualDesktop(oldDesktop);
	auto newVirtualDesktop = VirtualDesktop(newDesktop);
	VirtualDesktopChangedEventArgs args{ oldVirtualDesktop, newVirtualDesktop };
	currentDesktopChanged_->raise(*vdmi_, args);

	return S_OK;
}


