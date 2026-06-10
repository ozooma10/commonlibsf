#pragma once

#include "RE/B/BSPointerHandle.h"
#include "RE/B/BSTArray.h"
#include "RE/B/BSTEvent.h"
#include "RE/B/BSTSingleton.h"

namespace RE
{
	struct PositionPlayerEvent;

	// EXE RTTI (2026-06-09, exe_query `vtable ProcessLists --bases`): the
	// BaseClassArray proves BSTEventSink<PositionPlayerEvent> at mdisp 0x0
	// (2-slot vtable 0x144CC4228) and BSTSingletonSDM at mdisp 0x8 (1-slot
	// vtable 0x144CC4240) — the original sink-first order, with bases ending
	// at 0x18; the 0x20/0x44/0x58/0x1EC member anchors are preserved by
	// pad018. The ctor (0x141a8b790) stores the singleton at RVA 0x5FE1AC8
	// (ID 937584).
	class __declspec(novtable) ProcessLists :
		public BSTEventSink<PositionPlayerEvent>,  // 000 — exe RTTI mdisp
		public BSTSingletonSDM<ProcessLists>       // 008 — exe RTTI mdisp
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
		std::byte                        pad04C[0x6];                   // 04C
		BSTArray<BSPointerHandle<Actor>> highActorHandles;              // 058
		BSTArray<BSPointerHandle<Actor>> lowActorHandles;               // 068
		BSTArray<BSPointerHandle<Actor>> middleHighActorHandles;        // 078
		BSTArray<BSPointerHandle<Actor>> middleLowActorHandles;         // 088
		std::byte                        pad098[0x154];                 // 098
		bool                             runSchedules;                  // 1EC
		bool                             runMovement;                   // 1ED
		bool                             runAnimations;                 // 1EE
	};
	static_assert(offsetof(ProcessLists, numberHighActors) == 0x20);
	static_assert(offsetof(ProcessLists, runDetection) == 0x44);
	static_assert(offsetof(ProcessLists, showSubtitles) == 0x4B);
	static_assert(offsetof(ProcessLists, highActorHandles) == 0x58);
	static_assert(offsetof(ProcessLists, runSchedules) == 0x1EC);
}
