#include "RE/B/BSInputEventUserStandalone.h"

namespace RE
{
	BSInputEventUserStandalone::BSInputEventUserStandalone()
	{
		using func_t = BSInputEventUser* (*)(BSInputEventUser*);
		static REL::Relocation<func_t> construct{ ID::BSInputEventUser::ctor };
		construct(this);
		// The concrete derived constructor installs its own vtable afterward.
	}

	BSInputEventUserStandalone::~BSInputEventUserStandalone()
	{
		using func_t = void* (*)(BSInputEventUser*, std::uint32_t);
		static REL::Relocation<func_t> destroy{ ID::BSInputEventUser::ScalarDeletingDestructor };
		destroy(this, 0); // Release held actions without freeing the caller's object storage.
	}
}
