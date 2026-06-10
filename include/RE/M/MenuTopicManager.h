#pragma once

#include "RE/B/BSTEvent.h"
#include "RE/B/BSTSingleton.h"

namespace RE
{
	struct MenuOpenCloseEvent;
	struct PositionPlayerEvent;

	// EXE RTTI (2026-06-09, exe_query `vtable MenuTopicManager --bases`): the
	// BaseClassArray proves BSTSingletonSDM at mdisp 0x0,
	// BSTEventSink<MenuOpenCloseEvent> at 0x10, and
	// BSTEventSink<PositionPlayerEvent> at 0x18 (vtables 0x144CAC290 /
	// 0x144CAC2A0 / 0x144CAC278). The ctor (0x141792b90) stores the singleton
	// at RVA 0x61EA280 (ID 938421).
	class MenuTopicManager :
		public BSTSingletonSDM<MenuTopicManager>,  // 00 — exe RTTI mdisp
		public BSTEventSink<MenuOpenCloseEvent>,   // 10 — exe RTTI mdisp
		public BSTEventSink<PositionPlayerEvent>   // 18 — exe RTTI mdisp
	{
	public:
		[[nodiscard]] static MenuTopicManager* GetSingleton()
		{
			static REL::Relocation<MenuTopicManager**> singleton{ ID::MenuTopicManager::Singleton };
			return *singleton;
		}

		// members — base map is proven; speaker's own offset is carried over,
		// not independently re-proven
		BSPointerHandle<TESObjectREFR> speaker;  // 20
	};
}
