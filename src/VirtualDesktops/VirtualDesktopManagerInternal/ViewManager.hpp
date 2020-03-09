#pragma once
#include "TopLevelWindow.hpp"


class ViewManager
{
public:
	ViewManager();
	std::list<TopLevelWindow> GetTopLevelWindows();

protected:
	void InitializeComObjects();

private:
	winrt::com_ptr<IServiceProvider> immersiveShellServiceProvider_;
	winrt::com_ptr<IApplicationViewCollection> viewCollection_;

};
