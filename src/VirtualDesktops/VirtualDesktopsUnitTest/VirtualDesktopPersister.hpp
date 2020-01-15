#pragma once


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

	// State persistence
	void LoadPreviousVirtualDesktopMappings();
	void SaveVirtualDesktopMappings();

	// View management
	bool IsViewExempt(std::wstring& aumid);
	bool IsViewKnown(std::wstring& aumid);
	std::shared_ptr<VirtualDesktop> LookupPreviousVirtualDesktopForView(std::wstring& aumid);

	const wchar_t* keyRoot = L"VirtualDesktopManagerUnitTests";

private:
	std::list<std::wstring> knownViews_;
	std::list<std::wstring> exemptViews_ = {L"", L"Microsoft.Windows.Shell.RunDialog"};
	std::map<std::wstring, std::wstring> viewToVirtualDesktop_;
	std::unique_ptr<VirtualDesktopManagerInternal> vdmi_;
	dumbnose::event_source<VirtualDesktopManagerInternal&, WindowChangedDesktopEventArgs&>::cookie_t windowChangedDesktopCookie_;
};

