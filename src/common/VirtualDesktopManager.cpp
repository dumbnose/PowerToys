#include "pch.h"
#include "VirtualDesktopManager.hpp"

namespace VirtualDesktops {


std::shared_ptr<VirtualDesktopManager> VirtualDesktopManager::GetDefault()
{
	// @todo: Probably just want a singleton for this
	return std::make_shared<VirtualDesktopManager>();
}


}
