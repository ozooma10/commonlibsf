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

	// FIXED FOR STARFIELD (2026-06-09, exe 1.16.242): the compiled
	// BSTSingletonSDM is POLYMORPHIC — a 0x10 subobject with a vptr at +0x00
	// and its own bases at +0x08/+0x09. Exe RTTI proved this across all 7
	// audited singletons (UI, GameVM, ActorEquipManager, ConsoleLog,
	// PlayerCamera, BGSInventoryInterface, ChargenDataModel), and static
	// vtable analysis of UI (primary vtable 0x144D824B8 = exactly ONE slot =
	// the scalar deleting destructor) proved the SDM declares exactly one
	// virtual: the destructor. EngineShapeSDM_Mirror in
	// src/Test/UILayoutTests.cpp static_asserts that this shape is 0x10 under
	// MSVC; SdmLayoutTests verifies the deriving headers' base offsets against
	// the RTTI-proven maps at runtime. Deriving headers were re-padded with
	// this change — see each class for its per-class proof status.
#ifdef __EDG__

	template <class T, template <class> class Buffer = BSTSingletonSDMOpStaticBuffer>
	struct BSTSingletonSDM
	{
	public:
		std::uint8_t padding[0x10];
	};

#else

	template <class T, template <class> class Buffer = BSTSingletonSDMOpStaticBuffer>
	struct BSTSingletonSDM :
		public BSTSingletonSDMBase<BSTSDMTraits<T, Buffer<T>>>
	{
	public:
		using value_type = T;

		virtual ~BSTSingletonSDM() = default;  // 00 — the engine SDM's single virtual
	};

	static_assert(sizeof(BSTSingletonSDM<void*>) == 0x10);

#endif
}
