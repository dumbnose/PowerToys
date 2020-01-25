#include "pch.h"
#include "ViewManager.hpp"


ViewManager::ViewManager()
{
	InitializeComObjects();
}

std::list<TopLevelWindow> ViewManager::GetTopLevelWindows()
{
	std::list<TopLevelWindow> windows;

	winrt::com_ptr<IObjectArray> views;
	HRESULT hr = viewCollection_->GetViews(views.put());
	if (FAILED(hr)) throw windows_exception(__FUNCTION__ ": GetViews() failed", hr);

	UINT count = 0;
	hr = views->GetCount(&count);
	if (FAILED(hr)) throw windows_exception(__FUNCTION__ ": GetCount() failed", hr);

	for(UINT i=0 ; i<count ; i++) {
		winrt::com_ptr<IApplicationView> view;
		hr = views->GetAt(i, __uuidof(view), view.put_void());
		if (FAILED(hr)) throw windows_exception(__FUNCTION__ ": GetAt() failed", hr);

		windows.push_back(TopLevelWindow(view.get()));
	}

	return windows;
}

void
ViewManager::InitializeComObjects()
{
	HRESULT hr = ::CoInitializeEx(nullptr, COINIT_MULTITHREADED);

	hr = ::CoCreateInstance(CLSID_ImmersiveShell, nullptr, CLSCTX_LOCAL_SERVER, __uuidof(immersiveShellServiceProvider_), immersiveShellServiceProvider_.put_void());
	if (FAILED(hr)) throw windows_exception(__FUNCTION__ ": CoCreateInstance() failed", hr);

	hr = immersiveShellServiceProvider_->QueryService(__uuidof(viewCollection_), viewCollection_.put());
	if (FAILED(hr)) throw windows_exception(__FUNCTION__ ": QueryService(IApplicationViewCollection) failed", hr);
}
