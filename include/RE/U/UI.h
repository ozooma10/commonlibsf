#pragma once
#include "RE/B/BSInputEventReceiver.h"
#include "RE/B/BSTArray.h"
#include "RE/B/BSTEvent.h"
#include "RE/B/BSTScatterTable.h"
#include "RE/I/IMenu.h"

namespace RE
{
	class IMenu;

	namespace Scaleform
	{
		template <class T>
		class Ptr;
	}

	struct BIUIMenuVisiblePausedBeginEvent;
	struct BIUIMenuVisiblePausedEndEvent;
	struct BSCursorRotationChange;
	struct BSCursorTypeChange;
	struct MenuModeChangeEvent;
	struct MenuOpenCloseEvent;
	struct MenuPauseChangeEvent;
	struct MenuPauseCounterChangeEvent;
	struct TutorialEvent;

	// EXE RTTI (2026-06-09, UILayoutProbe RTTI walk of the live singleton): the
	// binary's BaseClassArray for UI enumerates, with member offsets (mdisp):
	//   +0x00  BSTSingletonSDM<UI, BSTSingletonSDMOpStaticBuffer>
	//   +0x08    BSTSingletonSDMBase<BSTSDMTraits<UI, BSTSingletonSDMOpStaticBuffer<UI>>>
	//   +0x09    BSTSingletonSDMOpStaticBuffer<UI>
	//   +0x10  BSInputEventReceiver
	//   +0x20  BSTEventSource band, 0x28 stride (event order matches below)
	// So in Starfield, BSTSingletonSDM is polymorphic — a vtable was observed at
	// UI+0x00 and the SDM's own bases land at +0x08/+0x09 — making it a 0x10
	// subobject. CommonLibSF's BSTSingletonSDM template (BSTSingleton.h) is an
	// empty, non-polymorphic placeholder, so it cannot be used as the base here
	// without collapsing the layout; this opaque stand-in models the compiled
	// subobject for UI only. Do not generalize to other BSTSingletonSDM users
	// without per-class proof.
	struct BSTSingletonSDM_UI
	{
		virtual ~BSTSingletonSDM_UI() = default;  // 00 — vtable observed at UI+0x00
		std::uint8_t sdm08[8];                    // 08 — SDM bases per RTTI (+0x08/+0x09), padded
	};
	static_assert(sizeof(BSTSingletonSDM_UI) == 0x10);

