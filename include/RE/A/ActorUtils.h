#pragma once

namespace RE
{
	class Actor;
	class BGSKeyword;
}

namespace RE::ActorUtils
{
	inline bool ChangeAnimArchetype(Actor* a_actor, BGSKeyword* a_keyword)
	{
		using func_t = decltype(&ActorUtils::ChangeAnimArchetype);
		static REL::Relocation<func_t> func{ ID::ActorUtils::ChangeAnimArchetype };
		return func(a_actor, a_keyword);
	}

	// Plays an idle on the actor by driving the actor's AIProcess idle-setter directly.
	inline bool PlayIdle(Actor* a_actor, TESIdleForm* a_idle)
	{
		if (!a_actor || !a_idle) {
			return false;
		}
		AIProcess* process = a_actor->currentProcess;
		if (!process) {
			return false;
		}
		return process->PlayIdle(a_actor, a_idle);
	}
}
