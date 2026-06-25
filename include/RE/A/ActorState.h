#pragma once

#include "RE/I/IMovementState.h"

namespace RE
{
	// Packed in ActorState::actorState1 bits 17-20 (mask 0x1E0000) at actor+0xF8;
	// lifeState = (actorState1 >> 17) & 0xF. Values 0/1/2/3/6/7 proven from the
	// .244 SetLifeState/SetRestrained/SetUnconscious code, and kBleedOut=8 runtime-
	// confirmed (SetLifeState(8) downs a live NPC into bleedout, 2026-06-25);
	// 4/5 inferred from the standard Bethesda ordering, not yet byte-confirmed.
	enum class ACTOR_LIFE_STATE : std::uint32_t
	{
		kAlive = 0,
		kDying = 1,
		kDead = 2,
		kUnconscious = 3,
		kReanimate = 4,
		kRecycle = 5,
		kRestrained = 6,
		kEssentialDown = 7,
		kBleedOut = 8
	};

	class ActorState : public IMovementState
	{
	public:
		SF_RTTI(ActorState);

		~ActorState() override;  // 00

		// add
		virtual void Unk_31();  // 31
		virtual void Unk_32();  // 32
		virtual void Unk_33();  // 33
		virtual void Unk_34();  // 34

		// members
		std::uint32_t actorState1;  // 08
		std::uint32_t actorState2;  // 0C
	};
	static_assert(sizeof(ActorState) == 0x10);
}
