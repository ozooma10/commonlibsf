#pragma once

namespace RE
{
	class MovementControllerAI
	{
	public:
		SF_RTTI_VTABLE(MovementControllerAI);

		virtual ~MovementControllerAI();  // 00

		// members
		std::byte    unk08[0x68];   // 08
		// 70 - documented 1 = animation-driven, 2 = motion-driven. ⚠ SUSPECT (OSF runtime 2026-06-27): read 0 for
		// BOTH a motion-driven walking NPC AND a movement-locked player (expected 2 and 1) — this field/offset looks
		// MIS-MAPPED for 1.16.244; do not trust until re-RE'd. The working animation-driven lever was
		// Actor::boolFlags2 kAnimationDriven (bit 19), not this field. Module: gameplay.actor_animation_driven.
		std::uint8_t movementMode;  // 70
		std::byte    unk71[0x07];   // 71
	};
	static_assert(offsetof(MovementControllerAI, movementMode) == 0x70);
	static_assert(sizeof(MovementControllerAI) == 0x78);
}
