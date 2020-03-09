#pragma once
#include "TopLevelWindow.hpp"



struct ApplicationId
{
	std::wstring AUMID;
};


class VirtualDesktop
{
public:
	VirtualDesktop(IVirtualDesktop* desktop);

	std::wstring Id;
	winrt::com_ptr<IVirtualDesktop> ComVirtualDesktop() { return desktop_; }

	bool IsWindowVisible(TopLevelWindow& window);
	bool IsWindowPresent(TopLevelWindow& window);
	bool IsApplicationPresent(ApplicationId& app);

	std::list<ApplicationId> Applications;
	std::list<TopLevelWindow> Windows;

private:
	winrt::com_ptr<IVirtualDesktop> desktop_;
};


