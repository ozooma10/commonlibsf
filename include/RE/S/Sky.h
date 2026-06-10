#pragma once

#include "RE/B/BSTSingleton.h"

namespace RE
{
	// EXE RTTI (2026-06-09, exe_query `vtable Sky --bases`): Sky's
	// BaseClassArray proves IExternalEmittanceManager DERIVES
	// ILightEmittanceManager (same mdisp 0x10, contained), and the subobject
	// at Sky+0x10 is POLYMORPHIC with a >=4-slot vtable (0x144BE1978) — the
	// old model of two empty sibling interfaces was wrong. The exact virtual
	// list is unmapped; only the dtor is declared here as the vptr carrier.
	class ILightEmittanceManager
	{
	public:
		SF_RTTI(ILightEmittanceManager);

		virtual ~ILightEmittanceManager() = default;  // 00 — vtable has >=4 slots; remaining virtuals unmapped
	};

	class IExternalEmittanceManager :
		public ILightEmittanceManager
	{
	public:
		SF_RTTI(IExternalEmittanceManager);
	};

	// EXE RTTI (2026-06-09): BSTSingletonSDM at mdisp 0x0 (1-slot vtable
	// 0x144BE1968), IExternalEmittanceManager at mdisp 0x10 — members start at
	// 0x18; the pad preserves the windSpeed@0xEC4 anchor. No singleton
	// accessor is known in CommonLibSF yet.
	class __declspec(novtable) Sky :
		public BSTSingletonSDM<Sky>,      // 00 — exe RTTI mdisp
		public IExternalEmittanceManager  // 10 — exe RTTI mdisp
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
