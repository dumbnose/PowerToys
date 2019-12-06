#include "pch.h"
#include "CppUnitTest.h"


using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace VirtualDesktopsUnitTest
{
	TEST_CLASS(VirtualDesktopsUnitTest)
	{
	public:
		
		TEST_METHOD(TestMethod1)
		{
			VirtualDesktopManagerInternal vdmi;
			uint64_t cookie = vdmi.CurrentDesktopChanged.register_listener([](const VirtualDesktopManagerInternal& src, const VirtualDesktopChangedEventArgs& args) {
				OutputDebugString(L"Event fired\n");
			});
			Sleep(60000);
			vdmi.CurrentDesktopChanged.unregister_listener(cookie);
		}
	};
}
