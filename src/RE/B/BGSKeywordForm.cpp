#include "RE/B/BGSKeywordForm.h"
#include "RE/B/BGSFormFolderKeywordList.h"
#include "RE/B/BGSKeyword.h"

namespace RE
{
	namespace
	{
		template <class Array>
		bool ContainsKeyword(const Array& a_keywords, const BGSKeyword* a_keyword)
		{
			for (const auto* existing : a_keywords) {
				if (existing == a_keyword) {
					return true;
				}
			}
			return false;
		}

		template <class Array>
		void CollectAllKeywordsImpl(BGSKeywordForm& a_owner, Array& a_outKeywords)
		{
			for (const auto* keyword : a_owner.keywords) {
				if (keyword) {
					a_outKeywords.push_back(keyword);
				}
			}

			for (const auto* formFolderKeywordList : a_owner.formFolderKeywordLists) {
				if (!formFolderKeywordList) {
					continue;
				}

				for (const auto* keyword : formFolderKeywordList->keywords) {
					if (keyword && !ContainsKeyword(a_outKeywords, keyword)) {
						a_outKeywords.push_back(keyword);
					}
				}
			}

			if (const auto* defaultKeyword = a_owner.GetDefaultKeyword(); defaultKeyword) {
				a_outKeywords.push_back(defaultKeyword);
			}
		}
	}

	void BGSKeywordForm::CollectAllKeywords(void* a_refOutKeywordA, const TBO_InstanceData* a_data)
	{
		(void) a_data;

		if (!a_refOutKeywordA) {
			return;
		}

		auto& outKeywords = *static_cast<BSScrapArray<const BGSKeyword*>*>(a_refOutKeywordA);
		CollectAllKeywordsImpl(*this, outKeywords);
	}

	void BGSKeywordForm::CollectAllKeywordsToArray(BSTArray<const BGSKeyword*>& a_outKeywords, const TBO_InstanceData* a_data)
	{
		(void) a_data;
		CollectAllKeywordsImpl(*this, a_outKeywords);
	}

	bool BGSKeywordForm::ContainsKeywordString(std::string_view a_editorID)
	{
		bool result{};
		ForEachKeyword([&](const BGSKeyword* a_keyword) {
			if (result = a_keyword->formEditorID.contains(a_editorID); result) {
				return BSContainer::ForEachResult::kStop;
			}
			return BSContainer::ForEachResult::kContinue;
		});
		return result;
	}

	void BGSKeywordForm::ForEachKeyword(std::function<BSContainer::ForEachResult(BGSKeyword*)> a_callback)
	{
		for (const auto& keyword : keywords) {
			if (keyword && a_callback(keyword) == BSContainer::ForEachResult::kStop) {
				return;
			}
		}
		for (const auto& formFolderKeywordList : formFolderKeywordLists) {
			if (formFolderKeywordList) {
				for (const auto& keyword : formFolderKeywordList->keywords) {
					if (keyword && a_callback(keyword) == BSContainer::ForEachResult::kStop) {
						return;
					}
				}
			}
		}
	}

	std::uint32_t BGSKeywordForm::GetNumKeywords() const
	{
		return keywords.size();
	}

	bool BGSKeywordForm::HasKeywordString(std::string_view a_editorID)
	{
		bool result{};
		ForEachKeyword([&](const BGSKeyword* a_keyword) {
			if (result = (a_keyword->formEditorID == a_editorID); result) {
				return BSContainer::ForEachResult::kStop;
			}
			return BSContainer::ForEachResult::kContinue;
		});
		return result;
	}
}
