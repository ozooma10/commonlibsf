#pragma once

#include "RE/N/NiPoint.h"

namespace RE
{
	class BGSModelNode;
	class IPostAnimationChannelUpdateFunctor;

	// Built on the stack by the animation update drivers and passed to
	// BSAnimationGraph::Update (vfunc 04). location/rotation are copied from
	// the owning TESObjectREFR's OBJ_REFR data, so an Update hook can match a
	// graph update to its actor. modelNode gates the post-update pose apply:
	// when null (e.g. the event-driven UpdateAnimationGraphManager worker), the
	// generated pose cannot be written to the rig buffers.
	class BSAnimationUpdateData
	{
	public:
		// members
		NiPoint3A                           location;           // 00 - from OBJ_REFR::location
		NiPoint3A                           rotation;           // 10 - from OBJ_REFR::angle
		NiPoint3A                           unk20;              // 20 - second location-style triple
		NiPoint3A                           unk30;              // 30 - second rotation-style triple
		float                               unk40;              // 40
		float                               unk44;              // 44
		IPostAnimationChannelUpdateFunctor* postUpdateFunctor;  // 48
		BGSModelNode*                       modelNode;          // 50 - rig apply target; null disables the pose apply
		float                               unk58;              // 58
		float                               unk5C;              // 5C
		float                               timeDelta;          // 60
		std::uint16_t                       unk64;              // 64
		std::uint16_t                       unk66;              // 66
		std::uint16_t                       unk68;              // 68 - 0xFFFF at construction
		bool                                forceUpdate;        // 6A
		bool                                modelCulled;        // 6B
		bool                                unk6C;              // 6C - true at construction
		bool                                unk6D;              // 6D
		bool                                unk6E;              // 6E
		bool                                unk6F;              // 6F
		float                               unk70;              // 70
	};
	static_assert(offsetof(BSAnimationUpdateData, modelNode) == 0x50);
	static_assert(offsetof(BSAnimationUpdateData, timeDelta) == 0x60);
	static_assert(offsetof(BSAnimationUpdateData, modelCulled) == 0x6B);
	static_assert(sizeof(BSAnimationUpdateData) == 0x80);
}
