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
	if (FAILED(hr)) throw windows_exception(__FUNCTION__ ": CoCreateInstance() failed", hr);

	hr = immersiveShellServiceProvider_->QueryService(__uuidof(IVirtualDesktopManager), desktopManager_.put());
	if (FAILED(hr)) throw windows_exception(__FUNCTION__ ": QueryService(IVirtualDesktopManager) failed", hr);

	hr = immersiveShellServiceProvider_->QueryService(CLSID_VirtualDesktopManagerInternal, __uuidof(desktopManagerInternal_), desktopManagerInternal_.put_void());
	if (FAILED(hr)) throw windows_exception(__FUNCTION__ ": QueryService(CLSID_VirtualDesktopManagerInternal) failed", hr);

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
	notificationRegistrationCookie_ = 0;

	Sleep(100);  // After unregistering, events seem to keep coming for a short while, let the queue drain.
}


std::list<VirtualDesktop> VirtualDesktopManagerInternal::VirtualDesktops()
{
	std::list<VirtualDesktop> desktops;
	winrt::com_ptr<IObjectArray> comDesktops;
	HRESULT hr = desktopManagerInternal_->GetDesktops(comDesktops.put());
	if (FAILED(hr)) throw windows_exception(__FUNCTION__ ": GetDesktops() failed", hr);

	UINT count = 0;
	comDesktops->GetCount(&count);
	for (UINT i = 0; i < count; i++) {
		winrt::com_ptr<IVirtualDesktop> comDesktop;
		comDesktops->GetAt(i, __uuidof(comDesktop), comDesktop.put_void());
		desktops.push_back(VirtualDesktop(comDesktop.get()));
	}

	return desktops;
}

std::shared_ptr<VirtualDesktop> VirtualDesktopManagerInternal::CurrentDesktop()
{
	IVirtualDesktop* desktop;
	HRESULT hr = desktopManagerInternal_->GetCurrentDesktop(&desktop);
	return std::make_shared<VirtualDesktop>(desktop);
}


bool VirtualDesktopManagerInternal::TrySwitchToDesktop(VirtualDesktop& newDesktop)
{
	return SUCCEEDED(desktopManagerInternal_->SwitchDesktop(newDesktop.ComVirtualDesktop().get()));
}


std::shared_ptr<VirtualDesktop> VirtualDesktopManagerInternal::GetAdjacentDesktop(VirtualDesktopAdjacentDirection direction)
{
	return GetAdjacentDesktop(direction, *this->CurrentDesktop());
}


std::shared_ptr<VirtualDesktop> VirtualDesktopManagerInternal::GetAdjacentDesktop(VirtualDesktopAdjacentDirection direction, VirtualDesktop& desktopReference)
{
	AdjacentDesktop adjacency = VirtualDesktopAdjacentDirectionOps::Convert(direction);
	winrt::com_ptr<IVirtualDesktop> comDesktop;
	desktopManagerInternal_->GetAdjacentDesktop(desktopReference.ComVirtualDesktop().get(), adjacency, comDesktop.put());
	std::shared_ptr<VirtualDesktop> desktop = std::make_shared<VirtualDesktop>(comDesktop.get());

	return desktop;
}

std::shared_ptr<VirtualDesktop> VirtualDesktopManagerInternal::AddDesktop(VirtualDesktopAdjacentDirection direction)
{
	return AddDesktop(direction, *this->CurrentDesktop());
}

std::shared_ptr<VirtualDesktop> VirtualDesktopManagerInternal::AddDesktop(VirtualDesktopAdjacentDirection direction, const VirtualDesktop& desktopReference)
{
	// @todo:  Underlying internal APIs do not currently allow the new desktop to be placed in a direction.  So, for now, just create a new one.
	AdjacentDesktop adjacency = VirtualDesktopAdjacentDirectionOps::Convert(direction);
	winrt::com_ptr<IVirtualDesktop> comDesktop;
	desktopManagerInternal_->CreateDesktop(comDesktop.put());
	std::shared_ptr<VirtualDesktop> desktop = std::make_shared<VirtualDesktop>(comDesktop.get());

	return desktop;
}

