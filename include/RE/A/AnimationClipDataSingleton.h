#pragma once

// =====================================================================================================
// WIP / DRAFT — runtime reverse-engineered for Starfield 1.16.244 (OSF RE "clip-binding consumption gate"
// investigation, 2026-06-16). NOT yet PR-ready.
//
// What is PROVEN (runtime hook ran clean + Ghidra decompile + AddrLib re-anchor):
//   * the singleton instance pointer (ID 938037),
//   * AnimationClipDataSingleton::ResolveClipForKey (ID 123641) and its exact 5-arg signature,
//   * the +0x98 clip hashmap quad (map@0x98 / entries@0xa8 / end@0xb0 / lock@0xc8).
// What is NOT yet audited (-> the TODOs / `[candidate]` members below): the full struct layout
//   (everything 0x00..0x98, the secondary maps at 0x28/0x60/0x70, the loader map at 0x88, the
//   ClipDescriptor struct, the exact BSTScatterTable template, and sizeof()).
//
// Architecture (full narrative + evidence):
//   OSF RE/Investigations/Requests/2026-06-15-clip-selection-consumption-gate.md  and the
//   tools/ghidra/context_repo/modules/engine.animation_graphs.json note
//   "Clip-binding consumption gate CLOSED ...".
//
//   * Clip SELECTION (which tag) is read LIVE from the .agx graph node TagName (loose-overridable).
//   * Clip PATH resolves at GRAPH BIND (weapon equip), then is CACHED, via ResolveClipForKey below.
//   * The +0x98 map is a `uint64-hash -> ClipDescriptor*` hashmap keyed by the animationoffsets /
//     animationfiledata FILENAME HASH: low dword == crc32(graph_path) (e.g. 0x872B8B8E for
//     Player_Gun.agx), high dword == an opaque per-variant discriminator. It is BUILT FROM THE BASE
//     `Starfield - Animations.ba2` AT LOAD, which is exactly why loose / mod-archive `animationoffsets`
//     *text* edits are inert at runtime (the engine reads this in-memory hash index, not the files).
//
// Related IDs (same +0x98 map / family; documented here, headers TBD):
//   * SubgraphAndOffsetDBData::slot0  ID 123661 (0x1422bbf80) — queries the SAME +0x98 map by hash.
//   * find-helper                     ID 37921  (0x1403499f0) — CRC over the 64-bit key via byte-table
//                                     DAT_144d58d70 (a uint64 scatter-find, NOT a BSFixedString find).
//   * directory loader                ID 123640 (0x1422b5e20) — populates the loader map (+0x88).
//   * map inserter                    ID 123666 (0x1422bc3b0).
//   * lock acquire / release          0x1422cc350 / 0x1422cc3d0  (on +0xc8 / +0x60-area locks).
//   * the singleton is a BSTSingletonSDM<...> static-buffer singleton (per CLSF RTTI IDs
//     BSTSingletonSDM_AnimationClipDataSingleton_BSTSingletonSDMOpStaticBuffer_*).
// =====================================================================================================

#include "RE/B/BSTSingleton.h"

#include <cstdint>

namespace RE
{
	class AnimationClipDataSingleton :
		public BSTSingletonSDM<AnimationClipDataSingleton>  // 00
	{
	public:
		// [PROVEN — runtime] Instance pointer global = AddrLib ID 938037 (VA 0x1461df320 on 1.16.244),
		// an `AnimationClipDataSingleton**`. The pointee's vtable RTTI is `.?AVAnimationClipDataSingleton@@`.
		[[nodiscard]] static AnimationClipDataSingleton* GetSingleton()
		{
			static REL::Relocation<AnimationClipDataSingleton**> singleton{ REL::ID(938037) };
			return *singleton;
		}

		// Output of ResolveClipForKey: the 16-byte {object, descriptor} pair the engine writes to the
		// caller-supplied OUT pointer (the 5th arg). `descriptor` is the +0x98 map value (a ClipDescriptor,
		// refcounted at +0x10); `obj` is the selector-resolved object.
		struct ResolveResult
		{
			void* obj{ nullptr };         // 00 — selector-dependent resolved object (NiObject-ish)
			void* descriptor{ nullptr };  // 08 — ClipDescriptor* (the +0x98 map value)
		};
		static_assert(sizeof(ResolveResult) == 0x10);

		// [PROVEN — Ghidra FUN_1422b61e0 + runtime hook ran clean with this exact 5-arg signature]
		// AddrLib ID 123641 (VA 0x1422b61e0). Resolves clip data for a key at GRAPH BIND, then caches.
		//   a_keyPtr  : `uint64*` — *a_keyPtr is the animationoffsets graph-variant filename hash.
		//   a_selector: opaque — picks a variant via the +0x28 map (FUN_140320770).
		//   a_prev    : opaque — previous/contextual result (refcount-swapped on the +0x10 field).
		//   a_out     : OUT — receives {obj, descriptor}.
		// Returns true iff a descriptor was found for the key.
		// NB the OUT pointer (a_out) is the 5th, STACK-passed arg; omitting it crashes the callee (it
		// writes a 16-byte result via `vmovups [rax], xmm0` to *a_out).
		bool ResolveClipForKey(std::uint64_t* a_keyPtr, void* a_selector, void* a_prev, ResolveResult* a_out)
		{
			using func_t = decltype(&AnimationClipDataSingleton::ResolveClipForKey);
			static REL::Relocation<func_t> func{ REL::ID(123641) };
			return func(this, a_keyPtr, a_selector, a_prev, a_out);
		}

		// ------------------------------------------------------------------------------------------------
		// PARTIAL LAYOUT — offsets PROVEN by the 123641 / 123661 disasm where marked; the rest is
		// `[candidate]` pending the live object-layout audit. Members intentionally NOT declared yet (a
		// wrong offset poisons consumers); documented as comments until the audit fills the gaps.
		//
		//   0x00  BSTSingletonSDM base
		//   0x28  BSTScatterTable<...>*   secondary map: descriptor -> object   FUN_140320ef0(this+0x28,..)  [candidate]
		//   0x60  BSTScatterTable<...>    sub-node registration map             FUN_140320ef0(this+0x60,..)  [candidate]
		//   0x70  ...                     paired with +0x60                                                  [candidate]
		//   0x88  BSTScatterTable<...>    LOADER insert map (123640 / 123666)                                [candidate]
		//   0x98  <clip map header>       uint64-hash -> ClipDescriptor*:                                    [PROVEN]
		//   0xa8  void* entriesBase       0x18-byte entries { key@+0x00, value@+0x08, chainNext@+0x10 }      [PROVEN]
		//   0xb0  std::uint64_t endIdx    "not found" sentinel (find returns this on miss)                   [PROVEN]
		//   0xc8  BSReadWriteLock         acquired (0x1422cc350) before the +0x98 lookup                     [PROVEN]
		//
		// TODO(audit): GetSingleton() -> peek object 0x00..0xE0, confirm the BSTScatterTable<std::uint64_t,
		// ClipDescriptor*> shape against RE/B/BSTScatterTable.h, fill the [candidate] members + the
		// ClipDescriptor struct (descriptor+0x40 count, +0x48 sub-node array, +0x51 init flag, +0x10
		// refcount), then add real members and `static_assert(sizeof(AnimationClipDataSingleton) == 0x??)`.
		// ------------------------------------------------------------------------------------------------
	};
}
