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

		// members — the previous member block (numberHighActors@0x20,
		// process flags@0x44, four handle arrays from 0x58, run flags@0x1EC)
		// was carried over from Fallout 4 and is DISPROVEN for Starfield: a
		// live peek (2026-06-10, in gameplay) found churning floats at +0x20
		// and a BSTArray<handle-sized> at +0x30 (size=1108/cap=2048, heap
		// data at +0x38), with self-referential list pointers around
		// +0x60..0x78. The real Starfield member map needs its own
		// investigation; until then the block is opaque.
		std::byte pad018[0x1D8];  // 018 — UNMAPPED for Starfield
	};
	static_assert(sizeof(ProcessLists) == 0x1F0);
}
