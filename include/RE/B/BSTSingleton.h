#pragma once

namespace RE
{
	template <class T>
	class BSTSingletonImplicit
	{
	public:
		using value_type = T;
	};

	template <class T>
	class BSTSingletonExplicit
	{
	public:
		using value_type = T;
	};

	template <class T>
	struct BSTSingletonSDMOpStaticBuffer
	{
	public:
		using value_type = T;
	};

	template <class Traits>
	struct BSTSingletonSDMBase :
		public Traits,
		public BSTSingletonSDMOpStaticBuffer<typename Traits::value_type>
	{
	public:
	};

	template <class T, class Buffer>
	struct BSTSDMTraits
	{
	public:
		using value_type = T;
	};

	// WARNING — PROVEN WRONG FOR STARFIELD (2026-06-09, exe 1.16.242): the
	// compiled BSTSingletonSDM is POLYMORPHIC. Exe RTTI (BaseClassArray of all
	// 7 audited singletons: UI, GameVM, ActorEquipManager, ConsoleLog,
	// PlayerCamera, BGSInventoryInterface, ChargenDataModel) shows the same
	// hierarchy as below but with a vptr at +0x00 and the SDM's own bases at
	// mdisp +0x08/+0x09 — a 0x10-byte subobject. Static vtable analysis of UI
	// (primary vtable 0x144D824B8 = exactly ONE slot = the scalar deleting
	// destructor) proves the SDM declares exactly one virtual: the destructor.
	// The correct definition is this template plus
	//     virtual ~BSTSingletonSDM() = default;
	// (see EngineShapeSDM_Mirror in src/Test/UILayoutTests.cpp for the layout
	// proof). NOT applied here yet because every deriving header's member
	// offsets were authored against the empty 0-byte base — apply the fix and
	// re-pad the deriving classes together, with per-class proof for the
	// not-yet-audited ones. Until then, UI models it locally
	// (BSTSingletonSDM_UI in RE/U/UI.h).
#ifdef __EDG__

	template <class T, template <class> class Buffer = BSTSingletonSDMOpStaticBuffer>
	struct BSTSingletonSDM
	{
	public:
		std::uint8_t padding;
	};

#else

	template <class T, template <class> class Buffer = BSTSingletonSDMOpStaticBuffer>
	struct BSTSingletonSDM :
		public BSTSingletonSDMBase<BSTSDMTraits<T, Buffer<T>>>
	{
	public:
		using value_type = T;
	};

#endif
}
