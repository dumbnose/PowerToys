#include "pch.h"
#include "VirtualDesktopPersister.hpp"
#include <dumbnose/registry/key.hpp>
#include "../../modules/fancyzones/lib/RegistryHelpers.h"


// @todo:  Go through all existing views and put them on their previous virtual desktops
bool VirtualDesktopPersister::Initialize()
{
	viewsToViewsDesktops_.LoadMappings();

	// Catch all exceptions, since the object can go away if the shell crashes
	try {
		vdmi_ = std::make_unique<VirtualDesktopManagerInternal>();
		windowChangedDesktopCookie_ = vdmi_->WindowChangedDesktops.register_listener([](VirtualDesktopManagerInternal& src, WindowChangedDesktopEventArgs& args) {

			VirtualDesktopPersister::instance().WindowChangedDesktops(src, args);
		});

		// Handle all windows that were created before we were run
		MoveAllExistingWindowsToPreferredDesktop();

	} catch (std::exception & ex) {
		OutputDebugStringA(ex.what());
		return false;
	}

	return true;
}


void VirtualDesktopPersister::Uninitialize()
{
	viewsToViewsDesktops_.CheckpointMappings();
	vdmi_ = nullptr;
}


void
VirtualDesktopPersister::WindowChangedDesktops(VirtualDesktopManagerInternal& src, WindowChangedDesktopEventArgs& args) 
{
	MoveWindowToPreferredDesktop(args.Window);
}


std::shared_ptr<VirtualDesktop> VirtualDesktopPersister::LookupPreviousVirtualDesktopForView(std::wstring_view aumid, std::wstring_view window_title)
{
	auto vdId = viewsToViewsDesktops_.GetVirtualDesktopId(aumid, L"");
	if (!vdId) return nullptr;

	return vdmi_->GetById(vdId->data());
}

void VirtualDesktopPersister::MoveWindowToPreferredDesktop(TopLevelWindow& window)
{
	auto desktop = vdmi_->GetById(window.GetVirtualDesktopId());

	std::wstring aumid = window.GetAppUserModelId();
	if (trackedViews_.IsViewExempt(aumid)) return; // Don't process exempt views

	// Get the HWND for Win32 apps to handle multi-window apps better
	HWND hwnd = 0;
	winrt::com_ptr<IWin32ApplicationView> win32AppView = window.View().try_as<IWin32ApplicationView>();
	if (win32AppView != nullptr) win32AppView->GetWindow(&hwnd);

	// View is closing, remove from knownViews
	if (desktop == nullptr) {
		trackedViews_.RemoveView(aumid, hwnd);
		return;
	}

	std::wstring windowTitle;
	if (hwnd != 0) {
		wchar_t windowTitleBuffer[512];
		int len = GetWindowText(hwnd, windowTitleBuffer, ARRAYSIZE(windowTitleBuffer));
		if (len > 0) windowTitle = windowTitleBuffer;
	}

	std::wstringstream message;
	message << L"AppUserModelId:    " << aumid << std::endl
		<< L"VirtualDesktopId:  " << desktop->Id << std::endl
		<< L"HWND:              " << hwnd << std::endl
		<< L"Window Title:      " << windowTitle << std::endl;

	// If we have not seen this view before during this run, it is new
	if (!trackedViews_.IsViewKnown(aumid, hwnd)) {

		message << L"New window found" << std::endl;
		trackedViews_.TrackView(aumid, hwnd);

		// Check to see where it last was and move it to there, if applicable
		auto previousDesktop = LookupPreviousVirtualDesktopForView(aumid, L"");
		if (previousDesktop) {

			// Check to see if it is already on the correct desktop and move it if it isn't
			if (desktop->Id.compare(previousDesktop->Id) != 0) {

				bool moved = vdmi_->TryMoveWindowToDesktop(window, *previousDesktop);
				message << "Moved to previous window: " << moved << std::endl;
			}
			else {
				message << "Already on correct window" << std::endl;
			}
		}
		else {
			// Remember this view's desktop for later
			viewsToViewsDesktops_.SetVirtualDesktopId(aumid, L"", desktop->Id);
		}
	}
	else {

		// We have seen this window before, so it is moving between windows
		message << L"Window changed desktop" << std::endl;

		// Remember this value for later
		viewsToViewsDesktops_.SetVirtualDesktopId(aumid, L"", desktop->Id);
	}

	OutputDebugString(message.str().c_str());
}

void VirtualDesktopPersister::MoveAllExistingWindowsToPreferredDesktop()
{
	ViewManager viewManager;
	auto windows = viewManager.GetTopLevelWindows();
	for (auto& window : windows) {
		MoveWindowToPreferredDesktop(window);
	}
}
