#pragma once

namespace RE
{
	class BGSKeyword;
	class TBO_InstanceData;

	class IKeywordFormBase
	{
	public:
		SF_RTTI(IKeywordFormBase);

		virtual ~IKeywordFormBase();  // 00

		// add
		virtual bool AddKeyword(BGSKeyword* a_keyword, void** a_extraData);       // 01 - returns non-zero on success, keyword passed by value
		virtual void CollectForPropagation(void* a_outBSTArray, void** a_extraData);  // 02 - fills pre-allocated BSTArray with affected items
		virtual void CollectAllKeywords(void* /* BSScrapArray<const BGSKeyword> */ a_outKeywords, const TBO_InstanceData* a_data);  // 03
	};
}
