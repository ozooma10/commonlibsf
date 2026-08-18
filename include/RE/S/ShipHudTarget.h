#pragma once

#include "RE/B/BSCoreTypes.h"

namespace RE::ShipHudTarget
{
	inline void Set(TESFormID a_target)
	{
		using func_t = void (*)(TESFormID);
		static REL::Relocation<func_t> func{ ID::ShipHudTarget::Set };
		func(a_target);
	}

	[[nodiscard]] inline TESFormID GetCurrent()
	{
		static REL::Relocation<TESFormID*> current{ ID::ShipHudTarget::Current };
		return *current;
	}

	inline void Clear()
	{
		Set(0);
	}
}
