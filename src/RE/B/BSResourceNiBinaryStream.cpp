#include "RE/B/BSResourceNiBinaryStream.h"

namespace RE
{
	BSResourceNiBinaryStream::BSResourceNiBinaryStream(const char* a_path)
	{
		ctor(a_path);
	}

	BSResourceNiBinaryStream::BSResourceNiBinaryStream(const std::string& a_path) :
		BSResourceNiBinaryStream(a_path.c_str())
	{}

	BSResourceNiBinaryStream::~BSResourceNiBinaryStream()
	{
		dtor();
	}

	BSResourceNiBinaryStream::operator bool()
	{
		return Good();
	}

	void BSResourceNiBinaryStream::Seek(std::int64_t a_numBytes)
	{
		using func_t = decltype(&BSResourceNiBinaryStream::Seek);
		static REL::Relocation<func_t> func{ ID::BSResourceNiBinaryStream::Seek };
		return func(this, a_numBytes);
	}

	std::uint64_t BSResourceNiBinaryStream::GetPosition() const
	{
		return streamPosition;
	}

	void BSResourceNiBinaryStream::GetBufferInfo(BufferInfo& a_info)
	{
		using func_t = decltype(&BSResourceNiBinaryStream::GetBufferInfo);
		static REL::Relocation<func_t> func{ ID::BSResourceNiBinaryStream::GetBufferInfo };
		return func(this, a_info);
	}

	std::uint64_t BSResourceNiBinaryStream::DoRead(void* a_buffer, std::uint64_t a_bytes)
	{
		using func_t = decltype(&BSResourceNiBinaryStream::DoRead);
		static REL::Relocation<func_t> func{ ID::BSResourceNiBinaryStream::DoRead };
		return func(this, a_buffer, a_bytes);
	}

	std::uint64_t BSResourceNiBinaryStream::DoWrite(const void* a_buffer, std::uint64_t a_bytes)
	{
		using func_t = decltype(&BSResourceNiBinaryStream::DoWrite);
		static REL::Relocation<func_t> func{ ID::BSResourceNiBinaryStream::DoWrite };
		return func(this, a_buffer, a_bytes);
	}

	bool BSResourceNiBinaryStream::Good() const noexcept
	{
		return static_cast<bool>(stream);
	}

	std::uint32_t BSResourceNiBinaryStream::GetSize() const noexcept
	{
		return stream ? stream->totalSize : 0;
	}

	BSResourceNiBinaryStream* BSResourceNiBinaryStream::ctor(const char* a_path)
	{
		using func_t = decltype(&BSResourceNiBinaryStream::ctor);
		static REL::Relocation<func_t> func{ ID::BSResourceNiBinaryStream::ctor };
		return func(this, a_path);
	}

	void BSResourceNiBinaryStream::dtor()
	{
		using func_t = decltype(&BSResourceNiBinaryStream::dtor);
		static REL::Relocation<func_t> func{ ID::BSResourceNiBinaryStream::dtor };
		return func(this);
	}
}
