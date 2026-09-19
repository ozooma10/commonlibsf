#include "RE/B/BSScaleformTranslator.h"

#include <algorithm>
#include <cstring>
#include <string>
#include <vector>

namespace RE
{
	namespace
	{
		// A borrowed, synchronous resource stream. The native wide parser advances
		// NiBinaryStream::absolutePosition after DoRead; DoRead must not advance it.
		class TranslationStream final : public NiBinaryStream, public BSScaleformTranslator::StreamParserData
		{
		public:
			explicit TranslationStream(std::wstring_view a_text) :
				_bytes(reinterpret_cast<const std::byte*>(a_text.data())),
				_size(a_text.size() * sizeof(wchar_t))
			{}

			explicit operator bool() override { return _valid; }
			void Seek(std::int64_t) override { _valid = false; }
			std::uint64_t GetPosition() const override { return absolutePosition; }
			void GetBufferInfo(BufferInfo& a_info) override { a_info = {}; a_info.totalSize = _size; }
			std::uint64_t DoRead(void* a_buffer, std::uint64_t a_bytes) override
			{
				if (absolutePosition > _size) {
					_valid = false;
					return 0;
				}
				const auto count = (std::min)(a_bytes, _size - absolutePosition);
				if (count) std::memcpy(a_buffer, _bytes + absolutePosition, static_cast<std::size_t>(count));
				return count;
			}
			std::uint64_t DoWrite(const void*, std::uint64_t) override { _valid = false; return 0; }
			bool Ready() override { return _valid; }
			void Finish() override { finished = true; }
			NiBinaryStream* Stream() override { return this; }

			bool finished{};

		private:
			const std::byte* _bytes;
			std::uint64_t    _size;
			bool             _valid{ true };
		};
	}

	BSFixedStringWCS BSScaleformTranslator::ScaleformImpl::Translate(const wchar_t* a_text)
	{
		BSFixedStringWCS result;
		using func_t = BSFixedStringWCS* (*)(ScaleformImpl*, BSFixedStringWCS*, const wchar_t*);
		static REL::Relocation<func_t> func{ ID::BSScaleformTranslator::ScaleformImpl::Translate };
		func(this, &result, a_text);
		return result;
	}

	void BSScaleformTranslator::LoadTranslations(StreamParser* a_parser)
	{
		using func_t = void (*)(BSScaleformTranslator*, StreamParser*);
		static REL::Relocation<func_t> func{ ID::BSScaleformTranslator::LoadTranslations };
		func(this, a_parser);
	}

	bool BSScaleformTranslator::LoadTranslations(std::wstring_view a_resource)
	{
		if (a_resource.empty()) return true;
		TranslationStream stream(a_resource);
		StreamParser parser;
		parser.source = &stream;
		parser.totalBytes = a_resource.size() * sizeof(wchar_t);
		LoadTranslations(&parser);
		return stream.Ready() && stream.finished && !parser.cursor && stream.absolutePosition == parser.totalBytes;
	}

	bool BSScaleformTranslator::RegisterTranslations(std::span<const Translation> a_translations)
	{
		if (a_translations.empty()) return true;
		std::wstring resource;
		std::vector<std::pair<std::wstring, std::wstring>> translations;
		translations.reserve(a_translations.size());
		for (const auto& translation : a_translations) {
			if (!translation.key) return false;
			const std::wstring_view key(translation.key);
			if (key.size() < 2 || key.front() != L'$' || key.find_first_of(L"\t\r\n\uFEFF") != key.npos) return false;
			// A caller may borrow text from a value the native load will replace.
			translations.emplace_back(key, translation.value ? translation.value : L"");
			resource.append(key).append(L"\tplaceholder\r\n");
		}
		if (!LoadTranslations(resource) || !entries || free > capacity) return false;

		// Resolve after the complete load: insertion/growth invalidates old nodes.
		// Compare pooled keys directly without assuming the map's hash algorithm.
		using assign_t = BSFixedStringWCS* (*)(BSFixedStringWCS*, const wchar_t*);
		static REL::Relocation<assign_t> assign{ ID::BSFixedStringWCS::Assign };
		for (const auto& [keyText, value] : translations) {
			const BSFixedStringWCS key(keyText.c_str());
			const auto table = std::span(entries, capacity);
			const auto found = std::ranges::find_if(table, [&](const Entry& a_entry) {
				return a_entry.nextIndex != -1 && a_entry.key == key;
			});
			if (found == table.end()) return false;
			assign(&found->value, value.c_str());
		}
		return true;
	}
}
