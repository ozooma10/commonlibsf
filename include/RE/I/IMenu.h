#pragma once

#include "RE/B/BSFixedString.h"
#include "RE/B/BSInputEventUser.h"
#include "RE/B/BSTEvent.h"
#include "RE/S/SWFToCodeFunctionHandler.h"
#include "RE/S/ScaleformGFxMovie.h"
#include "RE/S/ScaleformGFxValue.h"
#include "RE/S/ScaleformMemoryHeap.h"
#include "RE/U/UIMessageQueue.h"

#define SF_MENU_NAME(NAME) \
	static constexpr std::string_view MENU_NAME = NAME

namespace RE
{
	struct UpdateSceneRectEvent;

	class IMenu :
		public SWFToCodeFunctionHandler,           // 00
		public BSInputEventUser,                   // 10
		public BSTEventSink<UpdateSceneRectEvent>  // 50
	{
	public:
		SF_RTTI_VTABLE(IMenu);

		// Bit semantics proven on 1.16.244 (MenuFlagsProbe live capture + static
		// causal RE; evidence in context_repo module ui.menu_flags). NOTE: this
		// field at +0xC0 mixes static per-menu config bits (set in each menu's
		// ctor via SetFlags) with two RUNTIME-TOGGLED state bits: kShowCursor (3)
		// and kAdvancesMovie (6) are set/cleared live as menus show/advance, so a
		// static ctor read will not show them. Unnamed bits stay FlagN (unproven).
		enum Flag : std::uint32_t
		{
			Flag0 = 1 << 0,
			Flag4 = 1 << 4,
			Flag9 = 1 << 9,
			Flag10 = 1 << 10,
			Flag18 = 1 << 18,
			Flag25 = 1 << 25,

			kPausesGame = 1 << 1,              // PROVEN 2026-07-02 (OSF RE module ui.menu_pause, live freeze/resume cycles on 1.16.244):
			                                   // THE simulation-pause flag. On menu open the pump's flag dispatch calls
			                                   // UI_ModifyMenuPauseCounter (130472) -> UI+0x4B4 pause-request counter++;
			                                   // Main::Update then recomputes Main::isGameMenuPaused = (UI+0x4B4>0) ||
			                                   // IsOpen("MainMenu") || g_145FB4B78 every frame. Close decrements
			                                   // symmetrically. Works on a movie-less admitted custom menu; no letterbox
			                                   // (that is kFreezeFrameLatch). Was previously misassigned to bit 27.
			ShowCursor = 1 << 3,               // proven: shows the cursor (runtime-toggled when the menu is shown)
			kAdvancesMovie = 1 << 6,           // proven: UI_AdvanceActiveMenus advance gate (runtime "advance this movie now")
			kModal = 1 << 8,                   // medium: top-of-stack application/modal selector (UI_SelectTopModalMenu 0x14253f580); leading menu-mode / input-ownership bit
			kAdvancesUnderPauseMenu = 1 << 15, // proven: UI_AdvanceActiveMenus advances this menu even while PauseMenu is up
			kFreezeFrameLatch = 1 << 27,       // RENAMED 2026-07-02 (was kPausesGame — wrong: it does NOT pause the sim; live-proven
			                                   // latch set with the calendar still advancing). Freeze-frame/letterbox latch ONLY,
			                                   // and only consulted when this menu is the top kModal menu (OSF RE ui.menu_pause).
		};

		virtual ~IMenu()  // 00
		{
			using func_t = void (*)(IMenu*);
			static REL::Relocation<func_t> func{ ID::IMenu::dtor };
			func(this);
		}

		// override
		virtual bool ShouldHandleEvent(const InputEvent* a_event) override
		{
			using func_t = bool (*)(BSInputEventUser*, const InputEvent*);
			static REL::Relocation<func_t> func{ ID::IMenu::ShouldHandleEvent };
			return func(this, a_event);
		}

		virtual void OnThumbstickEvent(const ThumbstickEvent* a_event) override
		{
			using func_t = void (*)(BSInputEventUser*, const ThumbstickEvent*);
			static REL::Relocation<func_t> func{ ID::IMenu::OnThumbstickEvent };
			return func(this, a_event);
		}

		virtual void OnButtonEvent(const ButtonEvent* a_event) override
		{
			using func_t = void (*)(BSInputEventUser*, const ButtonEvent*);
			static REL::Relocation<func_t> func{ ID::IMenu::OnButtonEvent };
			return func(this, a_event);
		}

		// add
		virtual const char*   GetName() const = 0;       // 03
		virtual const char*   GetRootPath() const = 0;   // 04
		virtual std::uint64_t GetViewScaleMode() = 0;    // 05

		virtual bool LoadMovie(bool a_addEventDispatcher, bool a_arg2)  // 06
		{
			using func_t = decltype(&IMenu::LoadMovie);
			static REL::Relocation<func_t> func{ ID::IMenu::LoadMovie };
			return func(this, a_addEventDispatcher, a_arg2);
		}

		virtual void Unk07() {}  // 07

		virtual UI_MESSAGE_RESULT ProcessMessage(UIMessageData& a_message)  // 08
		{
			using func_t = decltype(&IMenu::ProcessMessage);
			static REL::Relocation<func_t> func{ ID::IMenu::ProcessMessage };
			return func(this, a_message);
		}

		virtual bool Unk09()  // 09
		{
			using func_t = decltype(&IMenu::Unk09);
			static REL::Relocation<func_t> func{ ID::IMenu::Unk09 };
			return func(this);
		}

		virtual bool IsMovieLoaded()  // 0A
		{
			using func_t = decltype(&IMenu::IsMovieLoaded);
			static REL::Relocation<func_t> func{ ID::IMenu::IsMovieLoaded };
			return func(this);
		}

		virtual void Unk_0B(void) {}  // 0B
		virtual void Unk_0C(void) {}  // 0C
		virtual void Unk_0D(void) {}  // 0D

		virtual void OnMenuStackChanged(const BSFixedString& a_topMenuName, bool a_isTopMenu)  // 0E
		{
			using func_t = decltype(&IMenu::OnMenuStackChanged);
			static REL::Relocation<func_t> func{ ID::IMenu::OnMenuStackChanged };
			return func(this, a_topMenuName, a_isTopMenu);
		}

		virtual void OnMenuDisplayStateChanged() {}  // 0F

		virtual void OnAddedToMenuStack()  // 10
		{
			using func_t = decltype(&IMenu::OnAddedToMenuStack);
			static REL::Relocation<func_t> func{ ID::IMenu::OnAddedToMenuStack };
			return func(this);
		}

		virtual void OnRemovedFromMenuStack()  // 11
		{
			using func_t = decltype(&IMenu::OnRemovedFromMenuStack);
			static REL::Relocation<func_t> func{ ID::IMenu::OnRemovedFromMenuStack };
			return func(this);
		}

		virtual std::uint64_t Unk12()  // 12
		{
			using func_t = decltype(&IMenu::Unk12);
			static REL::Relocation<func_t> func{ ID::IMenu::Unk12 };
			return func(this);
		};

		virtual BSFixedString* Unk13()  // 13
		{
			using func_t = decltype(&IMenu::Unk13);
			static REL::Relocation<func_t> func{ ID::IMenu::Unk13 };
			return func(this);
		};

		virtual bool Unk14()  // 14
		{
			return true;
		};

		virtual bool WantsMovieEventForward(const InputEvent* a_event)  // 15
		{
			using func_t = decltype(&IMenu::WantsMovieEventForward);
			static REL::Relocation<func_t> func{ ID::IMenu::WantsMovieEventForward };
			return func(this, a_event);
		};

		virtual bool CanHandleWhenDisabled([[maybe_unused]] const ButtonEvent* a_event)  // 16
		{
			return false;
		};

		virtual bool OnButtonEventRelease([[maybe_unused]] const BSFixedString& a_eventName)  // 17
		{
			return false;
		};

		virtual std::uint64_t Unk18(void* a_arg1, std::uint64_t a_arg2)  // 18
		{
			using func_t = decltype(&IMenu::Unk18);
			static REL::Relocation<func_t> func{ ID::IMenu::Unk18 };
			return func(this, a_arg1, a_arg2);
		}

		virtual std::uint64_t Unk19(void* a_arg1, std::int32_t a_arg2, std::int32_t a_arg3)  // 19
		{
			using func_t = decltype(&IMenu::Unk19);
			static REL::Relocation<func_t> func{ ID::IMenu::Unk19 };
			return func(this, a_arg1, a_arg2, a_arg3);
		}

		virtual float Unk1A()  // 1A
		{
			using func_t = decltype(&IMenu::Unk1A);
			static REL::Relocation<func_t> func{ ID::IMenu::Unk1A };
			return func(this);
		};

		void SetFlags(std::uint32_t a_flags)
		{
			flags |= a_flags;
			flagsUpdated = true;
		}

		void RemoveFlags(std::uint32_t a_flags)
		{
			flags &= ~a_flags;
			flagsUpdated = true;
		}

		SF_SCALEFORM_HEAP_REDEFINE_NEW(IMenu);

		// members
		Scaleform::GFx::Value                 menuObj;       // 058
		Scaleform::Ptr<Scaleform::GFx::Movie> uiMovie;       // 088
		std::uint64_t                         unk090;        // 090
		std::uint64_t                         unk098;        // 098
		std::uint64_t                         unk0A0;        // 0A0
		std::uint32_t                         unk0A8;        // 0A8
		std::uint32_t                         unk0AC;        // 0AC
		BSFixedString                         menuName;      // 0B0
		BSFixedString                         unk0B8;        // 0B8
		std::uint32_t                         flags;         // 0C0
		std::uint32_t                         unk0C4;        // 0C4
		std::uint32_t                         unk0C8;        // 0C8
		std::uint32_t                         unk0CC;        // 0CC
		std::uint16_t                         unk0D0;        // 0D0
		bool                                  flagsUpdated;  // 0D2
		std::uint8_t                          unk0D3;        // 0D3
		std::uint32_t                         unk0D4;        // 0D4
		std::uint64_t                         unk0D8;        // 0D8
		std::uint64_t                         unk0E0;        // 0E0
		std::uint64_t                         unk0E8;        // 0E8
		std::uint64_t                         unk0F0;        // 0F0
		std::uint64_t                         unk0F8;        // 0F8
		std::uint64_t                         unk100;        // 100
		std::uint32_t                         unk108;        // 108
		std::uint32_t                         unk10C;        // 10C
		std::uint64_t                         unk110;        // 110
		std::uint64_t                         unk118;        // 118
		std::uint64_t                         unk120;        // 120
		std::uint64_t                         unk128;        // 128
		std::uint64_t                         unk130;        // 130
	};
	static_assert(offsetof(IMenu, uiMovie) == 0x088);
	static_assert(offsetof(IMenu, menuName) == 0x0B0);
	static_assert(offsetof(IMenu, flags) == 0x0C0);
	static_assert(offsetof(IMenu, flagsUpdated) == 0x0D2);
}
