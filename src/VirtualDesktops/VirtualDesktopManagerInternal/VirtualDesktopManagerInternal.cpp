// VirtualDesktopManagerInternal.cpp : Defines the functions for the static library.
//

#include "pch.h"
#include "VirtualDesktopManagerInternal.h"


VirtualDesktopManagerInternal::VirtualDesktopManagerInternal()
{
	HRESULT hr = ::CoInitializeEx(NULL, COINIT_MULTITHREADED);
	if (FAILED(hr)) throw windows_exception(__FUNCTION__ ": CoInitializeEx failed", hr);

	hr = ::CoCreateInstance(CLSID_ImmersiveShell, nullptr, CLSCTX_LOCAL_SERVER, IID_PPV_ARGS(&immersiveShellServiceProvider_));
	if (FAILED(hr)) throw windows_exception(__FUNCTION__ ": CoCreateInstance failed", hr);

	immersiveShellServiceProvider_->QueryService(__uuidof(IApplicationViewCollection), &viewCollection_);
	if (FAILED(hr)) throw windows_exception(__FUNCTION__ ": QueryService(IApplicationViewCollection) failed", hr);

	immersiveShellServiceProvider_->QueryService(__uuidof(IVirtualDesktopManager), &desktopManager_);
	if (FAILED(hr)) throw windows_exception(__FUNCTION__ ": QueryService(IVirtualDesktopManager) failed", hr);

	immersiveShellServiceProvider_->QueryService(CLSID_VirtualDesktopPinnedApps, IID_PPV_ARGS(&pinnedApps_));
	if (FAILED(hr)) throw windows_exception(__FUNCTION__ ": QueryService(CLSID_VirtualDesktopPinnedApps) failed", hr);

	immersiveShellServiceProvider_->QueryService(CLSID_VirtualDesktopManagerInternal, IID_PPV_ARGS(&desktopManagerInternal_));
	if (FAILED(hr)) throw windows_exception(__FUNCTION__ ": QueryService(CLSID_VirtualDesktopManagerInternal) failed", hr);
}

