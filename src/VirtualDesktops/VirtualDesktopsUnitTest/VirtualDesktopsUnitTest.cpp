#include "pch.h"
#include "CppUnitTest.h"
#include <algorithm>
#include <list>
#include <map>
#include <sstream>
#include <dumbnose/registry/key.hpp>
#include "../../modules/fancyzones/lib/RegistryHelpers.h"


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

		const wchar_t* keyRoot = L"VirtualDesktopManagerUnitTests";

		void LoadPreviousVirtualDesktopMappings(std::map<std::wstring, std::wstring>& viewToVirtualDesktop)
		{

			auto settingsKey = dumbnose::registry::key::hkcu().open_all_access(RegistryHelpers::REG_SETTINGS);
			auto key = settingsKey.open_all_access(keyRoot);
			
			viewToVirtualDesktop = key.enum_values_as_strings();
		}

		void SaveVirtualDesktopMappings(std::map<std::wstring, std::wstring>& viewToVirtualDesktop)
		{
			auto settingsKey = dumbnose::registry::key::hkcu().open_all_access(RegistryHelpers::REG_SETTINGS);
			auto key = settingsKey.open_all_access(keyRoot);

			key.save_enum_values_as_strings(viewToVirtualDesktop);
		}

		TEST_METHOD(TestViewVirtualDesktopManagement)
		{
			std::list<std::wstring> knownViews;
			std::map<std::wstring, std::wstring> viewToVirtualDesktop;
			LoadPreviousVirtualDesktopMappings(viewToVirtualDesktop);

			VirtualDesktopManagerInternal vdmi;

			auto windowChangedDesktopCookie = vdmi.WindowChangedDesktops.register_listener([&](VirtualDesktopManagerInternal& src, WindowChangedDesktopEventArgs& args) {

				auto desktop = vdmi.GetById(args.Window.GetVirtualDesktopId());

				std::wstring aumid = args.Window.GetAppUserModelId();

				// View is closing, remove from knownViews
				if (desktop == nullptr) {
					knownViews.remove(aumid);
					return;
				}

				std::wstringstream message;
				message << L"AppUserModelId:    " << aumid << std::endl
						<< L"VirtualDesktopId:  " << desktop->Id << std::endl;

				// If we have not seen this view before during this run, it is new
				if (std::find(knownViews.begin(), knownViews.end(),aumid) == knownViews.end()) {

					message << L"New window found" << std::endl;
					knownViews.push_back(aumid);

					// Check to see where it last was and move it to there, if applicable
					auto previousDesktopIter = viewToVirtualDesktop.find(aumid);
					if (previousDesktopIter != viewToVirtualDesktop.end()) {

						// Check to see if it is already on the correct desktop and move it if it isn't
						if (desktop->Id.compare(previousDesktopIter->second) != 0) {
							auto previousDesktop = vdmi.GetById(previousDesktopIter->second);

							bool moved = vdmi.TryMoveWindowToDesktop(args.Window, *previousDesktop);
							message << "Moved to previous window: " << moved << std::endl;
						} else {
							message << "Already on correct window" << std::endl;
						}

					} else {

						// Remember this view's desktop for later
						viewToVirtualDesktop[aumid] = desktop->Id;

					}
				} else {

					// We have seen this window before, so it is moving between windows
					message << L"Window changed desktop" << std::endl;

					// Remember this value for later
					viewToVirtualDesktop[aumid] = desktop->Id;
				}

				OutputDebugString(message.str().c_str());

			});

			Sleep(10000);

			SaveVirtualDesktopMappings(viewToVirtualDesktop);
		}


	};
}
