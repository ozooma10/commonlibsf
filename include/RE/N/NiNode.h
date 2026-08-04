#pragma once

#include "RE/N/NiAVObject.h"
#include "RE/N/NiSmartPointer.h"

namespace RE
{
	class __declspec(novtable) NiNode :
		public NiAVObject
	{
	public:
		SF_RTTI_VTABLE(NiNode);

		// NiNode vtable = REL::ID(497979), 95 slots on 1.16.244. The methods
		// below are the NiNode-added virtuals 84..94, runtime-proven 2026-07-30
		// (OSF RE: Util/SceneGraph.cpp + SceneGraphProbe harness; module note
		// engine.scene_graph_attach). Semantics:
		//   - AttachChild INCREFS the child; if the child already has a parent
		//     it is first detached from it (NiAVObject::SetParent, ID 147223).
		//     a_firstAvail=true reuses a null hole in the child array.
		//   - DetachChild validates child->parent == this and transfers the
		//     array's reference into a_out; the NoOut variant RELEASES it (may
		//     destroy the child). Both clear child->parent and set the child's
		//     childIndex (NiAVObject+0x0C, u16) to 0xFFFF.
		//   - No internal locking (refcounts aside): callers must be on the
		//     game's main thread outside the scene-graph update.
		// The pre-2026-07-30 declarations (AddChild/RemoveChild/...) were wrong
		// and caused a crash: AddChild -> slot 84 with garbage a_firstAvail,
		// RemoveChild -> slot 85 SetAt(child-pointer-as-index, garbage).

		~NiNode() override = default;  // 00

		// add
		virtual void AttachChild(NiAVObject* a_child, bool a_firstAvail);                                   // 84 - vt+0x2A0, ID 147173
		virtual void SetAt(std::uint32_t a_index, NiAVObject* a_child);                                     // 85 - vt+0x2A8, ID 147174
		virtual void DetachChild(NiAVObject* a_child, NiPointer<NiAVObject>& a_out);                        // 86 - vt+0x2B0, ID 147178
		virtual void DetachChildNoOut(NiAVObject* a_child);                                                 // 87 - vt+0x2B8, ID 147177
		virtual void DetachChildAlt(NiAVObject* a_child);                                                   // 88 - vt+0x2C0, ID 147179
		virtual void DetachChildAt(std::uint32_t a_index, NiPointer<NiAVObject>& a_out);                    // 89 - vt+0x2C8, ID 147176
		virtual void DetachChildAtNoOut(std::uint32_t a_index);                                             // 90 - vt+0x2D0, ID 147175
		virtual void SetAtOut(std::uint32_t a_index, NiAVObject* a_child, NiPointer<NiAVObject>& a_oldOut); // 91 - vt+0x2D8, ID 147182
		virtual void SetAtNoOut(std::uint32_t a_index, NiAVObject* a_child);                                // 92 - vt+0x2E0, ID 147181
		virtual void Unk93(void* a_arg);                                                                    // 93 - vt+0x2E8, ID 147190 - propagates up the parent chain (calls vt+0x270 on this, then parent vt+0x2E8)
		virtual void Unk94();                                                                               // 94 - vt+0x2F0 - nullsub

		// members - children is a Gamebryo NiTObjectArray, NOT a BSTArray
		// (runtime-proven: raw dump 2026-06-25 + attach/detach harness 2026-07-30)
		void*         childrenVtbl;     // 130 - NiTObjectArray vtable, REL::ID(497983)
		NiAVObject**  children;         // 138 - m_pBase; entries may be null (holes)
		std::uint16_t childrenMaxSize;  // 140
		std::uint16_t childrenSize;     // 142 - used range (includes null holes); a child's index is at NiAVObject+0x0C
		std::uint16_t childrenFree;     // 144 - null holes below childrenSize
		std::uint16_t childrenGrowBy;   // 146
		std::uint64_t unk148;           // 148
	};
	static_assert(sizeof(NiNode) == 0x150);
}
