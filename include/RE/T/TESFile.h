#pragma once

namespace RE
{
	enum class TESFileFlags : std::uint32_t
	{
		kMaster = 1 << 0,
		kLocalized = 1 << 7,
		kSmall = 1 << 8,
		kUpdate = 1 << 9,
		kMedium = 1 << 10,
		kBlueprint = 1 << 11
	};

	struct TESFileIndex
	{
		std::uint8_t  fullIndex;    // 00
		std::uint8_t  mediumIndex;  // 01
		std::uint16_t smallIndex;   // 02
	};
	static_assert(sizeof(TESFileIndex) == 0x4);

	class TESFile
	{
	public:
		SF_RTTI_VTABLE(TESFile);

		std::uint8_t pad0[0x38];     // 000
		char         fileName[260];  // 038
		std::uint8_t pad13C[0x7B];   // 13C
		std::uint8_t compileIndex;   // 1B7
		TESFileFlags fileFlags;      // 1B8
		std::uint8_t pad1BC[0x4];    // 1BC
		std::uint8_t pad1C0[0x58];   // 1C0
		TESFileIndex fileIndex;      // 218
	};
	static_assert(offsetof(TESFile, fileFlags) == 0x1B8);
	static_assert(offsetof(TESFile, fileIndex) == 0x218);
}
