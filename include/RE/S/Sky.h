#pragma once

#include "RE/B/BSTSingleton.h"

namespace RE
{
	class ILightEmittanceManager
	{
	public:
		SF_RTTI(ILightEmittanceManager);

		virtual ~ILightEmittanceManager() = default;  // 00 - remaining virtuals unmapped
	};

	class IExternalEmittanceManager :
		public ILightEmittanceManager
	{
	public:
		SF_RTTI(IExternalEmittanceManager);
	};

	class __declspec(novtable) Sky :
		public BSTSingletonSDM<Sky>,      // 00
		public IExternalEmittanceManager  // 10
	{
	public:
		SF_RTTI_VTABLE(Sky);

		virtual ~Sky();  // 00

		// members
		std::byte pad[0xEC4 - 0x18];  // 18
		float     windSpeed;          // EC4
	};
	static_assert(offsetof(Sky, windSpeed) == 0xEC4);
}
