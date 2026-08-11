#pragma once

#include "RE/A/AVMData.h"
#include "RE/B/BSFixedString.h"
#include "RE/B/BSTArray.h"

namespace RE
{
	class BGSAVMData;
}

namespace RE::BSFaceDB
{
	[[nodiscard]] bool ResolveEntry(
		std::uint32_t a_storeIndex,
		const BSFixedString& a_category,
		const BSFixedString& a_value,
		AVMData::Entry& a_entry);

	void GetLayerValues(
		std::uint8_t a_skinTone,
		const BSFixedString& a_category,
		bool a_unk2,
		BSScrapArray<BSFixedString>& a_values);

	[[nodiscard]] BGSAVMData* FindCategoryData(const BSFixedString& a_category);

	[[nodiscard]] std::uint32_t FindSimpleColorIndex(
		std::uint32_t a_storeIndex,
		const BSFixedString& a_category,
		const BSFixedString& a_value);

	[[nodiscard]] std::uint32_t FindMappedColorIndex(
		const BSFixedString& a_primary,
		const BSFixedString& a_secondary,
		const BSFixedString& a_value);

	[[nodiscard]] const BSFixedString& GetHairColorCategory();
	[[nodiscard]] const BSFixedString& GetFacialHairColorCategory();
	[[nodiscard]] const BSFixedString& GetTeethCategory();
	[[nodiscard]] const BSFixedString& GetJewelryColorCategory();
	[[nodiscard]] const BSFixedString& GetEyeColorCategory();
}
