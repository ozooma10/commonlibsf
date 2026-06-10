#pragma once
#include "RE/B/BSFixedString.h"
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

		// RUNTIME-PROVEN (2026-06-09, MenuMapProbe + exe disasm of
		// UI::RegisterMenu 0x142546020 / UI::GetMenu 0x142544560 in 1.16.242):
		// the value part of a registration-map slot. RegisterMenu fills it as
		// { menu=null, initFunc=factory, unk10=0, flag (byte, usually 1), unk20=0 }.
		struct UIMenuEntry
		{
			using Create_t = Scaleform::Ptr<IMenu>*(Scaleform::Ptr<IMenu>*);

			Scaleform::Ptr<IMenu> menu;         // 00 — null until the menu is opened
			Create_t*             initFunc;     // 08 — per-menu factory passed to RegisterMenu
			void*                 unk10 = nullptr;  // 10
			uint64_t              flag = 1;     // 18 — byte; RegisterMenu's bool param (0 for e.g. ScopeMenu/TestMenu)
			uint64_t              unk20 = 0;    // 20 — refcounted object once live (released by GetMenu's copy path)
		};
		static_assert(sizeof(UIMenuEntry) == 0x28);

		// One slot of the name->menu registration map: the compiled shape of
		// BSTScatterTableEntry<BSFixedString, UIMenuEntry>.
		// nextIndex: -1 = empty slot; == bucketCount = end of chain; else the
		// next slot index in the collision chain. homeIndex = the bucket the
		// key hashes to (written on insert).
		struct UIMenuEntrySlot
		{
			BSFixedString name;       // 00 — interned key
			UIMenuEntry   entry;      // 08
			std::int32_t  nextIndex;  // 30
			std::int32_t  homeIndex;  // 34
		};
		static_assert(sizeof(UIMenuEntrySlot) == 0x38);

		// RUNTIME-PROVEN (2026-06-09): the registration map at UI+0x450. The
		// engine's hash is a 32-bit xor-fold of the interned char* ADDRESS
		// (string-pool identity, not characters), masked by (bucketCount - 1);
		// MenuMapProbe replayed the algorithm against the live table and found
		// "PauseMenu" at its home bucket. Header fields differ from CommonLibSF's
		// generic BSTScatterTable (entries at +0x38 here, not +0x18), so this is
		// a UI-local model of the compiled container, not the template.
		// NOTE: the engine guards access with a global reader/writer lock
		// (see UI::GetMenu 0x142544560); Find() reads locklessly, which is safe
		// after startup because registration happens during UI construction.
		struct UIMenuNameMap
		{
			[[nodiscard]] const UIMenuEntrySlot* Find(const BSFixedString& a_name) const
			{
				if (!entries || !bucketCount) {
					return nullptr;
				}
				const auto chars = reinterpret_cast<std::uintptr_t>(a_name.c_str());
				const auto hash = static_cast<std::uint32_t>(chars) ^ static_cast<std::uint32_t>(chars >> 32);
				auto idx = static_cast<std::uint64_t>(hash) & (bucketCount - 1);
				for (std::uint64_t hops = 0; hops < bucketCount; ++hops) {
					const auto& slot = entries[idx];
					if (slot.nextIndex == -1) {
						return nullptr;  // empty home slot
					}
					if (slot.name == a_name) {
						return &slot;
					}
					if (static_cast<std::uint64_t>(slot.nextIndex) == bucketCount) {
						return nullptr;  // end of chain
					}
					idx = static_cast<std::uint64_t>(slot.nextIndex);
				}
				return nullptr;
			}

			std::uint64_t    unk00[7];       // 00 — all zero in the live snapshot; allocator/lock state
			UIMenuEntrySlot* entries;        // 38 — dense slot array, bucketCount slots
			std::uint64_t    bucketCount;    // 40 — power of two (512 live); also the chain-end sentinel
			std::uint64_t    freeCount;      // 48 — bucketCount - registered menus (446 live -> 66 menus)
			std::uint64_t    freeScanIndex;  // 50 — free-slot scan cursor (name unproven)
		};
		static_assert(sizeof(UIMenuNameMap) == 0x58);

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

		// RESTORED (2026-06-09): the name->menu registration map was located at
		// UI+0x450 (see UIMenuNameMap above) and proven live by MenuMapProbe, so
		// the read-only name-keyed helpers are back with the proven layout.
		[[nodiscard]] const UIMenuEntrySlot* GetMenuEntry(const BSFixedString& a_menuName) const
		{
			return menuMap.Find(a_menuName);
		}

		[[nodiscard]] Scaleform::Ptr<IMenu> GetMenu(const BSFixedString& a_menuName) const
		{
			const auto* slot = menuMap.Find(a_menuName);
			return slot ? slot->entry.menu : nullptr;
		}

		[[nodiscard]] Scaleform::Ptr<Scaleform::GFx::Movie> GetMenuMovie(const BSFixedString& a_menuName) const
		{
			auto menu = GetMenu(a_menuName);
			return menu ? menu->uiMovie : nullptr;
		}

		bool IsMenuOpen(const BSFixedString& a_name) const
		{
			using func_t = decltype(&UI::IsMenuOpen);
			static REL::Relocation<func_t> func{ ID::UI::IsMenuOpen };
			return func(this, a_name);
		}

		[[nodiscard]] bool IsMenuRegistered(const BSFixedString& a_name) const
		{
			return menuMap.Find(a_name) != nullptr;
		}

		bool IsMenusVisible() const
		{
			return menusVisible;
		}

#if 0  // ID::UI::RegisterMenu is still 0 (unfilled). The engine function is
		// proven at RVA 0x2546020 in 1.16.242: UI::RegisterMenu(this,
		// const char* name, bool flag, factory) inserts into menuMap and fills
		// the slot (see UIMenuEntry). Restore via the ID once it is filled —
		// do NOT mutate the map inline; inserts take the engine's global lock
		// and its grow path.
		template <class T>
			requires(std::derived_from<T, IMenu>)
		bool RegisterMenu(const BSFixedString& a_name)
		{
			...
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
		// Second BSTArray (count=0, cap=4 with live heap data in both probe
		// snapshots); element type not yet identified.
		BSTArray<void*>                 unkArray440;    // 440
		// RUNTIME-PROVEN (MenuMapProbe, 2026-06-09): name->menu registration
		// map. Lookup base UI+0x450 (UI::GetMenu 0x142544560), insert base
		// UI+0x458 (UI::RegisterMenu 0x142546020 -> 0x142548940); entries at
		// UI+0x488, bucket count at UI+0x490, free count at UI+0x498.
		UIMenuNameMap                   menuMap;        // 450
		std::uint8_t                    pad4A8[0x50];   // 4A8
		uint16_t                        unk4F8;         // 4F8
		bool                            menusVisible;   // 4FA
	};
	static_assert(offsetof(UI, menuStack) == 0x3F0);
	static_assert(offsetof(UI, menuArray) == 0x430);
	static_assert(offsetof(UI, menuMap) == 0x450);
	static_assert(offsetof(UI, menuMap) + offsetof(UI::UIMenuNameMap, entries) == 0x488);
	static_assert(offsetof(UI, menusVisible) == 0x4FA);
}