bool VirtualDesktopManagerInternal::CanRemoveDesktop(const VirtualDesktop& desktop)
{
	// Assumption is that, if there is more than one desktop, we can remove one
	winrt::com_ptr<IObjectArray> comDesktops;
	HRESULT hr = desktopManagerInternal_->GetDesktops(comDesktops.put());
	if (FAILED(hr)) throw windows_exception(__FUNCTION__ ": GetDesktops() failed", hr);

	UINT count = 0;
	comDesktops->GetCount(&count);

	return count > 1;
}

bool VirtualDesktopManagerInternal::TryRemoveDesktop(VirtualDesktop& desktop, VirtualDesktop& newTargetDesktop)
{
	return SUCCEEDED(desktopManagerInternal_->RemoveDesktop(desktop.ComVirtualDesktop().get(), newTargetDesktop.ComVirtualDesktop().get()));
}

std::shared_ptr<VirtualDesktop> VirtualDesktopManagerInternal::GetById(GUID& virtualDesktopId)
{
	GUID nullGuid = { 0 };
	if (virtualDesktopId == nullGuid) return std::shared_ptr<VirtualDesktop>();

	winrt::com_ptr<IVirtualDesktop> comDesktop;
	HRESULT hr = desktopManagerInternal_->FindDesktop(&virtualDesktopId, comDesktop.put());
	if (FAILED(hr)) throw windows_exception(__FUNCTION__ ": FindDesktop() failed", hr);

	return std::make_shared<VirtualDesktop>(comDesktop.get());
}

std::shared_ptr<VirtualDesktop> VirtualDesktopManagerInternal::GetById(std::wstring_view virtualDesktopId)
{
	GUID vdId = { 0 };
	HRESULT hr = CLSIDFromString(virtualDesktopId.data(), &vdId);
	if (FAILED(hr)) throw windows_exception(__FUNCTION__ ": CLSIDFromString() failed", hr);

	return GetById(vdId);
}

bool VirtualDesktopManagerInternal::TryMoveWindowToDesktop(TopLevelWindow& window, VirtualDesktop& desktop)
{
	return SUCCEEDED(desktopManagerInternal_->MoveViewToDesktop(window.View().get(), desktop.ComVirtualDesktop().get()));
}

// @todo:  Implement Application-level functionality
//void VirtualDesktopManagerInternal::MoveApplicationToDesktop(ApplicationId& appId, VirtualDesktop& desktop)
//{
//	throw windows_exception(__FUNCTION__ ": Not Implemented", E_NOTIMPL);
//}

bool VirtualDesktopManagerInternal::PinApplication(ApplicationId& appId)
{
	return SUCCEEDED(pinnedApps_->PinAppID(appId.AUMID.c_str()));
}

bool VirtualDesktopManagerInternal::PinWindow(TopLevelWindow& window)
{
	return SUCCEEDED(pinnedApps_->PinView(window.View().get()));
}

bool VirtualDesktopManagerInternal::UnpinApplication(ApplicationId& appId)
{
	return SUCCEEDED(pinnedApps_->UnpinAppID(appId.AUMID.c_str()));
}

bool VirtualDesktopManagerInternal::UnpinWindow(TopLevelWindow& window)
{
	return SUCCEEDED(pinnedApps_->UnpinView(window.View().get()));
}

bool VirtualDesktopManagerInternal::IsApplicationPinned(const ApplicationId& appId)
{
	BOOL pinned = FALSE;
	HRESULT hr = pinnedApps_->IsAppIdPinned(appId.AUMID.c_str(), &pinned);
	if (FAILED(hr)) throw windows_exception(__FUNCTION__ ": IsAppIdPinned() failed", hr);

	return pinned;
}

bool VirtualDesktopManagerInternal::IsWindowPinned(TopLevelWindow& window)
{
	BOOL pinned = FALSE;
	HRESULT hr = pinnedApps_->IsViewPinned(window.View().get(), &pinned);
	if (FAILED(hr)) throw windows_exception(__FUNCTION__ ": IsViewPinned() failed", hr);

	return pinned;
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

