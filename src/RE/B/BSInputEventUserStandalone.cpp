#include "RE/B/BSInputEventUserStandalone.h"

namespace RE
{
	BSInputEventUserStandalone::BSInputEventUserStandalone()
	{
		using func_t = BSInputEventUser* (*)(BSInputEventUser*);
		static REL::Relocation<func_t> construct{ ID::BSInputEventUser::ctor };
		construct(this);
	}

	BSInputEventUserStandalone::~BSInputEventUserStandalone()
	{
		using func_t = void* (*)(BSInputEventUser*, std::uint32_t);
		static REL::Relocation<func_t> destroy{ ID::BSInputEventUser::ScalarDeletingDestructor };
		destroy(this, 0);  // Run the scalar deleting destructor without freeing storage.
	}
}
