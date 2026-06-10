#pragma once
#include "RE/B/BSFixedString.h"
#include "RE/B/BSInputEventReceiver.h"
#include "RE/B/BSTSingleton.h"
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

	class UI :
		public BSTSingletonSDM<UI>,                              // 000
		public BSInputEventReceiver,                             // 010
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

			Scaleform::Ptr<IMenu> menu;             // 00 - null until the menu is opened
			Create_t*             initFunc;         // 08
			void*                 unk10 = nullptr;  // 10
			std::uint64_t         flag = 1;         // 18 - RegisterMenu's bool param
			std::uint64_t         unk20 = 0;        // 20
		};
		static_assert(sizeof(UIMenuEntry) == 0x28);

		// compiled shape of BSTScatterTableEntry<BSFixedString, UIMenuEntry>
		struct UIMenuEntrySlot
		{
			BSFixedString name;       // 00
			UIMenuEntry   entry;      // 08
			std::int32_t  nextIndex;  // 30 - -1 = empty, == bucketCount = chain end
			std::int32_t  homeIndex;  // 34
		};
		static_assert(sizeof(UIMenuEntrySlot) == 0x38);

		// name -> menu registration map; the hash is a 32-bit xor-fold of the
		// interned char* address. The engine guards writes with a global lock,
		// so mutate only through UI::RegisterMenu.
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
						return nullptr;
					}
					if (slot.name == a_name) {
						return &slot;
					}
					if (static_cast<std::uint64_t>(slot.nextIndex) == bucketCount) {
						return nullptr;
					}
					idx = static_cast<std::uint64_t>(slot.nextIndex);
				}
				return nullptr;
			}

			std::uint64_t    unk00[7];       // 00
			UIMenuEntrySlot* entries;        // 38
			std::uint64_t    bucketCount;    // 40 - power of two; also the chain-end sentinel
			std::uint64_t    freeCount;      // 48
			std::uint64_t    freeScanIndex;  // 50
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

		void RegisterMenu(const char* a_name, UIMenuEntry::Create_t* a_create, bool a_flag = true)
		{
			using func_t = void (*)(UI*, const char*, bool, UIMenuEntry::Create_t*);
			static REL::Relocation<func_t> func{ ID::UI::RegisterMenu };
			func(this, a_name, a_flag, a_create);
		}

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

		// members
		std::uint8_t                    pad188[0x268];   // 188
		BSTArray<Scaleform::Ptr<IMenu>> menuStack;       // 3F0
		std::uint8_t                    pad400[0x18];    // 400
		std::uint64_t                   unk418;          // 418
		std::uint64_t                   unk420;          // 420
		std::uint64_t                   unk428;          // 428
		BSTArray<Scaleform::Ptr<IMenu>> menuArray;       // 430 - active menus
		BSTArray<Scaleform::Ptr<IMenu>> menusToAdvance;  // 440 - rebuilt and emptied within each update tick
		UIMenuNameMap                   menuMap;         // 450
		std::uint8_t                    pad4A8[0x38];    // 4A8
		std::uint64_t                   unk4E0;          // 4E0 - timestamp of the previous update tick
		std::uint64_t                   unk4E8;          // 4E8
		float                           unk4F0;          // 4F0
		float                           advanceDelta;    // 4F4 - delta time passed to the per-menu movie advance
		std::uint16_t                   unk4F8;          // 4F8
		bool                            menusVisible;    // 4FA
		std::uint8_t                    pad4FB[5];       // 4FB
		std::uint64_t                   unk500;          // 500 - ms counter passed to the per-menu movie advance
		std::uint64_t                   unk508;          // 508
		std::uint8_t                    pad510[0x28];    // 510
		std::uint8_t                    unk538;          // 538 - guard byte around the menu advance loop
		std::uint8_t                    pad539[7];       // 539
	};
	static_assert(offsetof(UI, menuStack) == 0x3F0);
	static_assert(offsetof(UI, menuArray) == 0x430);
	static_assert(offsetof(UI, menusToAdvance) == 0x440);
	static_assert(offsetof(UI, menuMap) == 0x450);
	static_assert(offsetof(UI, advanceDelta) == 0x4F4);
	static_assert(offsetof(UI, menusVisible) == 0x4FA);
	static_assert(sizeof(UI) == 0x540);
}
