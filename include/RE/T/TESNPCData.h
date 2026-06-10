#pragma once

#include "RE/B/BSTSingleton.h"
#include "RE/I/IDataModel.h"

namespace RE
{
	class MenuActor;
}

namespace RE::TESNPCData
{
	// EXE RTTI (2026-06-09, SingletonSdmAuditProbe): IDataModel at mdisp 0x0
	// (0x10), BSTSingletonSDM at mdisp 0x10 (0x10, polymorphic) — members
	// would start at 0x20. The fixed SDM template produces this naturally.
	class ChargenDataModel :
		public IDataModel,                        // 00
		public BSTSingletonSDM<ChargenDataModel>  // 10 — exe RTTI mdisp
	{
	public:
		SF_RTTI_VTABLE(TESNPCData__ChargenDataModel);

		virtual ~ChargenDataModel() override;

		[[nodiscard]] static ChargenDataModel* GetSingleton()
		{
			static REL::Relocation<ChargenDataModel**> singleton{ ID::TESNPCData::ChargenDataModel::Singleton };
			return *singleton;
		}

		void Update(MenuActor* a_menuActor)
		{
			using func_t = decltype(&ChargenDataModel::Update);
			static REL::Relocation<func_t> func{ ID::TESNPCData::ChargenDataModel::Update };
			return func(this, a_menuActor);
		}
	};
}
