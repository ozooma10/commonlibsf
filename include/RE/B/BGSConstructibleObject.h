#pragma once

#include "RE/B/BGSCraftableForm.h"
#include "RE/B/BGSCraftingUseSound.h"
#include "RE/B/BGSPickupPutdownSounds.h"
#include "RE/B/BSTHeapSTLVector.h"
#include "RE/B/BSTList.h"
#include "RE/T/TESValueForm.h"

namespace RE
{
	class BGSKeyword;
	class TESBoundObject;
	class TESGlobal;

	class BGSConstructibleObject :
		public BGSCraftableForm,        // 000
		public BGSPickupPutdownSounds,  // 0A0
		public TESValueForm,            // 108
		public BGSCraftingUseSound      // 118
	{
	public:
		SF_RTTI_VTABLE(BGSConstructibleObject);
		SF_FORMTYPE(COBJ);

		enum class LEARN_METHOD
		{
			kPickupOrScript,
			kScrapping,
			kIngestion,
			kDefaultOrConditionsMet,
			kPlan
		};

		~BGSConstructibleObject() override;  // 00

		// Runtime bulk-patching of the CK RecipeFilters keyword list (the
		// `category` member). Safe on any COBJ regardless of which mod authored
		// it; mutates via MemoryManager so the engine can free the buffer on
		// shutdown. Call from the main thread at a stable time (e.g. OnDataLoaded
		// or a script callback); not thread-safe.
		[[nodiscard]] bool HasCategoryKeyword(const BGSKeyword* a_keyword) const noexcept
		{
			if (!a_keyword) {
				return false;
			}
			for (const auto* kw : category) {
				if (kw == a_keyword) {
					return true;
				}
			}
			return false;
		}

		bool AddCategoryKeyword(BGSKeyword* a_keyword)
		{
			if (!a_keyword || HasCategoryKeyword(a_keyword)) {
				return false;
			}
			category.push_back(a_keyword);
			return true;
		}

		bool RemoveCategoryKeyword(const BGSKeyword* a_keyword)
		{
			if (!a_keyword) {
				return false;
			}
			return category.erase_value(const_cast<BGSKeyword*>(a_keyword));
		}

		// members
		// category models the engine's `std::vector<BGSKeyword*, BSTHeapSTLAllocator<BGSKeyword, 2>>`.
		// Modeled as BSTHeapSTLVector so we can safely read and mutate via the
		// game's MemoryManager. CK field name: RecipeFilters.
		BSTHeapSTLVector<BGSKeyword*>                                                   category;               // 150 - CK RecipeFilters
		TESBoundObject*                                                                 learnedFrom;            // 168 - CK LearnedFrom (any TESBoundObject)
		BGSCurveForm*                                                                   baseReturnScaleTable;   // 170 - CK BaseReturnScaleTable
		TESGlobal*                                                                      maxBuildCount;          // 178 - CK MaxBuildCount
		BSTArray<BSTTuple3<TESForm*, BGSCurveForm*, BGSTypedFormValuePair::SharedVal>>* workbenchRepairRecipe;  // 180 - CK WorkbenchRepairRecipe
		REX::TEnum<LEARN_METHOD, std::uint8_t>                                          learnMethod;            // 188 - CK LearnMethod enum
		TESGlobal*                                                                      unk190;                 // 190 - TESGlobal???
		BGSKeyword*                                                                     unk198;                 // 198 - BGSKeyword???
		std::uint32_t                                                                   flags;                  // 1A0 - CK flag bitfield incl. ExcludeFromWorkshopLOD (bit layout TBD) (???)
	};
	static_assert(sizeof(BGSConstructibleObject) == 0x1A8);
	static_assert(offsetof(BGSConstructibleObject, pickupSound) == 0xA8);        // BGSPickupPutdownSounds base @ +0x0A0
	static_assert(offsetof(BGSConstructibleObject, value) == 0x110);             // TESValueForm base @ +0x108
	static_assert(offsetof(BGSConstructibleObject, craftingUseSound) == 0x120);  // BGSCraftingUseSound base @ +0x118
	static_assert(offsetof(BGSConstructibleObject, category) == 0x150);          // first derived member
}
