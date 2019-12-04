#pragma once

class VirtualDesktopManagerInternal
{
public:
	VirtualDesktopManagerInternal();

private:
	IServiceProvider* immersiveShellServiceProvider_ = nullptr;
	IVirtualDesktopManagerInternal* desktopManagerInternal_ = nullptr;
	IVirtualDesktopManager* desktopManager_ = nullptr;
	IApplicationViewCollection* viewCollection_ = nullptr;
	IVirtualDesktopPinnedApps* pinnedApps_ = nullptr;
	IVirtualDesktopNotificationService* desktopNotificationService_ = nullptr;
};
