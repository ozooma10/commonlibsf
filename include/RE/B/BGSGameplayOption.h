#pragma once

#include "RE/T/TESForm.h"

namespace RE
{
	class BGSGameplayOption : public TESForm
	{
	public:
		SF_RTTI_VTABLE(BGSGameplayOption);
		SF_FORMTYPE(GPOF);

		~BGSGameplayOption() override;  // 00

		[[nodiscard]] std::uint64_t GetIndex() const
		{
			using func_t = decltype(&BGSGameplayOption::GetIndex);
			static REL::Relocation<func_t> func{ ID::BGSGameplayOption::GetIndex };
			return func(this);
		}

		// members
		std::byte unk38[0x48];  // 38
	};
	static_assert(sizeof(BGSGameplayOption) == 0x80);
}
