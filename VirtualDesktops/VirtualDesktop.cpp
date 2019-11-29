#include "pch.h"
#include "VirtualDesktop.h"
#include "VirtualDesktop.g.cpp"

namespace winrt::VirtualDesktops::implementation
{
	winrt::hstring VirtualDesktop::Id()
	{
		throw hresult_not_implemented();
	}

	void VirtualDesktop::Id(winrt::hstring /* value */)
	{
		throw hresult_not_implemented();
	}
}
