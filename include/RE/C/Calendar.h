#pragma once

#include "RE/B/BSTSingleton.h"
#include "RE/T/TESGlobal.h"

namespace RE
{
	// EXE RTTI + ctor disasm (2026-06-09): the BaseClassArray (exe_query
	// `vtable Calendar --bases`) proves BSTSingletonSDM at mdisp 0x0 as the
	// only base (1-slot vtable 0x144CB5340), and the magic-static initializer
	// behind the accessor 0x14059b7e0 PROVES the member offsets: it stores the
	// looked-up TESGlobal* sequence at instance+0x10/+0x18/+0x20/... (instance
	// 0x145C1CA28; the Calendar* global it publishes is RVA 0x5FE4D28 =
	// ID 937673).
	class Calendar :
		public BSTSingletonSDM<Calendar>  // 00 — exe RTTI mdisp
	{
	public:
		[[nodiscard]] static auto GetSingleton()
		{
			static REL::Relocation<Calendar**> singleton{ ID::Calendar::Singleton };
			return *singleton;
		}

		std::uint32_t GetDay() const noexcept
		{
			return gameDay ? static_cast<std::uint32_t>(gameDay->value) : 17u;
		}

		std::uint32_t GetDaysPassed() const noexcept
		{
			return gameDaysPassed ? static_cast<std::uint32_t>(gameDaysPassed->value) : 0u;
		}

		float GetDaysPassedExact() const noexcept
		{
			return gameDaysPassed ? gameDaysPassed->value : 0.0f;
		}

		float GetHour() const noexcept
		{
			return gameHour ? gameHour->value : 12.0f;
		}

		float GetHourFromDaysPassed(float a_daysPassed) const noexcept
		{
			return std::fmodf(a_daysPassed, 1.0f) * 24.0f;
		}

		float GetHoursPassedExact() const noexcept
		{
			return gameDaysPassed ? gameDaysPassed->value * 24.0f : 0.0f;
		}

		std::uint16_t GetMonth() const noexcept
		{
			return gameMonth ? static_cast<std::uint16_t>(gameMonth->value) : 8u;
		}

		float GetTimeScale() const
		{
			return timeScale->value;
		}

		std::uint32_t GetYear() const noexcept
		{
			return gameYear ? static_cast<std::uint32_t>(gameYear->value) : 77u;
		}

		// members — PROVEN by the initializer disasm (see class comment)
		TESGlobal*    gameYear;         // 10
		TESGlobal*    gameMonth;        // 18
		TESGlobal*    gameDay;          // 20
		TESGlobal*    gameHour;         // 28
		TESGlobal*    gameDaysPassed;   // 30
		TESGlobal*    timeScale;        // 38
		std::uint32_t midnightsPassed;  // 40
		float         rawDaysPassed;    // 44
	};
	static_assert(sizeof(Calendar) == 0x48);
}
