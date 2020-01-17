#include "pch.h"
#include "CppUnitTest.h"
#include <algorithm>
#include <list>
#include <map>
#include <sstream>
#include "VirtualDesktopPersister.hpp"
#include "ViewToVirtualDesktopMapper.hpp"


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
						<< L"Original Desktop:  " << args.OriginalDesktop.Id << std::endl
						<< L"New Desktop:       " << args.NewDesktop.Id << std::endl
						<< L"Current Desktop:   " << vdmi.CurrentDesktop()->Id << std::endl;

				OutputDebugString(message.str().c_str());
			});

			auto desktopAddedCookie = vdmi.VirtualDesktopAdded.register_listener([&](const VirtualDesktopManagerInternal& src, const VirtualDesktopAddedEventArgs& args) {
				OutputDebugString(L"Virtual desktop added\n");
			});

			auto desktopRemovedCookie = vdmi.VirtualDesktopRemoved.register_listener([&](const VirtualDesktopManagerInternal& src, const VirtualDesktopRemovedEventArgs& args) {
				OutputDebugString(L"Virtual desktop removed\n");
			});

			auto windowChangedDesktopCookie = vdmi.WindowChangedDesktops.register_listener([&](const VirtualDesktopManagerInternal& src, const WindowChangedDesktopEventArgs& args) {

				auto desktop = vdmi.GetById(args.Window.GetVirtualDesktopId());

				std::wstringstream message;
				if (desktop != nullptr) {
					message << L"Window changed desktop" << std::endl
							<< L"AUMID:  " << args.Window.GetAppUserModelId() << std::endl
							<< L"VDID:   " << desktop->Id << std::endl;
				} else {
					message << L"Window removed" << std::endl
							<< L"AUMID:  " << args.Window.GetAppUserModelId() << std::endl;
				}

				OutputDebugString(message.str().c_str());
			});

			CycleThroughDesktops(vdmi);

			Sleep(1000);
		}

		TEST_METHOD(TestEnumeration)
		{
			VirtualDesktopManagerInternal vdmi;
			auto desktops = vdmi.VirtualDesktops();

			std::wstringstream message;
			message << L"Virtual Desktops:\n";
			for (auto& desktop : desktops) {
				message << L"Desktop: " << desktop.Id << std::endl;
			}

			OutputDebugString(message.str().c_str());
		}

		void CycleThroughDesktops(VirtualDesktopManagerInternal& vdmi)
		{
			auto originalDesktop = vdmi.CurrentDesktop();
			Assert::IsNotNull(originalDesktop.get());

			auto desktops = vdmi.VirtualDesktops();
			for (auto& desktop : desktops) {
				bool switched = vdmi.TrySwitchToDesktop(desktop);
				Assert::IsTrue(switched, L"TrySwitchToDesktop()");
				Sleep(2000);
			}

			bool switched = vdmi.TrySwitchToDesktop(*originalDesktop);
			Assert::IsTrue(switched, L"TrySwitchToDesktop()");
		}

		TEST_METHOD(TestAdjacentDesktops)
		{
			VirtualDesktopManagerInternal vdmi;

			// Create a new desktop to the right (currently, they always get appended to the right) so that it exists
			auto newDesktop = vdmi.AddDesktop(VirtualDesktopAdjacentDirection::Right);
			Assert::IsNotNull(newDesktop.get());

			auto originalDesktop = vdmi.CurrentDesktop();
			Assert::IsNotNull(originalDesktop.get());

			auto rightDesktop = vdmi.GetAdjacentDesktop(VirtualDesktopAdjacentDirection::Right);
			Assert::IsNotNull(rightDesktop.get());

			std::wstringstream message;
			message << L"Current Desktop:  " << originalDesktop->Id << std::endl
					<< L"Right Desktop:    " << rightDesktop->Id    << std::endl;

			bool switched = vdmi.TrySwitchToDesktop(*rightDesktop);
			Assert::IsTrue(switched, L"TrySwitchToDesktop()");
			Assert::AreEqual(vdmi.CurrentDesktop()->Id, rightDesktop->Id);
			Sleep(1000);
			message << L"Current Desktop:  " << vdmi.CurrentDesktop()->Id << std::endl;

			switched = vdmi.TrySwitchToDesktop(*originalDesktop);
			Assert::IsTrue(switched, L"TrySwitchToDesktop()");
			Assert::AreEqual(vdmi.CurrentDesktop()->Id, originalDesktop->Id);
			Sleep(1000);
			message << L"Current Desktop:  " << vdmi.CurrentDesktop()->Id << std::endl;

			// Clean up new desktop
			bool removed = vdmi.TryRemoveDesktop(*newDesktop, *originalDesktop);
			Assert::IsTrue(removed);
			Sleep(1000);

			OutputDebugString(message.str().c_str());
		}


		TEST_METHOD(TestViewVirtualDesktopManagement)
		{
			try {

				VirtualDesktopPersister::instance().Initialize();

				Sleep(100000);

				VirtualDesktopPersister::instance().Uninitialize();
			}
			catch (std::exception& ex) {
				OutputDebugStringA(ex.what());
			}
		}

		TEST_METHOD(TestVirtualDesktopMapper)
		{
			ViewToVirtualDesktopMapper mapper;

			mapper.LoadMappings();

			mapper.SetVirtualDesktopId(L"aumid1", L"window title 1", L"{275D7711-5032-4C7B-9F76-F4195049AF13}");
			mapper.SetVirtualDesktopId(L"aumid1", L"window title 2", L"{ASDFDGHJ-5032-4C7B-9F76-F4195049AF13}");
			mapper.SetVirtualDesktopId(L"aumid1", L"",				 L"{ASDFDGHJ-5032-4C7B-9F76-F4195049AF13}");
			mapper.SetVirtualDesktopId(L"aumid2", L"window title 1", L"{275D7711-5032-4C7B-9F76-F4195049AF13}");
			mapper.SetVirtualDesktopId(L"aumid2", L"window title 2", L"{ASDFDGHJ-5032-4C7B-9F76-F4195049AF13}");

			auto vdId = mapper.GetVirtualDesktopId(L"aumid1", L"window title 2");
			OutputDebugString((*vdId).data());

			mapper.CheckpointMappings();

			ViewToVirtualDesktopMapper mapper2;
			mapper2.LoadMappings();

			auto vdId2 = mapper2.GetVirtualDesktopId(L"aumid1", L"window title 2");
			OutputDebugString((*vdId2).data());

			Assert::AreEqual((*vdId).data(), (*vdId2).data());
		}

	};
}
