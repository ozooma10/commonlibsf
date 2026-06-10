#pragma once

#include "RE/B/BSTEvent.h"
#include "RE/B/BSTSingleton.h"

namespace RE
{
	class Actor;
	class BGSEquipSlot;
	class BGSObjectInstance;
	class TESBoundObject;

	namespace ActorEquipManagerEvent
	{
		struct Event;
		struct SpellEvent;
	}

	// EXE RTTI (2026-06-09, SingletonSdmAuditProbe): ActorEquipManager's
	// BaseClassArray has only the class + the BSTSingletonSDM internals — the
	// two BSTEventSources are NOT bases. They are standalone complete objects
	// at +0x10 and +0x38 (their vtable COLs carry offset 0 with their own type
	// names), i.e. members. The old header modeled them as bases at 0x08/0x30.
	class ActorEquipManager :
		public BSTSingletonSDM<ActorEquipManager>  // 00 — exe RTTI mdisp 0x0, 0x10 bytes
	{
	public:
		[[nodiscard]] static ActorEquipManager* GetSingleton()
		{
			static REL::Relocation<ActorEquipManager**> singleton{ ID::ActorEquipManager::Singleton };
			return *singleton;
		}

		bool EquipObject(Actor* a_actor, const BGSObjectInstance& a_object, const BGSEquipSlot* a_slot, bool a_queueEquip, bool a_forceEquip, bool a_playSounds, bool a_applyNow, bool a_locked)
		{
			using func_t = decltype(&ActorEquipManager::EquipObject);
			static REL::Relocation<func_t> func{ ID::ActorEquipManager::EquipObject };
			return func(this, a_actor, a_object, a_slot, a_queueEquip, a_forceEquip, a_playSounds, a_applyNow, a_locked);
		}

		bool UnequipObject(Actor* a_actor, const BGSObjectInstance& a_object, const BGSEquipSlot* a_slot, bool a_queueUnequip, bool a_forceUnequip, bool a_playSounds, bool a_applyNow, const BGSEquipSlot* a_slotBeingReplaced)
		{
			using func_t = decltype(&ActorEquipManager::UnequipObject);
			static REL::Relocation<func_t> func{ ID::ActorEquipManager::UnequipObject };
			return func(this, a_actor, a_object, a_slot, a_queueUnequip, a_forceUnequip, a_playSounds, a_applyNow, a_slotBeingReplaced);
		}

		// members — runtime-proven positions (standalone event-source objects,
		// see class comment); anything past 0x60 is unmapped.
		BSTEventSource<ActorEquipManagerEvent::Event>      equipEventSource;  // 10
		BSTEventSource<ActorEquipManagerEvent::SpellEvent> spellEventSource;  // 38
	};
	static_assert(offsetof(ActorEquipManager, equipEventSource) == 0x10);
	static_assert(offsetof(ActorEquipManager, spellEventSource) == 0x38);
	static_assert(sizeof(ActorEquipManager) == 0x60);
}
