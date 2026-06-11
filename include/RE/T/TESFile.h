#pragma once

namespace RE
{
	class TESFile
	{
	public:
		SF_RTTI_VTABLE(TESFile);

		std::uint8_t pad0[0x38];     //0
		char         fileName[260];  //38
		std::uint8_t pad13C[0x7B];   //13C
		// Live-proven 2026-06-10 (1.16.242): position in the owning
		// TESFileCollection array for full masters (0, 1, 2, ...);
		// the tier sentinel 0xFE for smallFiles and 0xFD for mediumFiles.
		std::uint8_t compileIndex;   //1B7
	};
}
