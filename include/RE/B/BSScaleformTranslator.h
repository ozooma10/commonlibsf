#pragma once

#include "RE/B/BSFixedString.h"
#include "RE/N/NiBinaryStream.h"

#include <cstddef>
#include <span>
#include <string_view>

namespace RE
{
	// Partial 1.16.244 layout. The engine owns this wrapper and its table;
	// retaining the Scaleform implementation does not retain the table.
	class BSScaleformTranslator
	{
	public:
		// Descriptive names for the recovered synchronous loader ABI.
		class StreamParserData
		{
		public:
			virtual ~StreamParserData() = default;
			virtual bool Ready() = 0;
			virtual void Finish() = 0;
			virtual NiBinaryStream* Stream() = 0;
		};
		static_assert(sizeof(StreamParserData) == 0x8);

		struct StreamParser
		{
			wchar_t           buffer[256]{};  // 000
			wchar_t*          cursor{};       // 200
			StreamParserData* source{};       // 208
			std::uint64_t     totalBytes{};   // 210
		};
		static_assert(offsetof(StreamParser, source) == 0x208);
		static_assert(sizeof(StreamParser) == 0x218);

		struct ScaleformImpl
		{
			// Returns an owned string. Native Translate constructs its result slot.
			[[nodiscard]] BSFixedStringWCS Translate(const wchar_t* a_text);

			std::byte   unk00[0x20];  // 00: vptr, reference count, state and flags
			const void* translations; // 20: borrows wrapper + 8
		};
		static_assert(sizeof(ScaleformImpl) == 0x28);

		struct Entry
		{
			BSFixedStringWCS key;        // 00
			BSFixedStringWCS value;      // 08
			std::int32_t     nextIndex;  // 10: -1 means empty
			std::int32_t     index;      // 14
		};
		static_assert(offsetof(Entry, value) == 0x8);
		static_assert(sizeof(Entry) == 0x18);

		struct Translation
		{
			const wchar_t* key;
			const wchar_t* value;
		};

		// Both mutation APIs require the engine's initialization phase, before
		// publication to manager/GFx readers. They are not concurrent registration
		// APIs. Loading may reallocate entries; never retain entry pointers across it.
		void LoadTranslations(StreamParser* a_parser);
		[[nodiscard]] bool LoadTranslations(std::wstring_view a_resource);

		// Synchronously copies C strings into the engine pool. Keys must start with
		// '$' and contain no resource delimiters. Values preserve literal backslash-n,
		// tabs and line breaks; embedded NUL retains native C-string truncation.
		// Duplicate keys use the last value. False can leave a partially loaded table.
		[[nodiscard]] bool RegisterTranslations(std::span<const Translation> a_translations);

		// This is not a default BSTHashMap: native hashing uses pooled text addresses.
		ScaleformImpl* impl;              // 00
		std::byte      unk08[0x18];       // 08: opaque map header
		Entry*         entries;           // 20
		std::uint64_t  capacity;          // 28
		std::uint64_t  free;              // 30
		std::uint64_t  cursor;            // 38
	};
	static_assert(offsetof(BSScaleformTranslator, entries) == 0x20);
	static_assert(sizeof(BSScaleformTranslator) == 0x40);
}
