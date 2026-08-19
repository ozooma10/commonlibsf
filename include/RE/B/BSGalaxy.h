#pragma once

#include "RE/B/BSCoreTypes.h"

#include <cstddef>
#include <cstdint>
#include <limits>
#include <optional>

namespace RE::BSGalaxy
{
	struct SatelliteCSVData
	{
		std::uint32_t numericSystemID;  // 00
		std::uint32_t parentOrdinal;    // 04
		std::uint32_t planetOrdinal;    // 08
	};
	static_assert(offsetof(SatelliteCSVData, numericSystemID) == 0x00);
	static_assert(offsetof(SatelliteCSVData, parentOrdinal) == 0x04);
	static_assert(offsetof(SatelliteCSVData, planetOrdinal) == 0x08);
	static_assert(sizeof(SatelliteCSVData) == 0x0C);

	// These native wrappers acquire the ComponentDB guard and copy the result
	// into caller-owned storage. Numeric system ID zero is valid (Sol).
	[[nodiscard]] inline std::optional<TESFormID> GetSystemFormID(TESFormID a_formID)
	{
		using func_t = TESFormID* (*)(TESFormID*, TESFormID);
		static REL::Relocation<func_t> func{ ID::BSGalaxy::GetSystemFormID };

		TESFormID result = (std::numeric_limits<TESFormID>::max)();
		if (func(&result, a_formID) != &result || result == 0 || result == (std::numeric_limits<TESFormID>::max)()) {
			return std::nullopt;
		}
		return result;
	}

	[[nodiscard]] inline std::optional<std::uint32_t> GetNumericSystemID(TESFormID a_formID)
	{
		using func_t = std::uint32_t* (*)(std::uint32_t*, TESFormID);
		static REL::Relocation<func_t> func{ ID::BSGalaxy::GetNumericSystemID };

		std::uint32_t result = (std::numeric_limits<std::uint32_t>::max)();
		if (func(&result, a_formID) != &result || result == (std::numeric_limits<std::uint32_t>::max)()) {
			return std::nullopt;
		}
		return result;
	}

	[[nodiscard]] inline std::optional<std::uint32_t> GetPlanetOrdinal(TESFormID a_formID)
	{
		using func_t = std::uint32_t* (*)(std::uint32_t*, TESFormID);
		static REL::Relocation<func_t> func{ ID::BSGalaxy::GetPlanetOrdinal };

		std::uint32_t result = (std::numeric_limits<std::uint32_t>::max)();
		if (func(&result, a_formID) != &result || result == (std::numeric_limits<std::uint32_t>::max)()) {
			return std::nullopt;
		}
		return result;
	}

	[[nodiscard]] inline std::optional<TESFormID> GetBodyFormID(std::uint32_t a_numericSystemID, std::uint32_t a_planetOrdinal)
	{
		using func_t = TESFormID* (*)(TESFormID*, std::uint32_t, std::uint32_t);
		static REL::Relocation<func_t> func{ ID::BSGalaxy::GetBodyFormID };

		TESFormID result = (std::numeric_limits<TESFormID>::max)();
		if (func(&result, a_numericSystemID, a_planetOrdinal) != &result || result == 0 || result == (std::numeric_limits<TESFormID>::max)()) {
			return std::nullopt;
		}
		return result;
	}
}
