#include "RE/B/BSInputDevice.h"

namespace RE
{
	BSInputDevice::~BSInputDevice()
	{
		// The engine entry is a scalar deleting destructor. Flags=0 performs
		// native cleanup without freeing storage owned by the caller.
		using func_t = void (*)(BSInputDevice*, std::uint32_t);
		static REL::Relocation<func_t> func{ ID::BSInputDevice::dtor };
		func(this, 0);
	}

	bool BSInputDevice::GetKeyNameFromCode(std::uint32_t a_keyCode, BSFixedStringCS& a_name) const
	{
		using func_t = decltype(&BSInputDevice::GetKeyNameFromCode);
		static REL::Relocation<func_t> func{ ID::BSInputDevice::GetKeyNameFromCode };
		return func(this, a_keyCode, a_name);
	}

	std::uint32_t BSInputDevice::GetKeyCodeFromName(const char* a_name) const
	{
		using func_t = decltype(&BSInputDevice::GetKeyCodeFromName);
		static REL::Relocation<func_t> func{ ID::BSInputDevice::GetKeyCodeFromName };
		return func(this, a_name);
	}

	bool BSInputDevice::GetMappedKeyCode(std::uint32_t a_keyCode, std::uint32_t& a_mappedKeyCode) const
	{
		using func_t = decltype(&BSInputDevice::GetMappedKeyCode);
		static REL::Relocation<func_t> func{ ID::BSInputDevice::GetMappedKeyCode };
		return func(this, a_keyCode, a_mappedKeyCode);
	}
}
