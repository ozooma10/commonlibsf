#pragma once

#include "RE/B/BSInputEventUser.h"

namespace RE
{
	// Opt-in C++ adapter for standalone native handlers, not an engine RTTI type.
	// IMenu already manages this state through its complete native constructor
	// and destructor, so the ordinary BSInputEventUser base remains unchanged.
	class BSInputEventUserStandalone : public BSInputEventUser
	{
	protected:
		BSInputEventUserStandalone();
		~BSInputEventUserStandalone() override;

		BSInputEventUserStandalone(const BSInputEventUserStandalone&) = delete;
		BSInputEventUserStandalone& operator=(const BSInputEventUserStandalone&) = delete;
	};
	static_assert(sizeof(BSInputEventUserStandalone) == 0x40);
}
