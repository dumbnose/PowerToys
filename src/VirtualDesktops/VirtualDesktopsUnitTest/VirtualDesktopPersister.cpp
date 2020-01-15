#include "pch.h"
#include "VirtualDesktopPersister.hpp"
#include <dumbnose/registry/key.hpp>
#include "../../modules/fancyzones/lib/RegistryHelpers.h"


// @todo:  Go through all existing views and put them on their previous virtual desktops
bool VirtualDesktopPersister::Initialize()
{
	LoadPreviousVirtualDesktopMappings();

	// Catch all exceptions, since the object can go away if the shell crashes
	try {
		vdmi_ = std::make_unique<VirtualDesktopManagerInternal>();
		windowChangedDesktopCookie_ = vdmi_->WindowChangedDesktops.register_listener([](VirtualDesktopManagerInternal& src, WindowChangedDesktopEventArgs& args) {

			VirtualDesktopPersister::instance().WindowChangedDesktops(src, args);
		});
	} catch (std::exception & ex) {
		OutputDebugStringA(ex.what());
		return false;
	}

	return true;
}


void VirtualDesktopPersister::Uninitialize()
{
	SaveVirtualDesktopMappings();
	vdmi_ = nullptr;
}


void
VirtualDesktopPersister::WindowChangedDesktops(VirtualDesktopManagerInternal& src, WindowChangedDesktopEventArgs& args) {

	auto desktop = vdmi_->GetById(args.Window.GetVirtualDesktopId());

	std::wstring aumid = args.Window.GetAppUserModelId();
	if (IsViewExempt(aumid)) return; // Don't process exempt views

	// View is closing, remove from knownViews
	if (desktop == nullptr) {
		knownViews_.remove(aumid);
		return;
	}

	// Get the HWND for Win32 apps to handle multi-window apps better
	HWND hwnd = 0;
	winrt::com_ptr<IWin32ApplicationView> win32AppView = args.Window.View().try_as<IWin32ApplicationView>();
	if (win32AppView != nullptr) win32AppView->GetWindow(&hwnd);

	std::wstring windowTitle;
	if (hwnd != 0) {
		wchar_t windowTitleBuffer[512];
		int len = GetWindowText(hwnd, windowTitleBuffer, ARRAYSIZE(windowTitleBuffer));
		if (len > 0) windowTitle = windowTitleBuffer;
	}

	std::wstringstream message;
	message << L"AppUserModelId:    "	<< aumid		<< std::endl
			<< L"VirtualDesktopId:  "	<< desktop->Id	<< std::endl
			<< L"HWND:              "	<< hwnd			<< std::endl
			<< L"Window Title:      "	<< windowTitle	<< std::endl;

	// If we have not seen this view before during this run, it is new
	if (!IsViewKnown(aumid)) {

		message << L"New window found" << std::endl;
		knownViews_.push_back(aumid);

		// Check to see where it last was and move it to there, if applicable
		auto previousDesktop = LookupPreviousVirtualDesktopForView(aumid);
		if (previousDesktop) {

			// Check to see if it is already on the correct desktop and move it if it isn't
			if (desktop->Id.compare(previousDesktop->Id) != 0) {

				bool moved = vdmi_->TryMoveWindowToDesktop(args.Window, *previousDesktop);
				message << "Moved to previous window: " << moved << std::endl;
			}
			else {
				message << "Already on correct window" << std::endl;
			}
		}
		else {
			// Remember this view's desktop for later
			viewToVirtualDesktop_[aumid] = desktop->Id;
		}
	}
	else {

		// We have seen this window before, so it is moving between windows
		message << L"Window changed desktop" << std::endl;

		// Remember this value for later
		viewToVirtualDesktop_[aumid] = desktop->Id;
	}

	OutputDebugString(message.str().c_str());
}


void VirtualDesktopPersister::LoadPreviousVirtualDesktopMappings()
{
	auto settingsKey = dumbnose::registry::key::hkcu().create(RegistryHelpers::REG_SETTINGS);
	auto key = settingsKey.create(keyRoot);

	viewToVirtualDesktop_ = key.enum_values_as_strings();
}


void VirtualDesktopPersister::SaveVirtualDesktopMappings()
{
	auto settingsKey = dumbnose::registry::key::hkcu().create(RegistryHelpers::REG_SETTINGS);
	auto key = settingsKey.create(keyRoot);

	key.save_enum_values_as_strings(viewToVirtualDesktop_);
}


// Exempt views are typically system views, e.g. RunDialog, that shouldn't be tracked
bool VirtualDesktopPersister::IsViewExempt(std::wstring& aumid)
{
	return std::find(exemptViews_.begin(), exemptViews_.end(), aumid) != exemptViews_.end();
}


bool VirtualDesktopPersister::IsViewKnown(std::wstring& aumid)
{
	return std::find(knownViews_.begin(), knownViews_.end(), aumid) != knownViews_.end();
}


std::shared_ptr<VirtualDesktop> VirtualDesktopPersister::LookupPreviousVirtualDesktopForView(std::wstring& aumid)
{
	auto mapping = viewToVirtualDesktop_.find(aumid);
	if (mapping == viewToVirtualDesktop_.end()) return nullptr;
	return vdmi_->GetById(mapping->second);
}
