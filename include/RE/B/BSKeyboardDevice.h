#pragma once

#include "RE/B/BSInputDevice.h"

namespace RE
{
	class BSKeyboardDevice :
		public BSInputDevice  // 00
	{
	public:
		SF_RTTI_VTABLE(BSKeyboardDevice);

		~BSKeyboardDevice() override = default;  // 00
	};
	static_assert(sizeof(BSKeyboardDevice) == 0x80);
}
