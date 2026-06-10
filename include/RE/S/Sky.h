#pragma once

#include "RE/B/BSTSingleton.h"

namespace RE
{
	class IExternalEmittanceManager
	{
	public:
		SF_RTTI(IExternalEmittanceManager);
	};

	class ILightEmittanceManager
	{
	public:
		SF_RTTI(ILightEmittanceManager);
	};

	// CAUTION (2026-06-09): NOT audited — CommonLibSF has no Sky::GetSingleton,
	// so the RTTI audit could not run. SDM is now the proven polymorphic 0x10
	// shape (vptr lives in it); the leading pad was resized to PRESERVE the
	// windSpeed@0xEC4 anchor. Base order/positions are UNPROVEN for this class.
	class __declspec(novtable) Sky :
		public BSTSingletonSDM<Sky>,      // 00
		public IExternalEmittanceManager,
		public ILightEmittanceManager
	{
	public:
		SF_RTTI_VTABLE(Sky);

		virtual ~Sky();  // 00

		// members
		std::byte pad[0xEB2];  // 12?
		float     windSpeed;   // EC4
	};
	static_assert(offsetof(Sky, windSpeed) == 0xEC4);
}
