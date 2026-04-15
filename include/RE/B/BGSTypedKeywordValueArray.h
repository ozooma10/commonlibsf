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
		std::uint16_t keywordIndex;  // 0
		std::uint16_t pad02;         // 2
		std::uint32_t pad04;         // 4
	};
	static_assert(sizeof(BGSTypedKeywordValue<KeywordType::kNone>) == 0x8);

	namespace detail
	{
		[[nodiscard]] BGSKeyword* BGSKeywordGetTypedKeywordByIndex(KeywordType a_type, std::uint16_t a_index);
	}

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
				const auto kywd = detail::BGSKeywordGetTypedKeywordByIndex(TYPE, it->keywordIndex);
				if (kywd == a_keyword) {
					return true;
				}
			}
			return false;
		}

		[[nodiscard]] bool AddKeyword(BGSKeyword* a_keyword)
		{
			if (!a_keyword) {
				REX::WARN("AddKeyword: a_keyword is null");
				return false;
			}

			auto first = reinterpret_cast<BGSKeyword**>(begin);
			auto last = reinterpret_cast<BGSKeyword**>(end);

			for (auto it = first; it != last; ++it) {
				if (*it && (*it)->formID == a_keyword->formID) {
					REX::DEBUG("AddKeyword: keyword already exists {:08X}", a_keyword->formID);
					return false;
				}
			}

			const auto oldSize = static_cast<std::size_t>(last - first);
			const auto newSize = oldSize + 1;

			auto* newData = static_cast<BGSKeyword**>(
				RE::MemoryManager::GetSingleton()->Allocate(sizeof(BGSKeyword*) * newSize, 0, false));

			if (!newData) {
				REX::WARN("AddKeyword: allocation failed for keyword {:08X}", a_keyword->formID);
				return false;
			}

			for (std::size_t i = 0; i < oldSize; ++i) {
				newData[i] = first[i];
			}

			newData[oldSize] = a_keyword;

			auto oldFirst = first;
			auto oldLast = last;
			auto oldCap = reinterpret_cast<BGSKeyword**>(capacityEnd);

			begin = reinterpret_cast<BGSTypedKeywordValue<TYPE>*>(newData);
			end = reinterpret_cast<BGSTypedKeywordValue<TYPE>*>(newData + newSize);
			capacityEnd = reinterpret_cast<BGSTypedKeywordValue<TYPE>*>(newData + newSize);

			REX::DEBUG(
				"AddKeyword: reallocated array for keyword {:08X}, oldBegin={} oldEnd={} oldCap={}, newBegin={} newEnd={}",
				a_keyword->formID,
				static_cast<const void*>(oldFirst),
				static_cast<const void*>(oldLast),
				static_cast<const void*>(oldCap),
				static_cast<const void*>(begin),
				static_cast<const void*>(end));

			return true;
		}

		[[nodiscard]] bool RemoveKeyword(BGSKeyword* a_keyword)
		{
			if (!a_keyword) {
				return false;
			}
			auto first = reinterpret_cast<BGSKeyword**>(begin);
			auto last = reinterpret_cast<BGSKeyword**>(end);
			for (auto it = first; it != last; ++it) {
				if (*it && (*it)->formID == a_keyword->formID) {
					for (auto jt = it; jt + 1 != last; ++jt) {
						*jt = *(jt + 1);
					}
					--last;
					*last = nullptr;
					end = reinterpret_cast<BGSTypedKeywordValue<TYPE>*>(last);
					return true;
				}
			}
			return false;
		}

		BGSTypedKeywordValue<TYPE>* begin;        // 00
		BGSTypedKeywordValue<TYPE>* end;          // 08
		BGSTypedKeywordValue<TYPE>* capacityEnd;  // 10
	};
	static_assert(sizeof(BGSTypedKeywordValueArray<KeywordType::kNone>) == 0x18);
}
