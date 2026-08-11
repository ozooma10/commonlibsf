#include "RE/T/TESRace.h"

namespace RE
{
	const BSFixedStringCS* TESRace::GetBoneGroupName(SEX a_sex, std::uint32_t a_index)
	{
		using func_t = const BSFixedStringCS* (*)(TESRace*, std::int32_t, std::uint32_t);
		static REL::Relocation<func_t> func{ ID::TESRace::GetBoneGroupName };
		return func(this, static_cast<std::int32_t>(a_sex), a_index);
	}

	void* TESRace::FindShapeDescriptorByName(SEX a_sex, const char* a_name)
	{
		using func_t = void* (*)(TESRace*, std::int32_t, const char*);
		static REL::Relocation<func_t> func{ ID::TESRace::FindShapeDescriptorByName };
		return func(this, static_cast<std::int32_t>(a_sex), a_name);
	}

	void* TESRace::GetBoneRegionDescriptor(SEX a_sex, std::uint32_t a_id)
	{
		using func_t = void* (*)(TESRace*, std::int32_t, std::uint32_t);
		static REL::Relocation<func_t> func{ ID::TESRace::GetBoneRegionDescriptor };
		return func(this, static_cast<std::int32_t>(a_sex), a_id);
	}
}
