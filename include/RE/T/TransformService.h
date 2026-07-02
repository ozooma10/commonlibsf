#pragma once

#include "RE/N/NiPoint.h"

namespace RE
{
	class TESObjectREFR;

	class TransformService
	{
	public:
		enum class Opcode : std::uint32_t
		{
			kSetPosition = 0x1007,
			kSetAngle = 0x1009,
			kMoveToMarker = 0x109E,
			kSetScale = 0x113C,
		};

		[[nodiscard]] static TransformService* GetSingleton()
		{
			static REL::Relocation<TransformService**> singleton{ ID::TransformService::Singleton };
			return *singleton;
		}

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

		void SetHeadingZ(TESObjectREFR* a_refr, float a_radians)
		{
			constexpr double kRadToDeg = 57.29577951308232;
			Request(Opcode::kSetAngle, a_refr, 'Z', static_cast<double>(a_radians) * kRadToDeg);
		}
	};
}
