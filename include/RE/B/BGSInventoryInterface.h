#pragma once

#include "RE/B/BSTArray.h"
#include "RE/B/BSTEvent.h"
#include "RE/B/BSTSingleton.h"

namespace RE
{
	class BGSInventoryItem;
	class TESBoundObject;

	namespace InventoryInterface
	{
		struct FavoriteChangedEvent
		{
		public:
			~FavoriteChangedEvent() noexcept {}  // intentional

			// members
			BGSInventoryItem* itemAffected;  // 0
		};
		static_assert(sizeof(FavoriteChangedEvent) == 0x8);

		struct Handle
		{
		public:
			~Handle() noexcept {}  // intentional

			// members
			std::uint32_t id;  // 0
		};
		static_assert(sizeof(Handle) == 0x4);
	}

	// EXE RTTI (2026-06-09, SingletonSdmAuditProbe): BSTSingletonSDM at mdisp
	// 0x0 (0x10, polymorphic), the FavoriteChangedEvent source at mdisp 0x10
	// (old empty-SDM math put it at 0x08).
	class alignas(0x08) BGSInventoryInterface :
		BSTSingletonSDM<BGSInventoryInterface>,                   // 00
		BSTEventSource<InventoryInterface::FavoriteChangedEvent>  // 10 — exe RTTI mdisp
	{
	public:
		struct Agent
		{
		public:
			// members
			std::uint32_t handleID;   // 0
			std::uint32_t itemOwner;  // 4 - TESPointerHandle
			std::uint16_t listIndex;  // 8
			std::uint16_t refCount;   // A
		};
		static_assert(sizeof(Agent) == 0xC);

		[[nodiscard]] static BGSInventoryInterface* GetSingleton()
		{
			static REL::Relocation<BGSInventoryInterface**> singleton{ ID::BGSInventoryInterface::Singleton };
			return *singleton;
		}

		[[nodiscard]] TESBoundObject* GetInventoryObject(const std::uint32_t& a_handleID) const
		{
			struct UnkStruct
			{
				TESBoundObject** ppObj;
				std::uint64_t    unk08;
			};

			std::uint32_t   handle = a_handleID;
			TESBoundObject* pObj = 0;
			UnkStruct       outstruct = { .ppObj = &pObj, .unk08 = 0 };
			UnkStruct*      pOutstruct = &outstruct;

			using func_t = bool (*)(const BGSInventoryInterface*, const std::uint32_t*, UnkStruct**);
			static REL::Relocation<func_t> GetInventoryObjectHelperFn{ ID::BGSInventoryInterface::GetInventoryObjectSub };
			GetInventoryObjectHelperFn(this, &handle, &pOutstruct);

			return pObj;
		}

		virtual ~BGSInventoryInterface();  // 00

		// members — RUNTIME-PROVEN (live peek 2026-06-10): the event source
		// ends at 0x38 and a BSTArray header sits there directly (live
		// size=4/cap=0x400 with a heap data ptr at +0x40); the old header's
		// unk30 qword never existed.
		BSTArray<Agent> agentArray;  // 38
	};
	static_assert(offsetof(BGSInventoryInterface, agentArray) == 0x38);
	//static_assert(sizeof(BGSInventoryInterface) == 0x48);
}
