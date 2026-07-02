#pragma once

namespace RE
{
	class BGSSaveLoadGame
	{
	public:
		void SaveGame(void* a_unk1, void* a_unk2, const char* a_name)
		{
			using func_t = decltype(&BGSSaveLoadGame::SaveGame);
			static REL::Relocation<func_t> func{ ID::BGSSaveLoadGame::SaveGame };
			return func(this, a_unk1, a_unk2, a_name);
		}

		bool LoadGame(void* a_reader, bool a_unk1, bool a_unk2)
		{
			using func_t = decltype(&BGSSaveLoadGame::LoadGame);
			static REL::Relocation<func_t> func{ ID::BGSSaveLoadGame::LoadGame };
			return func(this, a_reader, a_unk1, a_unk2);
		}
	};
}
