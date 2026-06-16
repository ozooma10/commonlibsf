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

		// [AUDITED 2026-06-16, live peek] The singleton embeds THREE of these locked scatter tables
		// (at +0x28, +0x60, +0x98, exactly 0x38 apart). Layout confirmed against the find-helper (37921)
		// + a live walk: open-addressed, power-of-2 capacity, miss-sentinel == capacity. Bespoke (NOT stock
		// RE::BSTScatterTable) — it carries its own count fields + an embedded lock.
		struct LockedScatterTable  // 0x38
		{
			struct Entry  // 0x18
			{
				std::uint64_t key;        // 00 — lookup key (for the +0x98 map: the animationoffsets graph-variant hash)
				void*         value;      // 08 — mapped value (for the +0x98 map: a ClipDescriptor*)
				std::int32_t  chainNext;  // 10 — next-bucket INDEX, -1 = empty/end
				std::uint32_t pad14;      // 14
			};
			static_assert(sizeof(Entry) == 0x18);

			void*         pad00;     // 00
			void*         pad08;     // 08
			Entry*        entries;   // 10 — array[capacity]
			std::uint64_t capacity;  // 18 — power of 2; also the miss sentinel the find returns
			std::uint32_t count20;   // 20 — live count (clip map: 1701)
			std::uint32_t count28;   // 28 — live count (clip map: 2953)
			std::byte     lock[0x8]; // 30 — embedded lock (acquire 0x1422cc350 / release 0x1422cc3d0)
		};
		static_assert(sizeof(LockedScatterTable) == 0x38);

		// [AUDITED 2026-06-16] Object = *(REL::ID(938037)) = img+0x621AC00, a BSTSingletonSDMOpStaticBuffer
		// static object. Multi-inheritance: vtables at +0x00 / +0x10 / +0x18, and a 4th sub-object vtable at
		// +0x110 (the base sub-objects are UNIDENTIFIED, so real members are NOT modelled yet; sizeof TBD —
		// the object extends past +0x158). The three embedded LockedScatterTables:
		//   +0x28  descriptor -> object map        (cap 64;   FUN_140320ef0(this+0x28, ..))
		//   +0x60  sub-node registration map       (cap 2048; FUN_140320ef0(this+0x60, ..))
		//   +0x98  THE CLIP MAP: hash -> ClipDescriptor*  (cap 4096; entries@+0xa8, cap/sentinel@+0xb0,
		//          lock@+0xc8) — the runtime `animationoffsets` index; ResolveClipForKey reads it.
		// Corrects the earlier "+0x88 loader map" guess: there is NO map at +0x88; +0x88 is the +0x60
		// table's `count28`. TODO: identify the +0x10/+0x18/+0x110 base sub-objects and the ClipDescriptor
		// struct (descriptor +0x10 refcount, +0x40 sub-node count, +0x48 sub-node array, +0x51 init flag),
		// then model real members + a `static_assert(sizeof(AnimationClipDataSingleton) == 0x??)`.
	};
}
