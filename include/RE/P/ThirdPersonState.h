#pragma once

#include <cstddef>

namespace RE
{
	// SF ThirdPersonState (a TESCameraState subclass) - the camera state behind
	// PlayerCamera->cameraStates[CameraState::kThirdPerson] (== PlayerCamera+0x228).
	//
	// PARTIAL, OFFSET-ANCHORED VIEW (1.16.244). CommonLibSF does not model the
	// camera-state subclasses, so this exposes only the runtime-proven zoom fields
	// for consumers that need to seed/read the third-person zoom. Reach it by
	// casting the void* slot:
	//     auto* st = static_cast<ThirdPersonState*>(camera->cameraStates[CameraState::kThirdPerson]);
	//
	// GUARD every access on (a) the vtable (RVA 0x4d05bc8 == AddrLib 459751) and
	// (b) stateId == 20 (kThirdPerson) before trusting/writing any offset - the
	// engine reuses the same slot array for every state.
	//
	// THE ZOOM GLIDE (runtime-proven 2026-06-26, see osf-re camera.state_machine):
	//   - targetZoomOffset (+0x224) is the SETPOINT. Write this.
	//   - currentZoomOffset (+0x228) is the eased/rendered value; the engine eases
	//     it toward targetZoomOffset every frame (rate fMouseWheelZoomSpeed=3.0/s),
	//     so a SINGLE write to targetZoomOffset GLIDES the camera - the consumer
	//     does NOT drive the lerp.
	//   - Range is NORMALIZED [fMinCurrentZoom 0.0 .. fMaxCurrentZoom 2.0]; 0.0 is
	//     the closest framing (on the player's back). A DIRECT write is NOT clamped
	//     by the engine (only the scroll handler clamps) - clamp your own writes.
	//   - ForceThirdPerson RESETS target/current/saved to the default (0.0) on every
	//     re-entry from first person, so write targetZoomOffset AFTER ForceThirdPerson.
	class ThirdPersonState
	{
	public:
		// members (offsets verified against 1.16.244)
		std::byte unk00[0x224];          // 000 - vtable @ 0x00 (RVA 0x4d05bc8), refcount @ 0x40, stateId @ 0x50
		float     targetZoomOffset;      // 224 - zoom SETPOINT (scroll-written; write THIS to request a glide)
		float     currentZoomOffset;     // 228 - eased/rendered zoom; chases targetZoomOffset every frame
		float     trackDistTarget;       // 22C - auto-track distance to camera target (NOT the user zoom)
		float     trackDistCurrent;      // 230 - eased auto-track distance (NOT the user zoom)
		std::byte unk234[0x23C - 0x234]; // 234
		float     savedZoomOffset;       // 23C - zoom stash restored across POV toggles
		std::byte unk240[0x28A - 0x240]; // 240
		bool      zoomingInto1st;        // 28A - 3rd->1st collapse one-shot latch (cleared on state exit)
		// ... struct continues to >= 0x2e0 (remaining fields unmapped)
	};

	static_assert(offsetof(ThirdPersonState, targetZoomOffset) == 0x224);
	static_assert(offsetof(ThirdPersonState, currentZoomOffset) == 0x228);
	static_assert(offsetof(ThirdPersonState, trackDistTarget) == 0x22C);
	static_assert(offsetof(ThirdPersonState, trackDistCurrent) == 0x230);
	static_assert(offsetof(ThirdPersonState, savedZoomOffset) == 0x23C);
	static_assert(offsetof(ThirdPersonState, zoomingInto1st) == 0x28A);
}
