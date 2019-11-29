#pragma once

#include "VirtualDesktop.g.h"

namespace winrt::VirtualDesktops::implementation
{
	struct VirtualDesktop : VirtualDesktopT<VirtualDesktop>
	{
		VirtualDesktop() = default;

		winrt::hstring Id();
		void Id(winrt::hstring value);
	};
}

namespace winrt::VirtualDesktops::factory_implementation
{
	struct VirtualDesktop : VirtualDesktopT<VirtualDesktop, implementation::VirtualDesktop>
	{
	};
}
