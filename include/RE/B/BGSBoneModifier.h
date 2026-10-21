#pragma once

#include "RE/T/TESForm.h"

namespace RE
{
	class BGSBoneModifier : public TESForm
	{
	public:
		SF_RTTI_VTABLE(BGSBoneModifier);
		SF_FORMTYPE(BMOD);

		~BGSBoneModifier() override;  // 00

		// members
		void* unk30;  // 38 - BoneModifierData*; legacy member name retained
	};
	static_assert(sizeof(BGSBoneModifier) == 0x40);
}
