#pragma once

#include "RE/B/BSInputEventUser.h"

namespace RE
{
	// C++ lifetime adapter; not an engine RTTI type.
	class BSInputEventUserStandalone :
		public BSInputEventUser  // 00
	{
	protected:
		BSInputEventUserStandalone();
		virtual ~BSInputEventUserStandalone();  // 00

		BSInputEventUserStandalone(const BSInputEventUserStandalone&) = delete;
		BSInputEventUserStandalone& operator=(const BSInputEventUserStandalone&) = delete;
	};
	static_assert(sizeof(BSInputEventUserStandalone) == 0x40);
}
