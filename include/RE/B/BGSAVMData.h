#pragma once

#include "RE/A/AVMData.h"
#include "RE/B/BSTHeapSTLVector.h"
#include "RE/T/TESForm.h"

#include <span>

namespace RE
{
	class BGSAVMData : public TESForm
	{
	public:
		SF_RTTI_VTABLE(BGSAVMData);
		SF_FORMTYPE(AVMD);

		enum class TextureType : std::uint32_t
		{
			kNone = 0,
			kUnknown = 1 << 0,
			kRough = 1 << 1,
			kOpacity = 1 << 2,
			kNormal = 1 << 3,
			kAO = 1 << 4,
			kMetal = 1 << 5,
			kBiomeConditions = 1 << 6,
			kEmissive = 1 << 7
		};

		~BGSAVMData() override;  // 00

		[[nodiscard]] std::span<const AVMData::Entry> GetEntries() const noexcept
		{
			return { entries.data(), entries.size() };
		}

		// members
		BSFixedString                              editorID;     // 38
		BGSAVMData*                               self;         // 40
		REX::TEnum<AVMData::Type, std::uint32_t>  groupType;    // 48
		std::uint32_t                             pad4C;        // 4C
		BSFixedString                              nameYNAM;     // 50
		BSFixedString                              nameTNAM;     // 58
		BSTHeapSTLVector<AVMData::Entry>           entries;      // 60
		REX::TEnumSet<TextureType, std::uint32_t>  textureType;  // 78
		std::uint32_t                             pad7C;        // 7C
		BGSAVMData*                               avmsParent;   // 80
	};
	static_assert(sizeof(BGSAVMData) == 0x88);
}
