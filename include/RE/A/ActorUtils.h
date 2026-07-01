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

	// Plays a_idle on a_actor via the pure-native AIProcess path (no Papyrus VM). Replicates the
	// engine's outlined Actor::PlayIdle: proc = actor->currentProcess (Actor+0x228); then the 7-arg
	// idle-setter (ID 102136) with flags=0x36, byte5=true, a6=null, and the global from PlayIdleGlobal
	// (ID 35720). Returns the setter's bool (may be false-on-success; judge by the observed motion).
	// RE'd + in-game proven 2026-06-23 (osf-re engine.animation_graphs).
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
