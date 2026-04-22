#pragma once

#include "RE/B/BGSKeyword.h"
#include "RE/M/MemoryManager.h"

namespace RE
{
	enum class KeywordType
	{
		kNone,
		kComponentTechLevel,
		kAttachPoint,
		kComponentProperty,
		kInstantiationFilter,
		kModAssociation,
		kSound,
		kAnimArchetype,
		kFunctionCall,
		kRecipeFilter,
		kAttractionType,
		kDialogueSubtype,
		kQuestTarget,
		kAnimFlavor,
		kAnimGender,
		kAnimFaceArchetype,
		kQuestGroup,
		kAnimInjured,
		kDispelEffect,

		kTotal
	};

	template <KeywordType TYPE>
	class BGSTypedKeywordValue
	{
	public:
		std::uint16_t keywordIndex;  // 0
		std::uint16_t pad02;         // 2
		std::uint32_t pad04;         // 4
	};
	static_assert(sizeof(BGSTypedKeywordValue<KeywordType::kNone>) == 0x8);

	// Best-fit model: BGSTypedKeywordValueArray<KeywordType::kAttachPoint> may be
	// BSComponentDB2-backed index records during preload/finalize, while loaded
	// ARMO/WEAP attachParents runtime data is a contiguous BGSKeyword* buffer.
	template <KeywordType TYPE>
	class BGSTypedKeywordValueArray
	{
	public:
		[[nodiscard]] bool HasKeyword(BGSKeyword* a_keyword)
		{
			if (!a_keyword) {
				return false;
			}

			for (auto it = begin; it != end; ++it) {
				if (*it == a_keyword) {
					return true;
				}
			}
			return false;
		}

		[[nodiscard]] bool AddKeyword(BGSKeyword* a_keyword)
		{
			if (!a_keyword) {
				return false;
			}

			for (auto it = begin; it != end; ++it) {
				if (*it && (*it)->formID == a_keyword->formID) {
					return false;
				}
			}

			// Fast path: existing buffer has spare capacity. Avoids heap churn
			// when the field was grown previously.
			if (end < capacityEnd) {
				*end = a_keyword;
				++end;
				return true;
			}

			// Slow path: allocate a new buffer (with headroom), copy the old
			// contents, release the old buffer so we don't leak on repeated
			// mutation. Freeing via MemoryManager is proven compatible with the
			// engine's BSTHeapSTLAllocator (same global heap).
			const auto oldSize = static_cast<std::size_t>(end - begin);
			const auto newSize = oldSize + 1;
			// Double-and-min-4 growth matches our BSTHeapSTLVector policy and
			// keeps amortized cost down for bulk-patch scenarios.
			const auto oldCapacity = static_cast<std::size_t>(capacityEnd - begin);
			auto       newCapacity = oldCapacity * 2;
			if (newCapacity < 4) {
				newCapacity = 4;
			}
			if (newCapacity < newSize) {
				newCapacity = newSize;
			}

			auto* newData = static_cast<BGSKeyword**>(
				RE::MemoryManager::GetSingleton()->Allocate(sizeof(BGSKeyword*) * newCapacity, 0, false));

			if (!newData) {
				return false;
			}

			for (std::size_t i = 0; i < oldSize; ++i) {
				newData[i] = begin[i];
			}

			newData[oldSize] = a_keyword;
			if (begin) {
				RE::MemoryManager::GetSingleton()->Free(begin, false);
			}
			begin = newData;
			end = newData + newSize;
			capacityEnd = newData + newCapacity;
			return true;
		}

		[[nodiscard]] bool RemoveKeyword(BGSKeyword* a_keyword)
		{
			if (!a_keyword) {
				return false;
			}
			for (auto it = begin; it != end; ++it) {
				if (*it && (*it)->formID == a_keyword->formID) {
					for (auto jt = it; jt + 1 != end; ++jt) {
						*jt = *(jt + 1);
					}
					--end;
					*end = nullptr;
					return true;
				}
			}
			return false;
		}

		BGSKeyword** begin;        // 00
		BGSKeyword** end;          // 08
		BGSKeyword** capacityEnd;  // 10
	};
	static_assert(sizeof(BGSTypedKeywordValueArray<KeywordType::kNone>) == 0x18);
}