	class UI :
		public BSTSingletonSDM_UI,                               // 000  exe RTTI: BSTSingletonSDM<UI, BSTSingletonSDMOpStaticBuffer>
		public BSInputEventReceiver,                             // 010  exe RTTI: mdisp 0x10
		public BSTEventSource<MenuOpenCloseEvent>,               // 020
		public BSTEventSource<MenuModeChangeEvent>,              // 048
		public BSTEventSource<MenuPauseChangeEvent>,             // 070
		public BSTEventSource<MenuPauseCounterChangeEvent>,      // 098
		public BSTEventSource<TutorialEvent>,                    // 0C0
		public BSTEventSource<BSCursorTypeChange>,               // 0E8
		public BSTEventSource<BSCursorRotationChange>,           // 110
		public BSTEventSource<BIUIMenuVisiblePausedBeginEvent>,  // 138
		public BSTEventSource<BIUIMenuVisiblePausedEndEvent>     // 160
	{
	public:
		SF_RTTI_VTABLE(UI);

		struct UIMenuEntry
		{
			using Create_t = Scaleform::Ptr<IMenu>*(Scaleform::Ptr<IMenu>*);

			Scaleform::Ptr<IMenu> menu;
			Create_t*             initFunc;
			void*                 unk18 = nullptr;
			uint64_t              unk20 = 1;
			uint64_t              unk28 = 0;
		};

		template <class T>
		[[nodiscard]] auto GetEventSource()
		{
			return static_cast<BSTEventSource<T>*>(this);
		}

		inline static UI* GetSingleton()
		{
			static REL::Relocation<UI**> singleton{ ID::UI::Singleton };
			return *singleton;
		}

		// NOTE (2026-06-09): these name-keyed helpers depended on a `menuMap`
		// BSTHashMap that the previous header placed at UI+0x430. UILayoutProbe
		// proved 0x430 is actually a BSTArray of active menus (see `menuArray`
		// below), and no name->menu hashmap offset has been confirmed yet, so
		// GetMenu/GetMenuMovie/IsMenuRegistered/RegisterMenu are disabled rather
		// than left pointing at a wrong offset. Restore once menuMap is located.
#if 0
		Scaleform::Ptr<IMenu> GetMenu(const BSFixedString& a_menuName) const
		{
			auto it = menuMap.find(a_menuName);
			return it != menuMap.end() ? it->value.menu : nullptr;
		}

		Scaleform::Ptr<Scaleform::GFx::Movie> GetMenuMovie(const BSFixedString& a_menuName) const
		{
			auto menu = GetMenu(a_menuName);
			return menu ? menu->uiMovie : nullptr;
		}
#endif

		bool IsMenuOpen(const BSFixedString& a_name) const
		{
			using func_t = decltype(&UI::IsMenuOpen);
			static REL::Relocation<func_t> func{ ID::UI::IsMenuOpen };
			return func(this, a_name);
		}

#if 0  // see note above — depends on the unlocated menuMap
		bool IsMenuRegistered(const BSFixedString& a_name) const
		{
			return menuMap.contains(a_name);
		}
#endif

		bool IsMenusVisible() const
		{
			return menusVisible;
		}

#if 0  // see note above — depends on the unlocated menuMap
		template <class T>
			requires(std::derived_from<T, IMenu>)
		bool RegisterMenu(const BSFixedString& a_name)
		{
			if (menuMap.contains(a_name)) {
				return false;
			}

			auto RegisterMenuImpl = [](Scaleform::Ptr<IMenu>* a_menu) {
				using func_t = Scaleform::Ptr<IMenu>*(Scaleform::Ptr<IMenu>*, T*);
				static REL::Relocation<func_t> func{ ID::UI::RegisterMenu };
				func(a_menu, new T());
				return a_menu;
			};

			menuMap[a_name].initFunc = RegisterMenuImpl;
			return true;
		}
#endif

		template <class T>
		void RegisterSink(BSTEventSink<T>* a_sink)
		{
			GetEventSource<T>()->RegisterSink(a_sink);
		}

		template <class T>
		void UnregisterSink(BSTEventSink<T>* a_sink)
		{
			GetEventSource<T>()->UnregisterSink(a_sink);
		}

		// The base subobjects above now end at 0x188 (was 0x178 before the
		// UIInputRouterBase fix), so the first opaque member starts at 0x188.
		std::uint8_t                    pad188[0x268];  // 188
		BSTArray<Scaleform::Ptr<IMenu>> menuStack;      // 3F0  (empty in the probe snapshot)
		std::uint8_t                    pad400[0x18];   // 400
		uint64_t                        unk418;         // 418
		uint64_t                        unk420;         // 420
		uint64_t                        unk428;         // 428
		// RUNTIME (UILayoutProbe): the previous header declared a BSTHashMap
		// `menuMap` here, but the live object holds a BSTArray of active menus —
		// count/capacity at +0x430, data ptr at +0x438, with every entry's first
		// qword resolving to an in-module IMenu vtable. This also matches the
		// Ghidra UI_DispatchTranslatedInputToChildReceivers / UI_FindActiveMenuEntryByPointer findings.
		BSTArray<Scaleform::Ptr<IMenu>> menuArray;      // 430
		// 0x440 holds a second BSTArray header (count=0/cap=4 in the snapshot)
		// plus further unmapped fields; left opaque until reverse-engineered.
		std::uint8_t                    pad440[0xB8];   // 440
		uint16_t                        unk4F8;         // 4F8
		bool                            menusVisible;   // 4FA
	};
	static_assert(offsetof(UI, menuStack) == 0x3F0);
	static_assert(offsetof(UI, menuArray) == 0x430);
	static_assert(offsetof(UI, menusVisible) == 0x4FA);
}
