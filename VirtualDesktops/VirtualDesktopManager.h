#pragma once
#include "VirtualDesktopManager.g.h"


namespace winrt::VirtualDesktops::implementation
{
	struct VirtualDesktopManager : VirtualDesktopManagerT<VirtualDesktopManager>
	{
		VirtualDesktopManager();

		winrt::event_token CurrentDesktopChanged(Windows::Foundation::EventHandler<VirtualDesktops::VirtualDesktopChangedEventArgs> const& handler);
		void CurrentDesktopChanged(winrt::event_token const& token) noexcept;
		winrt::event_token VirtualDesktopAdded(Windows::Foundation::EventHandler<VirtualDesktops::VirtualDesktopAddedEventArgs> const& handler);
		void VirtualDesktopAdded(winrt::event_token const& token) noexcept;
		winrt::event_token VirtualDesktopRemoved(Windows::Foundation::EventHandler<VirtualDesktops::VirtualDesktopRemovedEventArgs> const& handler);
		void VirtualDesktopRemoved(winrt::event_token const& token) noexcept;
		winrt::event_token WindowChangedDesktops(Windows::Foundation::EventHandler<VirtualDesktops::WindowChangedDesktopEventArgs> const& handler);
		void WindowChangedDesktops(winrt::event_token const& token) noexcept;
		Windows::Foundation::Collections::IVectorView<VirtualDesktops::VirtualDesktop> VirtualDesktops();
		void VirtualDesktops(Windows::Foundation::Collections::IVectorView<VirtualDesktops::VirtualDesktop> const& value);
		VirtualDesktops::VirtualDesktop CurrentDesktop();
		void CurrentDesktop(VirtualDesktops::VirtualDesktop const& value);
		VirtualDesktops::VirtualDesktop GetAdjacentDesktop(VirtualDesktops::VirtualDesktopAdjacentDirection const& direction);
		VirtualDesktops::VirtualDesktop GetAdjacentDesktop(VirtualDesktops::VirtualDesktopAdjacentDirection const& direction, VirtualDesktops::VirtualDesktop const& desktopReference);
		bool TrySwitchToDesktop(VirtualDesktops::VirtualDesktop const& newDesktop);
		VirtualDesktops::VirtualDesktop AddDesktop(VirtualDesktops::VirtualDesktopAdjacentDirection const& direction);
		VirtualDesktops::VirtualDesktop AddDesktop(VirtualDesktops::VirtualDesktopAdjacentDirection const& direction, VirtualDesktops::VirtualDesktop const& desktopReference);
		bool CanRemoveDesktop(VirtualDesktops::VirtualDesktop const& desktop);
		bool TryRemoveDesktop(VirtualDesktops::VirtualDesktop const& desktop, VirtualDesktops::VirtualDesktop const& newTargetDesktop);
		VirtualDesktops::VirtualDesktop GetById(hstring const& virtualDesktopId);
		void MoveWindowToDesktop(VirtualDesktops::TopLevelWindow const& window, VirtualDesktops::VirtualDesktop const& desktop);
		void MoveApplicationToDesktop(VirtualDesktops::ApplicationId const& appId, VirtualDesktops::VirtualDesktop const& desktop);
		bool PinApplication(VirtualDesktops::ApplicationId const& appId);
		bool PinWindow(VirtualDesktops::TopLevelWindow const& window);
		bool UnpinApplication(VirtualDesktops::ApplicationId const& appId);
		bool UnpinWindow(VirtualDesktops::TopLevelWindow const& window);
		bool IsApplicationPinned(VirtualDesktops::ApplicationId const& appId);
		bool IsWindowPinned(VirtualDesktops::TopLevelWindow const& window);
	};
}
namespace winrt::VirtualDesktops::factory_implementation
{
	struct VirtualDesktopManager : VirtualDesktopManagerT<VirtualDesktopManager, implementation::VirtualDesktopManager>
	{
	};
}
