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

	// the compiled BSTSingletonSDM is polymorphic: a 0x10 subobject with a
	// vptr at +0x00 (its single virtual is the destructor) and its own bases
	// at +0x08/+0x09
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

		virtual ~BSTSingletonSDM() = default;  // 00
	};

	static_assert(sizeof(BSTSingletonSDM<void*>) == 0x10);

#endif
}
