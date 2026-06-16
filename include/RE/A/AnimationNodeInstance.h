#pragma once

// =====================================================================================================
// WIP / DRAFT — runtime reverse-engineered for Starfield 1.16.244 (OSF RE "clip-selection consumption
// gate" investigation, 2026-06-16). NOT yet PR-ready; layout-only (no function helpers) because the
// ctor / vtable-slot AddrLib IDs are not yet re-anchored (only raw VAs are known — see below).
//
// AnimationNodeInstance is the PER-ACTOR RUNTIME instance of an authored `.agx` NT_ANIMATION_NODE.
//   RTTI  : .?AVAnimationNodeInstance@@
//   vtable : 0x144d582e8 (.244)   (the authored/shared node is the SEPARATE class AnimationNode,
//            RTTI .?AVAnimationNode@@, vtable 0x144d58280 — do not conflate the two; they are adjacent
//            in RTTI and were conflated by an earlier static scout.)
//   ctor   : FUN_1422a6590 (VA 0x1422a6590 / RVA 0x22a6590) — sig (this, paramCtx, graphCtx, AnimationNode* tmpl);
//            sole caller = the factory at 0x1422a6428 (allocates the 0xA8 instance: `mov ecx, 0xA8`).
//   vtbl[4]: 0x1422a6ae0 (offset 0x20) — the per-frame update + dirty re-bind step.
//
// What is PROVEN (live `peek` + read-only ctor/slot-4 entry hooks ran clean, no crash):
//   * sizeof == 0xA8                         (the allocator size class at the factory).
//   * +0x18 = AnimationNode* template        (the SHARED authored node; written ONCE at ctor
//                                             `mov [rcx+0x18], r9`, never overwritten -> valid every call).
//   * +0x38 = clip data                      (the bound clip / clip-name struct; written by the ctor-binder
//                                             0x14223f7c0 into the channel sub-object at +0x20+0x18).
//   * +0x48 = u64 graph-variant filename hash (low dword == crc32(graph_path); e.g. 0x872B8B8E for
//                                             Player_Gun.agx — the same key family as AnimationClipDataSingleton).
//   * +0xa3 = "dirty"/needs-rebind byte       (vtbl[4]'s re-bind branch reads it, then clears it).
// NOT yet audited (-> gaps below): the base header / vtable region (0x00..0x18), the embedded per-instance
//   "channel"/playback sub-object (+0x20), and +0x40/+0x50.. fields. The AnimGraphNode base is unmodelled,
//   so this is a flat struct with named PROVEN offsets + pads (NOT the final class shape).
//
// RELATED (parallel OSF RE work, documented for cross-reference — model TBD):
//   * DynamicAnimationNodeInstance::vfunc7  AddrLib ID 123339 (0x142291bc0) — reads clip data @inst+0x38 +
//     template @inst+0x18 (SAME offsets as below) and composes the played `%s\%s.af` path (fmt 0x144d4f070),
//     then loads via 123610 (0x1422b3580) -> 123606 -> 123619. Likely the same family / a derived instance
//     class; reconcile before promoting either to a full class.
//
// IMPORTANT (runtime-proven, OSF RE 2026-06-16) — do NOT treat this as a clip-redirect seam:
//   The authored TagName at AnimationNode template+0xc8 is EMPTY at runtime and is NOT the selection key;
//   selection is a composed name `<stancePrefix>_<TagName>` (e.g. `Gun_FireSingle`, multi-prefix fallback
//   Relaxed_/Ready_/Gun_) resolved at GRAPH BUILD. Every runtime name-resolver tested (incl. the composed-name
//   registry lookup) is tamper-INERT for the RENDERED clip; only the offline `.agx` TagName edit (build-time)
//   changes the played clip. The load-bearing per-clip `.af` load is the path layer (123610/123606/123619).
//
// Full narrative + evidence:
//   OSF RE/Investigations/Requests/2026-06-15-clip-selection-consumption-gate.md
//   tools/ghidra/context_repo/modules/engine.animation_graphs.json
// =====================================================================================================

#include <cstddef>
#include <cstdint>

namespace RE
{
	class AnimationNode;  // the authored/shared node (vtable 0x144d58280); template at +0x18

	// RTTI .?AVAnimationNodeInstance@@, vtable 0x144d582e8 (.244), sizeof 0xA8.
	// Flat WIP layout: only the runtime-proven offsets are named; everything else is padding.
	class AnimationNodeInstance
	{
	public:
		std::byte      pad00[0x18];      // 00 — vtable (.?AVAnimationNodeInstance@@) + base header (UNAUDITED)
		AnimationNode* templateNode;     // 18 — SHARED authored `.agx` node (set once at ctor, never re-set)
		std::byte      pad20[0x18];      // 20 — embedded per-instance "channel"/playback sub-object (UNAUDITED)
		void*          clipData;         // 38 — bound clip / clip-name struct (composes `<dir>\<tag>.af`)
		std::byte      pad40[0x08];      // 40
		std::uint64_t  variantHash;      // 48 — graph-variant filename hash (low32 == crc32(graph_path))
		std::byte      pad50[0x53];      // 50
		std::uint8_t   dirty;            // a3 — needs-rebind flag (vtbl[4] reads, then clears)
		std::byte      padA4[0x04];      // a4
	};
	static_assert(offsetof(AnimationNodeInstance, templateNode) == 0x18);
	static_assert(offsetof(AnimationNodeInstance, clipData) == 0x38);
	static_assert(offsetof(AnimationNodeInstance, variantHash) == 0x48);
	static_assert(offsetof(AnimationNodeInstance, dirty) == 0xa3);
	static_assert(sizeof(AnimationNodeInstance) == 0xA8);
}
