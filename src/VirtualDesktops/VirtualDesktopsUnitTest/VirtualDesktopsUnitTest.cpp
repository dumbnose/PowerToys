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

			auto currentDesktopChangedCookie = vdmi.CurrentDesktopChanged.register_listener([](const VirtualDesktopManagerInternal& src, const VirtualDesktopChangedEventArgs& args) {
				OutputDebugString(L"Current virtual desktop changed\n");
			});

			auto desktopAddedCookie = vdmi.VirtualDesktopAdded.register_listener([](const VirtualDesktopManagerInternal& src, const VirtualDesktopAddedEventArgs& args) {
				OutputDebugString(L"Virtual desktop added\n");
			});

			auto desktopRemovedCookie = vdmi.VirtualDesktopRemoved.register_listener([](const VirtualDesktopManagerInternal& src, const VirtualDesktopRemovedEventArgs& args) {
				OutputDebugString(L"Virtual desktop removed\n");
			});

			auto windowChangedDesktopCookie = vdmi.WindowChangedDesktops.register_listener([](const VirtualDesktopManagerInternal& src, const WindowChangedDesktopEventArgs& args) {
				OutputDebugString(L"Window changed desktop\n");
			});

			Sleep(10000);
		}
	};
}
