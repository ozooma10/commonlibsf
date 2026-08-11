#pragma once

#include <cstdint>

namespace RE
{
	class __declspec(novtable) NiBinaryStream
	{
	public:
		SF_RTTI_VTABLE(NiBinaryStream);

		struct BufferInfo
		{
			void*         buffer{ nullptr };          // 00
			std::uint64_t totalSize{ 0 };             // 08
			std::uint64_t bufferAllocationSize{ 0 };  // 10
			std::uint64_t bufferReadSize{ 0 };        // 18
			std::uint64_t bufferPosition{ 0 };        // 20
			std::uint64_t streamPosition{ 0 };        // 28
		};
		static_assert(sizeof(BufferInfo) == 0x30);

		NiBinaryStream() = default;
		virtual ~NiBinaryStream() = default;  // 00

		virtual explicit operator bool() = 0;                      // 01
		virtual void          Seek(std::int64_t a_numBytes) = 0;    // 02
		virtual std::uint64_t GetPosition() const = 0;              // 03
		virtual void          GetBufferInfo(BufferInfo& a_info);     // 04
		virtual std::uint64_t DoRead(void* a_buffer, std::uint64_t a_bytes) = 0;        // 05
		virtual std::uint64_t DoWrite(const void* a_buffer, std::uint64_t a_bytes) = 0;  // 06

		std::uint64_t absolutePosition{ 0 };  // 08
	};
	static_assert(sizeof(NiBinaryStream) == 0x10);
}
