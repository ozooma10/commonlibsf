#include "RE/B/BSFaceDB.h"

#include "RE/B/BGSAVMData.h"

namespace RE::BSFaceDB
{
	void GetLayerValues(
		std::uint8_t a_skinTone,
		const BSFixedString& a_category,
		bool a_unk2,
		BSScrapArray<BSFixedString>& a_values)
	{
		using func_t = void (*)(
			std::uint8_t, const BSFixedString*, bool, BSScrapArray<BSFixedString>*);
		static REL::Relocation<func_t> func{ ID::BSFaceDB::GetLayerValues };
		func(a_skinTone, std::addressof(a_category), a_unk2, std::addressof(a_values));
	}

	BGSAVMData* FindCategoryData(const BSFixedString& a_category)
	{
		using func_t = bool (*)(void*, const BSFixedString*, void**);
		static REL::Relocation<func_t> find{ ID::BSFaceDB::FindCategoryData };
		// CategoryMapEntries addresses the table field at map+0x18.
		static REL::Relocation<void*> categoryMap{
			ID::BSFaceDB::CategoryMapEntries, -0x18
		};

		void* data = nullptr;
		return find(categoryMap.get(), std::addressof(a_category), std::addressof(data)) ?
			static_cast<BGSAVMData*>(data) :
			nullptr;
	}

	std::uint32_t FindSimpleColorIndex(
		std::uint32_t a_storeIndex,
		const BSFixedString& a_category,
		const BSFixedString& a_value)
	{
		using func_t = std::uint32_t (*)(
			std::uint32_t, const BSFixedString*, const BSFixedString*);
		static REL::Relocation<func_t> func{ ID::BSFaceDB::FindSimpleColorIndex };
		return func(a_storeIndex, std::addressof(a_category), std::addressof(a_value));
	}

	std::uint32_t FindMappedColorIndex(
		const BSFixedString& a_primary,
		const BSFixedString& a_secondary,
		const BSFixedString& a_value)
	{
		using func_t = std::uint32_t (*)(
			const BSFixedString*, const BSFixedString*, const BSFixedString*);
		static REL::Relocation<func_t> func{ ID::BSFaceDB::FindMappedColorIndex };
		return func(
			std::addressof(a_primary),
			std::addressof(a_secondary),
			std::addressof(a_value));
	}

	const BSFixedString& GetHairColorCategory()
	{
		static REL::Relocation<const BSFixedString*> category{ ID::BSFaceDB::HairColorCategory };
		return *category;
	}

	const BSFixedString& GetFacialHairColorCategory()
	{
		static REL::Relocation<const BSFixedString*> category{ ID::BSFaceDB::FacialHairColorCategory };
		return *category;
	}

	const BSFixedString& GetTeethCategory()
	{
		static REL::Relocation<const BSFixedString*> category{ ID::BSFaceDB::TeethCategory };
		return *category;
	}

	const BSFixedString& GetJewelryColorCategory()
	{
		static REL::Relocation<const BSFixedString*> category{ ID::BSFaceDB::JewelryColorCategory };
		return *category;
	}

	const BSFixedString& GetEyeColorCategory()
	{
		static REL::Relocation<const BSFixedString*> category{ ID::BSFaceDB::EyeColorCategory };
		return *category;
	}
}
