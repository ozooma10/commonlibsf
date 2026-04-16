#pragma once

#include "RE/B/BSExtraData.h"

namespace RE
{
	class ExtraKeywords : public BSExtraData
	{
	public:
		SF_RTTI_VTABLE(ExtraKeywords);
		SF_EXTRADATATYPE(Keywords);

		~ExtraKeywords() override;  // 00

		// Partial type: this extra is confirmed in the AddKeyword path, but its
		// payload layout is still unresolved.
	};
}
