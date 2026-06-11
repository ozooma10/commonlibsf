#pragma once

#include "RE/N/NiPoint.h"

namespace RE
{
	class TESObjectREFR;

	// The engine's reference transform-command service. The one actor
	// world-transform primitive that is safe to call from any thread: on the
	// game thread it applies immediately, off-thread it enqueues a refr-AddRef'd
	// command. Reached through a global service pointer rather than RTTI, so the
	// class name is behaviour-derived (Starfield 1.16.242.0), not RTTI-confirmed.
	class TransformService
	{
	public:
		enum class Opcode : std::uint32_t
		{
			kSetPosition = 0x1007,
			kSetAngle = 0x1009,    // degrees
			kMoveToMarker = 0x109E,
			kSetScale = 0x113C,
		};

		[[nodiscard]] static TransformService* GetSingleton()
		{
			static REL::Relocation<TransformService**> singleton{ ID::TransformService::Singleton };
			return *singleton;
		}

		// a_axis is 'X' / 'Y' / 'Z'. a_value is meters for position/scale,
		// degrees for angle.
		void Request(Opcode a_opcode, TESObjectREFR* a_refr, char a_axis, double a_value)
		{
			using func_t = void (*)(TransformService*, Opcode, TESObjectREFR*, char, double);
			static REL::Relocation<func_t> func{ ID::TransformService::Request };
			func(this, a_opcode, a_refr, a_axis, a_value);
		}

		void SetPosition(TESObjectREFR* a_refr, const NiPoint3& a_pos)
		{
			Request(Opcode::kSetPosition, a_refr, 'X', a_pos.x);
			Request(Opcode::kSetPosition, a_refr, 'Y', a_pos.y);
			Request(Opcode::kSetPosition, a_refr, 'Z', a_pos.z);
		}

		// heading about Z, given in radians (the service wants degrees)
		void SetHeadingZ(TESObjectREFR* a_refr, float a_radians)
		{
			constexpr double kRadToDeg = 57.29577951308232;  // 180/pi
			Request(Opcode::kSetAngle, a_refr, 'Z', static_cast<double>(a_radians) * kRadToDeg);
		}
	};
}
