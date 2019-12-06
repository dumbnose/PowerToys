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


struct VirtualDesktopChangedEventArgs
{
	//VirtualDesktop* OriginalDesktop;
	//VirtualDesktop* NewDesktop;
	IApplicationView* view;
};


struct VirtualDesktopAddedEventArgs
{
	VirtualDesktop* Desktop;
};


struct VirtualDesktopRemovedEventArgs
{
	VirtualDesktop* RemovedDesktop;
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

	typedef dumbnose::event_source<const VirtualDesktopManagerInternal&, const VirtualDesktopChangedEventArgs&> desktop_changed_event_t;
	desktop_changed_event_t CurrentDesktopChanged;
	dumbnose::event_source<const VirtualDesktopManagerInternal&, const VirtualDesktopAddedEventArgs&> VirtualDesktopAdded;
	dumbnose::event_source<const VirtualDesktopManagerInternal&, const VirtualDesktopRemovedEventArgs&> VirtualDesktopRemoved;
	dumbnose::event_source<const VirtualDesktopManagerInternal&, const WindowChangedDesktopEventArgs&> WindowChangedDesktops;

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
		VirtualDesktopManagerInternal::desktop_changed_event_t* desktop_changed_event_;

		VirtualDesktopNotifier(VirtualDesktopManagerInternal* vdmi,
							   VirtualDesktopManagerInternal::desktop_changed_event_t* desktop_changed_event);

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
