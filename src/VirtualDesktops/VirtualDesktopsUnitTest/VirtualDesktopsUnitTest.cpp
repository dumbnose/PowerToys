#include "pch.h"
#include "CppUnitTest.h"
#include <sstream>


using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace VirtualDesktopsUnitTest
{
	TEST_CLASS(VirtualDesktopsUnitTest)
	{
	public:
		
		TEST_METHOD(TestEventing)
		{
			VirtualDesktopManagerInternal vdmi;

			auto currentDesktopChangedCookie = vdmi.CurrentDesktopChanged.register_listener([&](const VirtualDesktopManagerInternal& src, const VirtualDesktopChangedEventArgs& args) {

				std::wstringstream message;
				message << L"Current virtual desktop changed" << std::endl
						<< "Original Desktop:  " << args.OriginalDesktop.Id << std::endl
						<< "New Desktop:       " << args.NewDesktop.Id << std::endl
						<< "Current Desktop:   " << vdmi.CurrentDesktop()->Id << std::endl;


				OutputDebugString(message.str().c_str());
			});

			auto desktopAddedCookie = vdmi.VirtualDesktopAdded.register_listener([&](const VirtualDesktopManagerInternal& src, const VirtualDesktopAddedEventArgs& args) {
				OutputDebugString(L"Virtual desktop added\n");
			});

			auto desktopRemovedCookie = vdmi.VirtualDesktopRemoved.register_listener([&](const VirtualDesktopManagerInternal& src, const VirtualDesktopRemovedEventArgs& args) {
				OutputDebugString(L"Virtual desktop removed\n");
			});

			auto windowChangedDesktopCookie = vdmi.WindowChangedDesktops.register_listener([&](const VirtualDesktopManagerInternal& src, const WindowChangedDesktopEventArgs& args) {
				OutputDebugString(L"Window changed desktop\n");
			});

			Sleep(10000);
		}
	};
}
