#pragma once

#include "RE/B/BSResourceEnums.h"
#include "RE/B/BSTSmartPointer.h"
#include "RE/N/NiBinaryStream.h"
#include "RE/S/Stream.h"

#include <string>

namespace RE
{
	class BSResourceNiBinaryStream final :
		public NiBinaryStream
	{
	public:
		SF_RTTI_VTABLE(BSResourceNiBinaryStream);

		struct BufferType;

		BSResourceNiBinaryStream() = delete;
		explicit BSResourceNiBinaryStream(const char* a_path);
		explicit BSResourceNiBinaryStream(const std::string& a_path);
		BSResourceNiBinaryStream(const BSResourceNiBinaryStream&) = delete;
		BSResourceNiBinaryStream(BSResourceNiBinaryStream&&) = delete;
		~BSResourceNiBinaryStream() override;  // 00

		BSResourceNiBinaryStream& operator=(const BSResourceNiBinaryStream&) = delete;
		BSResourceNiBinaryStream& operator=(BSResourceNiBinaryStream&&) = delete;

		explicit operator bool() override;                                      // 01
		void          Seek(std::int64_t a_numBytes) override;                   // 02
		std::uint64_t GetPosition() const override;                             // 03
		void          GetBufferInfo(BufferInfo& a_info) override;               // 04
		std::uint64_t DoRead(void* a_buffer, std::uint64_t a_bytes) override;   // 05
		std::uint64_t DoWrite(const void* a_buffer, std::uint64_t a_bytes) override;  // 06

		[[nodiscard]] bool          Good() const noexcept;
		[[nodiscard]] std::uint32_t GetSize() const noexcept;

	private:
		BSResourceNiBinaryStream* ctor(const char* a_path);
		void                      dtor();

		BSTSmartPointer<BSResource::Stream> stream;                            // 10
		BufferType*                         buffer{ nullptr };                  // 18
		std::uint64_t                       streamPosition{ 0 };                // 20
		BSResource::ErrorCode               lastError{ BSResource::ErrorCode::kNone };  // 28
	};
	static_assert(sizeof(BSResourceNiBinaryStream) == 0x30);
}
