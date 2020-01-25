#pragma once
#include "ViewToVirtualDesktopMapper.hpp"
#include "ViewTracker.hpp"

// @todo:  The shell can crash.  If it does, we need to react and reinitialize, since the event handler registrations are lost.
class VirtualDesktopPersister : public dumbnose::singleton<VirtualDesktopPersister>
{
	friend dumbnose::singleton<VirtualDesktopPersister>;  // It is already the parent class. Weird this is necessary.

public:
	bool Initialize();
	void Uninitialize();

protected:
	VirtualDesktopPersister() = default;
	~VirtualDesktopPersister() = default;

	// Event handlers
	void WindowChangedDesktops(VirtualDesktopManagerInternal& src, WindowChangedDesktopEventArgs& args);

	// View/Window management
	std::shared_ptr<VirtualDesktop> LookupPreviousVirtualDesktopForView(std::wstring_view aumid, std::wstring_view window_title);
	void MoveWindowToPreferredDesktop(TopLevelWindow& window);
	void MoveAllExistingWindowsToPreferredDesktop();

	const wchar_t* keyRoot = L"VirtualDesktopManagerUnitTests";

private:
	ViewTracker trackedViews_;
	ViewToVirtualDesktopMapper viewsToViewsDesktops_;
	std::unique_ptr<VirtualDesktopManagerInternal> vdmi_;
	dumbnose::event_source<VirtualDesktopManagerInternal&, WindowChangedDesktopEventArgs&>::cookie_t windowChangedDesktopCookie_;
};

