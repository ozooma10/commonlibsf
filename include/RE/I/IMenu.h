#pragma once

#include "RE/B/BSFixedString.h"
#include "RE/B/BSInputEventUser.h"
#include "RE/B/BSTEvent.h"
#include "RE/C/ControlMap.h"
#include "RE/S/SWFToCodeFunctionHandler.h"
#include "RE/S/ScaleformGFxMovie.h"
#include "RE/S/ScaleformGFxValue.h"
#include "RE/S/ScaleformMemoryHeap.h"
#include "RE/U/UIMessageQueue.h"

#include <cstddef>
#include <span>
#include <type_traits>

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

		using ScaleModeType = Scaleform::GFx::Movie::ScaleModeType;
		using InputContextID = ControlMap::InputContextID;

		// Native byte-vector storage. IMenu's engine constructor/destructor own
		// the buffer; growth uses a size-aware allocator via AddInputContext.
		// Keep this trivially destructible to avoid freeing the buffer twice.
		struct InputContextStorage
		{
			InputContextID* begin;        // 00
			InputContextID* end;          // 08
			InputContextID* capacityEnd;  // 10
		};
		static_assert(sizeof(InputContextID) == 0x1);
		static_assert(sizeof(InputContextStorage) == 0x18);
		static_assert(offsetof(InputContextStorage, end) == 0x08);
		static_assert(offsetof(InputContextStorage, capacityEnd) == 0x10);
		static_assert(std::is_trivially_destructible_v<InputContextStorage>);

		// Named bit semantics checked on 1.16.244 through static analysis and
		// runtime probes; evidence in docs/imenu.md and context_repo ui.menu_*. NOTE: this
		// field at +0xC0 mixes static per-menu config bits (set in each menu's
		// ctor via SetFlags) with two RUNTIME-TOGGLED state bits: kShowCursor (3)
		// and kAdvancesMovie (6) are set/cleared live as menus show/advance, so a
		// static ctor read will not show them. Unnamed bits stay FlagN (unproven).
		enum Flag : std::uint32_t
		{
			Flag9 = 1 << 9,
			Flag10 = 1 << 10,
			Flag18 = 1 << 18,
			Flag25 = 1 << 25,

			kUsesMenuMode = 1 << 0,            // static: contributes to UI+0x4B0; zero/nonzero edges emit MenuModeChangeEvent (docs/imenu.md)
			kPausesGame = 1 << 1,              // PROVEN 2026-07-02 (OSF RE module ui.menu_pause, live freeze/resume cycles on 1.16.244):
			                                   // THE simulation-pause flag. On menu open the pump's flag dispatch calls
			                                   // UI_ModifyMenuPauseCounter (130472) -> UI+0x4B4 pause-request counter++;
			                                   // Main::Update then recomputes Main::isGameMenuPaused = (UI+0x4B4>0) ||
			                                   // IsOpen("MainMenu") || g_145FB4B78 every frame. Close decrements
			                                   // symmetrically. Works on a movie-less admitted custom menu; no letterbox
			                                   // (that is kFreezeFrameLatch). Was previously misassigned to bit 27.
			ShowCursor = 1 << 3,               // proven: shows the cursor (runtime-toggled when the menu is shown)
			kBlocksLowerMenuInput = 1 << 4,   // static: forces input handled before dispatch reaches lower menus; see docs/imenu.md
			kAdvancesMovie = 1 << 6,           // proven: UI_AdvanceActiveMenus advance gate (runtime "advance this movie now")
			kModal = 1 << 8,                   // topmost application-modal selector (UI_SelectTopModalMenu 0x14253f580); input blocking uses bit 4
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
		// LoadMovie passes this 32-bit result to Movie::SetViewScaleMode.
		virtual ScaleModeType GetViewScaleMode() = 0;   // 05

		virtual bool LoadMovie(bool a_addEventDispatcher, bool a_arg2)  // 06
		{
			using func_t = decltype(&IMenu::LoadMovie);
			static REL::Relocation<func_t> func{ ID::IMenu::LoadMovie };
			return func(this, a_addEventDispatcher, a_arg2);
		}

		// Post-creation setup; the open path calls this before stack admission.
		virtual void PostCreate() {}  // 07

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

		// Configure requested contexts during menu setup. The engine applies
		// them to ControlMap; this does not push an input context directly.
		void AddInputContext(InputContextID a_context);

		// The view is invalidated by native updates or AddInputContext growth.
		[[nodiscard]] std::span<const InputContextID> GetInputContexts() const noexcept
		{
			if (!inputContexts.begin) {
				return {};
			}
			return { inputContexts.begin, static_cast<std::size_t>(inputContexts.end - inputContexts.begin) };
		}

		// Optional registration-time check for the native growth routine.
		[[nodiscard]] static bool IsInputContextAppendSupported();

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
		std::uint8_t                          unk0D0;        // 0D0
		bool                                  inputContextsChanged;  // 0D1
		bool                                  flagsUpdated;  // 0D2
		std::uint8_t                          unk0D3;        // 0D3
		std::uint32_t                         unk0D4;        // 0D4
		InputContextStorage                   inputContexts;  // 0D8 - requested contexts; mutate through AddInputContext
		std::uint64_t                         unk0F0;        // 0F0
		std::uint64_t                         unk0F8;        // 0F8
		std::uint64_t                         unk100;        // 100
		std::uint32_t                         unk108;        // 108
		std::uint32_t                         unk10C;        // 10C
		std::uint8_t                          depthPriority;  // 110 - higher values sort above lower ones; set before stacking
		std::byte                             unk111[0x7];    // 111
		std::uint64_t                         unk118;        // 118
		std::uint64_t                         unk120;        // 120
		std::uint64_t                         unk128;        // 128
		std::uint64_t                         unk130;        // 130
	};
	static_assert(offsetof(IMenu, uiMovie) == 0x088);
	static_assert(offsetof(IMenu, menuName) == 0x0B0);
	static_assert(offsetof(IMenu, flags) == 0x0C0);
	static_assert(offsetof(IMenu, inputContextsChanged) == 0x0D1);
	static_assert(offsetof(IMenu, flagsUpdated) == 0x0D2);
	static_assert(offsetof(IMenu, inputContexts) == 0x0D8);
	static_assert(offsetof(IMenu, unk0F0) == 0x0F0);
	static_assert(offsetof(IMenu, depthPriority) == 0x110);
	static_assert(offsetof(IMenu, unk118) == 0x118);
}
