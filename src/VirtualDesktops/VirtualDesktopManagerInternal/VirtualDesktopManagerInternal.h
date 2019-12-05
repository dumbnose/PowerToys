#pragma once

#include <list>
#include <dumbnose/event_source.hpp>


class TopLevelWindow
{
	int32_t HWnd;
};


class ApplicationId
{
	std::string AUMID;
};


class WindowChangedDesktopEventArgs
{
	TopLevelWindow Window;
};


class VirtualDesktop
{
public:
	std::string Id;

	bool IsWindowVisible(TopLevelWindow window);
	bool IsWindowPresent(TopLevelWindow window);
	bool IsApplicationPresent(ApplicationId app);

	std::list<ApplicationId> Applications;
	std::list<TopLevelWindow> Windows;
};


class VirtualDesktopChangedEventArgs
{
		VirtualDesktop OriginalDesktop;
		VirtualDesktop NewDesktop;
};


class VirtualDesktopAddedEventArgs
{
	VirtualDesktop Desktop;
};


class VirtualDesktopRemovedEventArgs
{
	VirtualDesktop RemovedDesktop;
};


enum class VirtualDesktopAdjacentDirection
{
	Left = 3,
	Right = 4
};


class VirtualDesktopManagerInternal
{
public:
	VirtualDesktopManagerInternal();

	dumbnose::event_source<VirtualDesktopManagerInternal, VirtualDesktopChangedEventArgs> CurrentDesktopChanged;
	dumbnose::event_source<VirtualDesktopManagerInternal, VirtualDesktopAddedEventArgs> VirtualDesktopAdded;
	dumbnose::event_source<VirtualDesktopManagerInternal, VirtualDesktopRemovedEventArgs> VirtualDesktopRemoved;
	dumbnose::event_source<VirtualDesktopManagerInternal, WindowChangedDesktopEventArgs> WindowChangedDesktops;

	std::list<VirtualDesktop> VirtualDesktops;
	std::shared_ptr<VirtualDesktop> CurrentDesktop();

	std::shared_ptr<VirtualDesktop> GetAdjacentDesktop(VirtualDesktopAdjacentDirection direction);
	std::shared_ptr<VirtualDesktop> GetAdjacentDesktop(VirtualDesktopAdjacentDirection direction, const VirtualDesktop& desktopReference);

	bool TrySwitchToDesktop(VirtualDesktop& newDesktop);
	std::shared_ptr<VirtualDesktop> AddDesktop(VirtualDesktopAdjacentDirection direction);
	std::shared_ptr<VirtualDesktop> AddDesktop(VirtualDesktopAdjacentDirection direction, const VirtualDesktop& desktopReference);
	bool CanRemoveDesktop(const VirtualDesktop& desktop);
	bool TryRemoveDesktop(VirtualDesktop& desktop, VirtualDesktop& newTargetDesktop);
	std::shared_ptr<VirtualDesktop> GetById(std::string& virtualDesktopId);

	void MoveWindowToDesktop(TopLevelWindow& window, VirtualDesktop& desktop);
	void MoveApplicationToDesktop(ApplicationId& appId, VirtualDesktop& desktop);

	// Why are these on the Manager and not on a VirtualDesktop? I assume you're pinning them to a specific desktop?
	// Also, no events for when the user does this via the UI?
	// Do any of these need to be 'Try'?
	bool PinApplication(ApplicationId& appId);
	bool PinWindow(TopLevelWindow& window);
	bool UnpinApplication(ApplicationId& appId);
	bool UnpinWindow(TopLevelWindow& window);
	bool IsApplicationPinned(const ApplicationId& appId);
	bool IsWindowPinned(const TopLevelWindow& window);

private:
	IServiceProvider* immersiveShellServiceProvider_ = nullptr;
	IVirtualDesktopManagerInternal* desktopManagerInternal_ = nullptr;
	IVirtualDesktopManager* desktopManager_ = nullptr;
	IApplicationViewCollection* viewCollection_ = nullptr;
	IVirtualDesktopPinnedApps* pinnedApps_ = nullptr;
	IVirtualDesktopNotificationService* desktopNotificationService_ = nullptr;
};
