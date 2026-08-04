#pragma once

#include "RE/B/BGSModelNode.h"
#include "RE/N/NiNode.h"
#include "RE/N/NiSmartPointer.h"

namespace RE
{
	class BSFadeNode :
		public NiNode  // 000
	{
	public:
		SF_RTTI_VTABLE(BSFadeNode);

		// members (NiNode is 0x150 — proven 2026-07-30, engine.scene_graph_attach)
		std::byte               unk150[0x30];  // 150
		NiPointer<BGSModelNode> bgsModelNode;  // 180 (runtime-proven: refr->loadedData->data3D->+0x180)
		std::byte               unk188[0x38];  // 188
	};
	static_assert(offsetof(BSFadeNode, bgsModelNode) == 0x180);
	static_assert(sizeof(BSFadeNode) == 0x1C0);
}
