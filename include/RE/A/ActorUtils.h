#pragma once

namespace RE
{
	class Actor;
	class BGSKeyword;
	class TESIdleForm;
}

namespace RE::ActorUtils
{
	inline bool ChangeAnimArchetype(Actor* a_actor, BGSKeyword* a_keyword)
	{
		using func_t = decltype(&ActorUtils::ChangeAnimArchetype);
		static REL::Relocation<func_t> func{ ID::ActorUtils::ChangeAnimArchetype };
		return func(a_actor, a_keyword);
	}

	inline bool PlayIdle(Actor* a_actor, TESIdleForm* a_idle)
	{
		using global_t = void* (*)();
		using setter_t = bool (*)(void*, Actor*, std::uint32_t, TESIdleForm*, bool, void*, void*);
		static REL::Relocation<global_t> getGlobal{ ID::ActorUtils::PlayIdleGlobal };
		static REL::Relocation<setter_t> setter{ ID::ActorUtils::PlayIdleSetter };
		auto* const proc = *reinterpret_cast<void**>(reinterpret_cast<std::byte*>(a_actor) + 0x228);
		return setter(proc, a_actor, 0x36, a_idle, true, nullptr, getGlobal());
	}
}
