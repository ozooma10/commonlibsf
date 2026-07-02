#pragma once

#include "RE/B/BSPointerHandle.h"
#include "RE/B/BSTArray.h"
#include "RE/B/BSTEvent.h"
#include "RE/B/BSTSingleton.h"

namespace RE
{
	struct PositionPlayerEvent;

	class __declspec(novtable) ProcessLists :
		public BSTEventSink<PositionPlayerEvent>,  // 000
		public BSTSingletonSDM<ProcessLists>       // 008
	{
	public:
		SF_RTTI_VTABLE(ProcessLists);

		virtual ~ProcessLists();  // 00

		// override
		virtual BSEventNotifyControl ProcessEvent(const PositionPlayerEvent&, BSTEventSource<PositionPlayerEvent>*) override;  // 01

		[[nodiscard]] static ProcessLists* GetSingleton()
		{
			static REL::Relocation<ProcessLists**> singleton{ ID::ProcessLists::Singleton };
			return *singleton;
		}

		[[nodiscard]] bool AreHostileActorsNear(BSScrapArray<BSPointerHandle<Actor>>* a_hostileActors = nullptr)
		{
			using func_t = decltype(&ProcessLists::AreHostileActorsNear);
			static REL::Relocation<func_t> func{ ID::ProcessLists::AreHostileActorsNear };
			return func(this, a_hostileActors);
		}

		void EnableAI(bool a_enable)
		{
			using func_t = decltype(&ProcessLists::EnableAI);
			static REL::Relocation<func_t> func{ ID::ProcessLists::ToggleAI };
			func(this, a_enable);
		}

		// Manager-level combat removal: drops a_actor from combat for ALL combatants. This is the lever
		// per-actor Actor::StopCombat (vfunc 0x16E) lacks - others re-acquire the target after StopCombat
		// but not after this. Core of the console StopCombatAlarmOnActor handler. Runtime-proven (osf-re
		// gameplay.defeat_damage_hook: a defeated NPC's attackers disengage only once this is applied).
		void StopCombatAndAlarmOnActor(Actor* a_actor, bool a_arg2 = false)  // a_arg2: console handler passes false; semantics unconfirmed
		{
			using func_t = decltype(&ProcessLists::StopCombatAndAlarmOnActor);
			static REL::Relocation<func_t> func{ ID::ProcessLists::StopCombatAndAlarmOnActor };
			func(this, a_actor, a_arg2);
		}

		// members
		std::byte                        pad018[0x8];                   // 018
		std::int32_t                     numberHighActors;              // 020
		std::int32_t                     numberFullyEnabledHighActors;  // 024
		std::byte                        pad028[0x1C];                  // 028
		bool                             runDetection;                  // 044
		std::byte                        unk045;                        // 045
		bool                             processHigh;                   // 046
		bool                             processLow;                    // 047
		bool                             processMiddleHigh;             // 048
		bool                             processMiddleLow;              // 049
		bool                             processSchedules;              // 04A
		bool                             showSubtitles;                 // 04B
		std::byte                        pad04C[0xE];                   // 04C
		// The four per-process actor lists (osf-re gameplay.process_lists, .244).
		// CORRECTED: each is at +8 vs the Fallout-4-era layout (highActorHandles was
		// 0x58). Layout {size(u32)@+0, cap(u32)@+4, data@+8}. Proven static (two
		// engine fns read count@+0x60 / data@+0x68, 4-byte handles) + live (5/5
		// resolved to Actor vtbl img+0x4CB9248). highActorHandles is the near-player,
		// 3D-loaded, cell-spanning set; numberHighActors above == highActorHandles
		// .size() at runtime. WARNING: lowActorHandles is large and holds
		// PARTIALLY-LOADED actors — resolving them is safe (GetSmartPointer
		// self-guards) but calling Actor vfuncs (GetDisplayFullName, etc.) on them
		// faults uncatchably; only vfunc the high list. Order/offsets of low/middle
		// per FO4 + live population (high proven; the rest strongly inferred).
		BSTArray<BSPointerHandle<Actor>> highActorHandles;              // 060
		BSTArray<BSPointerHandle<Actor>> lowActorHandles;               // 070
		BSTArray<BSPointerHandle<Actor>> middleHighActorHandles;        // 080
		BSTArray<BSPointerHandle<Actor>> middleLowActorHandles;         // 090
		std::byte                        pad0A0[0x14C];                 // 0A0
		bool                             runSchedules;                  // 1EC (unverified; carried from FO4-era layout)
		bool                             runMovement;                   // 1ED (unverified)
		bool                             runAnimations;                 // 1EE (unverified)
	};
	static_assert(offsetof(ProcessLists, numberHighActors) == 0x20);
	static_assert(offsetof(ProcessLists, runDetection) == 0x44);
	static_assert(offsetof(ProcessLists, showSubtitles) == 0x4B);
	static_assert(offsetof(ProcessLists, highActorHandles) == 0x60);
	static_assert(offsetof(ProcessLists, runSchedules) == 0x1EC);
}
