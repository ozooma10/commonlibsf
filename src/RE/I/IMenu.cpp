#include "RE/I/IMenu.h"

#include <array>
#include <cstring>

namespace RE
{
	void IMenu::AddInputContext(InputContextID a_context)
	{
		// Match PauseMenu's append path, including the native allocator and
		// the dirty byte. IMenu's native destructor releases this buffer.
		if (inputContexts.end == inputContexts.capacityEnd) {
			using func_t = InputContextID* (*)(InputContextStorage*, InputContextID*, const InputContextID*);
			static REL::Relocation<func_t> func{ ID::IMenu::ReallocateInputContexts };
			func(&inputContexts, inputContexts.end, &a_context);
		} else {
			*inputContexts.end++ = a_context;
		}
		inputContextsChanged = true;
	}

	bool IMenu::IsInputContextAppendSupported()
	{
		constexpr std::array<std::uint8_t, 13> prologue{ 0x48, 0x89, 0x5C, 0x24, 0x18, 0x48, 0x89, 0x6C, 0x24, 0x20, 0x57, 0x41, 0x56 };
		return std::memcmp(reinterpret_cast<const void*>(ID::IMenu::ReallocateInputContexts.address()), prologue.data(), prologue.size()) == 0;
	}
}
