#pragma once

#include "RE/B/BSExtraData.h"

namespace RE
{
	class TBO_InstanceData;

	class ExtraInstanceData : public BSExtraData
	{
	public:
		SF_RTTI_VTABLE(ExtraInstanceData);
		SF_EXTRADATATYPE(InstanceData);

		~ExtraInstanceData() override;  // 00

		[[nodiscard]] TBO_InstanceData* GetInstanceData() const noexcept { return instanceData; }

		// members
		std::uint64_t     unk18;        // 18
		TBO_InstanceData* instanceData;  // 20
	};
	static_assert(sizeof(ExtraInstanceData) == 0x28);
}
