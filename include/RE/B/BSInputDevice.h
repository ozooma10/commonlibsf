#pragma once

#include "RE/B/BSFixedString.h"

namespace RE
{
	class BSInputDevice
	{
	public:
		SF_RTTI_VTABLE(BSInputDevice);

		virtual ~BSInputDevice();  // 00

		// add
		virtual void Initialize() = 0;                // 01
		virtual void Process(float a_deltaTime) = 0;  // 02
		virtual void Release() = 0;                   // 03

		// Native display name, including keyboard-layout substitutions. A missing
		// key returns false and leaves a_name unchanged. Not an INI round-trip name.
		virtual bool GetKeyNameFromCode(std::uint32_t a_keyCode, BSFixedStringCS& a_name) const;  // 04

		// Case-insensitive native name lookup. Requires a non-null, NUL-terminated
		// name and an initialized device. Returns 0xFFFFFFFF when not found.
		// Keyboard results are Win32 virtual-key codes, not DirectInput scan codes.
		virtual std::uint32_t GetKeyCodeFromName(const char* a_name) const;  // 05

		// Secondary code from the device table (e.g. keyboard L Ctrl 0xA2 -> 0x11).
		// A missing key returns false and leaves a_mappedKeyCode unchanged. An entry
		// with no secondary code succeeds with zero; this is not a scan-code converter.
		virtual bool GetMappedKeyCode(std::uint32_t a_keyCode, std::uint32_t& a_mappedKeyCode) const;  // 06

		// The base implementation reports available. Gamepad devices override this
		// to check connection state and the assigned device ID.
		virtual bool IsEnabled() const { return true; }  // 07
		virtual void Reset() = 0;                       // 08

		// members
		std::uint8_t unk08[0x78];  // 08
	};
	static_assert(sizeof(BSInputDevice) == 0x80);
}
