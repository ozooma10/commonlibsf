#pragma once

#include "RE/B/BSIntrusiveRefCounted.h"
#include "RE/I/IMovementAnimationDrivenFeedback.h"
#include "RE/I/IMovementDirectControl.h"
#include "RE/I/IMovementPlannerDirectControl.h"
#include "RE/I/IMovementProcessMessageInterface.h"
#include "RE/I/IMovementQueryAnimDeltas.h"
#include "RE/I/IMovementSelectIdle.h"
#include "RE/M/MovementControllerAI.h"

namespace RE
{
	// The movement controller owned by Actor (Actor::movementController, +0x240).
	class MovementControllerNPC :
		public MovementControllerAI,              // 00
		public IMovementProcessMessageInterface,  // 78
		public IMovementSelectIdle,               // 80
		public IMovementDirectControl,            // 88
		public IMovementPlannerDirectControl,     // 90
		public IMovementQueryAnimDeltas,          // 98
		public IMovementAnimationDrivenFeedback,  // A0
		public BSIntrusiveRefCounted              // A8
	{
	public:
		SF_RTTI_VTABLE(MovementControllerNPC);

		// Switches the controller to animation-driven movement: graph root
		// motion becomes the only position writer. The engine's own paired-scene
		// anchor (BGSSynchronizedAnimationManager uses the same call). The mode
		// is latched under the controller lock and applied deferred by the next
		// movement update.
		//
		// Polarity note: this pair has been mislabeled in other tooling. It is
		// confirmed via the console's ToggleMotionDriven (same worker pair,
		// handler prints "now Animation-Driven" when this one fires).
		void SetAnimationDriven()
		{
			using func_t = decltype(&MovementControllerNPC::SetAnimationDriven);
			static REL::Relocation<func_t> func{ ID::MovementControllerNPC::SetAnimationDriven };
			return func(this);
		}

		// Switches the controller back to motion-driven movement (the default:
		// the movement/pathing system writes positions).
		void SetMotionDriven()
		{
			using func_t = decltype(&MovementControllerNPC::SetMotionDriven);
			static REL::Relocation<func_t> func{ ID::MovementControllerNPC::SetMotionDriven };
			return func(this);
		}

		// total object size is larger; members past the refcount are unmapped
	};
}
