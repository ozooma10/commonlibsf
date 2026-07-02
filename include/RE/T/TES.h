#pragma once

#include "RE/B/BSTEvent.h"
#include "RE/T/TESObjectCELL.h"

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

		void ForEachReferenceInRange(const NiPoint3A& a_origin, float a_radius,
			std::function<BSContainer::ForEachResult(const NiPointer<TESObjectREFR>&)> a_callback) const
		{
			if (interiorCell) {
				interiorCell->ForEachReferenceInRange(a_origin, a_radius, a_callback);
				return;
			}
			if (gridCells) {
				for (std::uint32_t i = 0; i < gridCellCount; ++i) {
					const auto* slot = static_cast<const std::byte*>(gridCells[i]);
					if (!slot) {
						continue;
					}
					const std::uint32_t state = *reinterpret_cast<const std::uint32_t*>(slot + 0x14);
					if ((((state & 0x70000000u) + 0xD0000000u) & 0xEFFFFFFFu) != 0) {
						continue;
					}
					auto* cell = *reinterpret_cast<TESObjectCELL* const*>(slot + 0x30);
					if (cell && cell->IsAttached()) {
						cell->ForEachReferenceInRange(a_origin, a_radius, a_callback);
					}
				}
			}
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
		void**         gridCells;      // 030
		std::byte      pad038[0x0C];   // 038
		std::uint32_t  gridCellCount;  // 044
		std::byte      pad048[0xA0];   // 048
		TESObjectCELL* interiorCell;   // 0E8
		std::byte      pad0F0[0x98];   // 0F0
		TESWorldSpace* worldSpace;     // 188
	};
	static_assert(offsetof(TES, sky) == 0x28);
	static_assert(offsetof(TES, gridCells) == 0x30);
	static_assert(offsetof(TES, gridCellCount) == 0x44);
	static_assert(offsetof(TES, interiorCell) == 0xE8);
	static_assert(offsetof(TES, worldSpace) == 0x188);
}
