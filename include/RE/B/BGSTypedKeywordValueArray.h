#pragma once

#include "RE/B/BGSKeyword.h"

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
		// members
		std::uint16_t keywordIndex;  // 0
	};

	namespace detail
	{
		[[nodiscard]] BGSKeyword* BGSKeywordGetTypedKeywordByIndex(KeywordType a_type, std::uint16_t a_index);
	}

	template <KeywordType TYPE>
	class BGSTypedKeywordValueArray
	{
	public:
		[[nodiscard]] std::uint32_t size() const
		{
			return _end && _begin ? static_cast<std::uint32_t>(_end - _begin) : 0;
		}

		[[nodiscard]] bool HasKeyword(BGSKeyword* a_keyword)
		{
			for (auto it = _begin; it != _end; ++it) {
				if (*it == a_keyword) {
					return true;
				}
			}
			return false;
		}

		[[nodiscard]] BGSKeyword* operator[](std::uint32_t a_index) const
		{
			return _begin[a_index];
		}

		[[nodiscard]] BGSKeyword** begin() const { return _begin; }
		[[nodiscard]] BGSKeyword** end() const { return _end; }

		// members
		BGSKeyword** _begin;        // 00 - pointer to first BGSKeyword*
		BGSKeyword** _end;          // 08 - one-past-last BGSKeyword*
		BGSKeyword** _capacityEnd;  // 10 - end of allocated storage
	};
	static_assert(sizeof(BGSTypedKeywordValueArray<KeywordType::kNone>) == 0x18);
}
