#pragma once

namespace RE
{
	class BGSSaveLoadGame
	{
	public:
		// SaveGame(ctx, writer, name): a_name is a plain null-terminated char*
		// carrying the ".sfs" extension (e.g. "Quicksave0_...sfs"). a_unk1 is a
		// serialization context, a_unk2 a SaveFileWriter*. AddrLib 98376.
		// Runtime-verified 1.16.244 (2026-06-13).
		void SaveGame(void* a_unk1, void* a_unk2, const char* a_name)
		{
			using func_t = decltype(&BGSSaveLoadGame::SaveGame);
			static REL::Relocation<func_t> func{ ID::BGSSaveLoadGame::SaveGame };
			return func(this, a_unk1, a_unk2, a_name);
		}

		// LoadGame(reader, unk1, unk2): the save name is NOT a direct argument -
		// a_reader is a SaveFileReader* whose inline buffer at +0x000 holds the
		// extension-less name. a_unk1/a_unk2 are bools (low byte of r8/r9). The
		// previous `LoadGame(const char* name, void*, void*)` signature was a
		// wrong guess. AddrLib 98380. Runtime-verified 1.16.244 (2026-06-13).
		bool LoadGame(void* a_reader, bool a_unk1, bool a_unk2)
		{
			using func_t = decltype(&BGSSaveLoadGame::LoadGame);
			static REL::Relocation<func_t> func{ ID::BGSSaveLoadGame::LoadGame };
			return func(this, a_reader, a_unk1, a_unk2);
		}
	};
}
