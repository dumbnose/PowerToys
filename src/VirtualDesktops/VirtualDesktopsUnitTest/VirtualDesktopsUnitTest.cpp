#include "pch.h"
#include "CppUnitTest.h"


using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace VirtualDesktopsUnitTest
{
	TEST_CLASS(VirtualDesktopsUnitTest)
	{
	public:
		
		TEST_METHOD(TestEventing)
		{
			VirtualDesktopManagerInternal vdmi;

			uint64_t currentDesktopChangedCookie = vdmi.CurrentDesktopChanged.register_listener([](const VirtualDesktopManagerInternal& src, const VirtualDesktopChangedEventArgs& args) {
				OutputDebugString(L"Current virtual desktop changed\n");
			});

			uint64_t desktopAddedCookie = vdmi.VirtualDesktopAdded.register_listener([](const VirtualDesktopManagerInternal& src, const VirtualDesktopAddedEventArgs& args) {
				OutputDebugString(L"Virtual desktop added\n");
				});

			uint64_t desktopRemovedCookie = vdmi.VirtualDesktopRemoved.register_listener([](const VirtualDesktopManagerInternal& src, const VirtualDesktopRemovedEventArgs& args) {
				OutputDebugString(L"Virtual desktop removed\n");
				});

			uint64_t windowChangedDesktopCookie = vdmi.WindowChangedDesktops.register_listener([](const VirtualDesktopManagerInternal& src, const WindowChangedDesktopEventArgs& args) {
				OutputDebugString(L"Window changed desktop\n");
				});

			Sleep(100000);

			vdmi.CurrentDesktopChanged.unregister_listener(currentDesktopChangedCookie);
			vdmi.VirtualDesktopAdded.unregister_listener(desktopAddedCookie);
			vdmi.VirtualDesktopRemoved.unregister_listener(desktopRemovedCookie);
			vdmi.WindowChangedDesktops.unregister_listener(windowChangedDesktopCookie);
		}
	};
}
