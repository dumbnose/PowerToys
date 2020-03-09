#include "pch.h"
#include "VirtualDesktop.hpp"

VirtualDesktop::VirtualDesktop(IVirtualDesktop* desktop)
{
	desktop->AddRef();
	desktop_.attach(desktop);

	// Get the desktop GUID and make that the Id
	GUID id = {0};
	HRESULT hr = desktop->GetID(&id);
	if (FAILED(hr)) throw windows_exception(__FUNCTION__ ": GetID() failed", hr);

	wchar_t guidBuffer[40] = { 0 };
	int len = StringFromGUID2(id, guidBuffer, ARRAYSIZE(guidBuffer));
	Id = guidBuffer;
}
