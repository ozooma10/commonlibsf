#pragma once

#include "RE/B/BSTEvent.h"
#include "RE/T/TESObjectCELL.h"  // ForEachReferenceInRange fans out to per-cell walks

#include <functional>

namespace RE
{
	namespace BSResource::Archive2
	{
		class StreamOpenedEvent;
	}

	class Sky;
	class TESActorBase;

	struct PositionPlayerEvent;

	class __declspec(novtable) TES :
		public BSTEventSink<BSResource::Archive2::StreamOpenedEvent>,  // 000
		public BSTEventSink<PositionPlayerEvent>                       // 008
	{
	public:
		SF_RTTI_VTABLE(TES);

		virtual ~TES();  // 00

		// override (BSTEventSink<BSResource::Archive2::StreamOpenedEvent>)
		BSEventNotifyControl ProcessEvent(const BSResource::Archive2::StreamOpenedEvent& a_event, BSTEventSource<BSResource::Archive2::StreamOpenedEvent>* a_eventSource) override;  // 01 - { return BSEventNotifyControl::kContinue; }

		// override (BSTEventSink<PositionPlayerEvent>)
		BSEventNotifyControl ProcessEvent(const PositionPlayerEvent& a_event, BSTEventSource<PositionPlayerEvent>* a_eventSource) override;  // 01

		[[nodiscard]] static TES* GetSingleton()
		{
			static REL::Relocation<TES**> singleton{ ID::TES::Singleton };
			return *singleton;
		}

		std::uint16_t GetDeadCount(TESActorBase* a_actorBase)
		{
			using func_t = decltype(&TES::GetDeadCount);
			static REL::Relocation<func_t> func{ ID::TES::GetDeadCount };
			return func(this, a_actorBase);
		}

		// Visit every loaded reference within a_radius of a_origin, spanning the whole loaded area:
		// the current interior cell if there is one, otherwise every attached cell in the loaded
		// exterior grid. This is what player->parentCell->ForEachReferenceInRange cannot do (a single
		// cell misses neighbours). Callback returns kStop to end the walk within a cell.
		void ForEachReferenceInRange(const NiPoint3A& a_origin, float a_radius,
			std::function<BSContainer::ForEachResult(const NiPointer<TESObjectREFR>&)> a_callback) const
		{
			if (interiorCell) {
				interiorCell->ForEachReferenceInRange(a_origin, a_radius, a_callback);
				return;
			}
			// Exterior: mirror the engine's own ref-in-range grid walk (ID 46199 on 1.16.244).
			// gridCells is an array of gridCellCount slot pointers; a slot exposes its state at
			// slot+0x14 (skip unless loaded) and its TESObjectCELL* at slot+0x30. The null / state /
			// attached guards below are exactly the engine's, so a bad slot is skipped, not dereffed.
			if (gridCells) {
				for (std::uint32_t i = 0; i < gridCellCount; ++i) {
					const auto* slot = static_cast<const std::byte*>(gridCells[i]);
					if (!slot) {
						continue;
					}
					const std::uint32_t state = *reinterpret_cast<const std::uint32_t*>(slot + 0x14);
					if ((((state & 0x70000000u) + 0xD0000000u) & 0xEFFFFFFFu) != 0) {
						continue;  // slot not in a loaded state
					}
					auto* cell = *reinterpret_cast<TESObjectCELL* const*>(slot + 0x30);
					if (cell && cell->IsAttached()) {
						cell->ForEachReferenceInRange(a_origin, a_radius, a_callback);
					}
				}
			}
			// The engine's radius search walks one more bucket after the grid: the current
			// worldspace's PERSISTENT cell (TESWorldSpace::GetPersistentCell, create-if-missing,
			// exactly as the engine calls it). Persistent refs — notably player-built outpost
			// furniture — live here, NOT in the visual grid cells, so skipping this bucket makes
			// built furniture invisible to the walk.
			if (worldSpace) {
				using GetPersistentCellFn = TESObjectCELL* (*)(TESWorldSpace*, char);
				static REL::Relocation<GetPersistentCellFn> getPersistentCell{ ID::TESWorldSpace::GetPersistentCell };
				if (auto* persistent = getPersistentCell(worldSpace, 1)) {
					persistent->ForEachReferenceInRange(a_origin, a_radius, a_callback);
				}
			}
		}

		// members
		std::byte      pad010[0x18];   // 010
		Sky*           sky;            // 028
		void**         gridCells;      // 030 - loaded exterior grid: array of slot ptrs (slot+0x30 -> TESObjectCELL, slot+0x14 = state)
		std::byte      pad038[0x0C];   // 038
		std::uint32_t  gridCellCount;  // 044
		std::byte      pad048[0xA0];   // 048
		TESObjectCELL* interiorCell;   // 0E8
		std::byte      pad0F0[0x98];   // 0F0
		TESWorldSpace* worldSpace;     // 188 - current worldspace; its persistent cell holds player-built refs
	};
	static_assert(offsetof(TES, sky) == 0x28);
	static_assert(offsetof(TES, gridCells) == 0x30);
	static_assert(offsetof(TES, gridCellCount) == 0x44);
	static_assert(offsetof(TES, interiorCell) == 0xE8);
	static_assert(offsetof(TES, worldSpace) == 0x188);
}
