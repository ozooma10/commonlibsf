#pragma once

#include "RE/B/BSModelNode.h"
#include "RE/B/BSTArray.h"
#include "RE/N/NiTransform.h"

namespace RE
{
	class NiAVObject;

	class BGSModelNode :
		public BSModelNode  // 00
	{
	public:
		SF_RTTI_VTABLE(BGSModelNode);

		struct Rig
		{
			struct Buffer
			{
				// members
				void*        unk00;  // 00
				void*        unk08;  // 08
				NiTransform* data;   // 10 - one slot per rig bone
			};

			// members
			std::uint32_t  refCount;     // 00
			std::uint32_t  unk04;        // 04
			Buffer*        local;        // 08
			Buffer*        world;        // 10
			Buffer*        prevWorld;    // 18
			void*          unk20;        // 20
			std::uint16_t* parents;      // 28
			void*          unk30;        // 30
			std::uint16_t* order;        // 38
			std::byte      unk40[0x48];  // 40
			bool           rebuild;      // 88
		};
		static_assert(offsetof(Rig, rebuild) == 0x88);

		struct NodeEntry
		{
			// members
			std::uint16_t rigIndex;  // 00
			std::uint16_t unk02;     // 02
			std::uint16_t unk04;     // 04
			std::uint16_t unk06;     // 06
			NiAVObject*   node;      // 08
		};
		static_assert(sizeof(NodeEntry) == 0x10);

		~BGSModelNode() override;  // 00

		// members
		Rig*                rig;          // 10
		BSTArray<NodeEntry> nodes;        // 18  - node->bone map; rig sync stamps EVERY entry (no skip)
		std::byte           unk28[0x40];  // 28
		std::uint32_t       frameStamp;   // 68  - last composed frame (rig sync 0x142be93f0)
		std::uint32_t       unk6C;        // 6C
		void*               sgSyncCallback;  // 70  - scene-graph-sync callback object (compose +0x70)
		std::uint16_t       boneCount;    // 78  - element count of rig->local/world/prevWorld buffers
		std::uint16_t       unk7A;        // 7A
		std::uint16_t       rootGroupA;   // 7C  - root compose count = rootGroupA + rootGroupB
		std::uint16_t       rootGroupB;   // 7E
		std::uint8_t        active;       // 80  - rig-path gate byte
		std::byte           unk81[0xF];   // 81
	};
	static_assert(offsetof(BGSModelNode, frameStamp) == 0x68);
	static_assert(offsetof(BGSModelNode, sgSyncCallback) == 0x70);
	static_assert(offsetof(BGSModelNode, rootGroupA) == 0x7c);
	static_assert(offsetof(BGSModelNode, nodes) == 0x18);
	static_assert(offsetof(BGSModelNode, boneCount) == 0x78);
	static_assert(offsetof(BGSModelNode, active) == 0x80);
	static_assert(sizeof(BGSModelNode) == 0x90);
}
