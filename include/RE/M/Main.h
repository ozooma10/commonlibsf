#pragma once

#include "RE/B/BSTEvent.h"
#include "RE/N/NiSmartPointer.h"
#include "RE/S/SceneGraph.h"
#include "REX/W32/BASE.h"

namespace RE
{
	struct PositionPlayerEvent;

	class Main :
		public BSTEventSink<PositionPlayerEvent>
	{
	public:
		SF_RTTI_VTABLE(Main);

		// The game's top-level window (engine type name unknown: no RTTI, no strings).
		// The engine destroys and recreates the HWND when the window mode changes, so
		// read it when needed instead of caching it.
		struct Window
		{
			REX::W32::HWND hwnd;  // 00
		};

		virtual ~Main();  // 00

		// override
		virtual BSEventNotifyControl ProcessEvent(const PositionPlayerEvent& a_event, BSTEventSource<PositionPlayerEvent>* a_source) override;  // 01

		[[nodiscard]] static Main* GetSingleton()
		{
			static REL::Relocation<Main**> singleton{ ID::Main::Singleton };
			return *singleton;
		}

		[[nodiscard]] static SceneGraph* GetWorldRoot()
		{
			static REL::Relocation<SceneGraph**> ptr{ ID::Main::WorldRoot };
			return *ptr;
		}

		[[nodiscard]] static NiPointer<NiCamera> GetWorldRootCamera()
		{
			return GetWorldRoot()->camera;
		}

		// members
		std::byte pad008[0x020];     // 008
		bool      quitGame;          // 028
		std::byte pad029[0x005];     // 029
		bool      resetGame;         // 02E
		std::byte pad02F[0x001];     // 02F
		Window*   window;            // 030
		std::byte pad038[0x410];     // 038
		bool      isGameMenuPaused;  // 448
	};
	static_assert(offsetof(Main, quitGame) == 0x028);
	static_assert(offsetof(Main, resetGame) == 0x02E);
	static_assert(offsetof(Main, window) == 0x030);
	static_assert(offsetof(Main, isGameMenuPaused) == 0x448);
}
