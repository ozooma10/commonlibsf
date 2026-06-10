#pragma once

#include "RE/B/BSTEvent.h"
#include "RE/B/BSTSingleton.h"

namespace RE
{
	struct MenuOpenCloseEvent;
	struct PositionPlayerEvent;

	// CAUTION (2026-06-09): NOT audited — ID::MenuTopicManager::Singleton is
	// 0, so the RTTI audit could not run. SDM is now the proven polymorphic
	// 0x10 shape; base order and the member offset below are UNPROVEN for
	// this class (the old header's speaker@0x14 comment was inconsistent even
	// with its own bases).
	class MenuTopicManager :
		public BSTSingletonSDM<MenuTopicManager>,  // 00
		public BSTEventSink<MenuOpenCloseEvent>,   // 10
		public BSTEventSink<PositionPlayerEvent>   // 18
	{
	public:
		[[nodiscard]] static MenuTopicManager* GetSingleton()
		{
			static REL::Relocation<MenuTopicManager**> singleton{ ID::MenuTopicManager::Singleton };
			return *singleton;
		}

		// members — UNPROVEN (see class comment)
		BSPointerHandle<TESObjectREFR> speaker;  // 20
	};
}
