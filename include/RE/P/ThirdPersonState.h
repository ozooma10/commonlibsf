#pragma once

#include <cstddef>

namespace RE
{
	//
	//
	//
	class ThirdPersonState
	{
	public:
		std::byte unk00[0x224];          // 000
		float     targetZoomOffset;      // 224
		float     currentZoomOffset;     // 228
		float     trackDistTarget;       // 22C
		float     trackDistCurrent;      // 230
		std::byte unk234[0x23C - 0x234]; // 234
		float     savedZoomOffset;       // 23C
		std::byte unk240[0x28A - 0x240]; // 240
		bool      zoomingInto1st;        // 28A
	};

	static_assert(offsetof(ThirdPersonState, targetZoomOffset) == 0x224);
	static_assert(offsetof(ThirdPersonState, currentZoomOffset) == 0x228);
	static_assert(offsetof(ThirdPersonState, trackDistTarget) == 0x22C);
	static_assert(offsetof(ThirdPersonState, trackDistCurrent) == 0x230);
	static_assert(offsetof(ThirdPersonState, savedZoomOffset) == 0x23C);
	static_assert(offsetof(ThirdPersonState, zoomingInto1st) == 0x28A);
}
