#pragma once

#include <list>
#include <dumbnose/event_source.hpp>
#include "TopLevelWindow.hpp"
#include "VirtualDesktop.hpp"


struct WindowChangedDesktopEventArgs
{
	TopLevelWindow Window;
};


struct VirtualDesktopChangedEventArgs
{
	VirtualDesktop& OriginalDesktop;
	VirtualDesktop& NewDesktop;
};


struct VirtualDesktopAddedEventArgs
{
	VirtualDesktop& Desktop;
};


struct VirtualDesktopRemovedEventArgs
{
	VirtualDesktop& RemovedDesktop;
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
	~VirtualDesktopManagerInternal();

	typedef dumbnose::event_source<const VirtualDesktopManagerInternal&, const VirtualDesktopChangedEventArgs&> CurrentDesktopChanged_T;
	CurrentDesktopChanged_T CurrentDesktopChanged;
	typedef dumbnose::event_source<const VirtualDesktopManagerInternal&, const VirtualDesktopAddedEventArgs&> VirtualDesktopAdded_T;
	VirtualDesktopAdded_T VirtualDesktopAdded;
	typedef dumbnose::event_source<const VirtualDesktopManagerInternal&, const VirtualDesktopRemovedEventArgs&> VirtualDesktopRemoved_T;
	VirtualDesktopRemoved_T VirtualDesktopRemoved;
	typedef dumbnose::event_source<const VirtualDesktopManagerInternal&, const WindowChangedDesktopEventArgs&> WindowChangedDesktops_T;
	WindowChangedDesktops_T WindowChangedDesktops;

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

protected:
	void InitializeWindowManagerComObjects();
	void InitializeEventHandlers();
	void UninitializeEventHandlers();

	struct VirtualDesktopNotifier : public winrt::implements<VirtualDesktopNotifier, IVirtualDesktopNotification>
	{
		VirtualDesktopManagerInternal* vdmi_;
		CurrentDesktopChanged_T* currentDesktopChanged_;
		VirtualDesktopAdded_T* virtualDesktopAdded_;
		VirtualDesktopRemoved_T* virtualDesktopRemoved_;
		WindowChangedDesktops_T* windowChangedDesktops_;

		VirtualDesktopNotifier(VirtualDesktopManagerInternal* vdmi,
							   CurrentDesktopChanged_T* desktopChanged,
							   VirtualDesktopAdded_T* virtualDesktopAdded,
							   VirtualDesktopRemoved_T* virtualDesktopRemoved,
							   WindowChangedDesktops_T* windowChangedDesktops
		);

		virtual HRESULT STDMETHODCALLTYPE VirtualDesktopCreated(IVirtualDesktop* pDesktop);
		virtual HRESULT STDMETHODCALLTYPE VirtualDesktopDestroyBegin(IVirtualDesktop* pDesktopDestroyed, IVirtualDesktop* pDesktopFallback);
		virtual HRESULT STDMETHODCALLTYPE VirtualDesktopDestroyFailed(IVirtualDesktop* pDesktopDestroyed, IVirtualDesktop* pDesktopFallback);
		virtual HRESULT STDMETHODCALLTYPE VirtualDesktopDestroyed(IVirtualDesktop* pDesktopDestroyed, IVirtualDesktop* pDesktopFallback);
		virtual HRESULT STDMETHODCALLTYPE ViewVirtualDesktopChanged(IApplicationView* view);
		virtual HRESULT STDMETHODCALLTYPE CurrentVirtualDesktopChanged(IVirtualDesktop* pDesktopOld, IVirtualDesktop* pDesktopNew);
	};



private:
	winrt::com_ptr<IServiceProvider> immersiveShellServiceProvider_;
	winrt::com_ptr<IVirtualDesktopManagerInternal> desktopManagerInternal_;
	winrt::com_ptr<IVirtualDesktopManager> desktopManager_;
	winrt::com_ptr<IApplicationViewCollection> viewCollection_;
	winrt::com_ptr<IVirtualDesktopPinnedApps> pinnedApps_;
	winrt::com_ptr<IVirtualDesktopNotificationService> desktopNotificationService_;

	DWORD notificationRegistrationCookie_ = 0;
	winrt::com_ptr<IVirtualDesktopNotification> notifier_;
};
