#pragma once
#include "RE/B/BSTSmartPointer.h"

#include "RE/B/BSFixedString.h"
#include "RE/B/BSTSmartPointer.h"

namespace RE
{
	class BSAnimationGraphManager;

	class __declspec(novtable) IAnimationGraphManagerHolder
	{
	public:
		SF_RTTI(IAnimationGraphManagerHolder);

		virtual ~IAnimationGraphManagerHolder();  // 00

		// add
		// NB (OSF RE, 1.16.244 — disasm + IN-GAME proven 2026-06-16: a `NotifyAnimationGraph(event)` probe
		// visibly drove the player's animation on command): the engine impl is __fastcall
		// bool(this, const BSFixedString& a_eventName, bool a_flag). The `bool` (r8b) is a REAL third arg the
		// impl forwards to the inner dispatcher — it is now IN this signature (the prior 1-param decl omitted
		// it, leaving r8b undefined for direct callers). Concrete REFR/Actor impl = AddrLib ID 73218
		// (0x140f09f60 on .244); PlayerCharacter does NOT override it (Actor holder vtable 0x144cb8d18 slot 1
		// == PlayerCharacter holder vtable 0x144cbc2e8 slot 1). Impl reads the eventName BSFixedString chars at
		// _data+0x18, calls GetAnimationGraphManagerImpl (slot 3), then dispatches (mgr, eventName, flag) ->
		// inner ID 123568 (0x1422af480).
		virtual bool          NotifyAnimationGraphImpl(const BSFixedString& a_eventName, bool a_flag);                                                            // 01
		virtual void          Unk_02();                                                                                                                           // 02
		virtual bool          GetAnimationGraphManagerImpl(BSTSmartPointer<BSAnimationGraphManager>& a_animGraphMgr);                                             // 03
		virtual void          Unk_04();                                                                                                                           // 04
		virtual void          Unk_05();                                                                                                                           // 05
		virtual void          Unk_06();                                                                                                                           // 06
		virtual bool          UnkFlag();                                                                                                                          // 07 - [Actor/Player: checks that flag 128 is not set on *(this + 0x448)] [Ref: Always False]
		virtual void          Unk_08();                                                                                                                           // 08
		virtual void          Unk_09();                                                                                                                           // 09
		virtual void          Unk_0A();                                                                                                                           // 0A
		virtual void          Unk_0B();                                                                                                                           // 0B
		virtual void          Unk_0C();                                                                                                                           // 0C
		virtual void          Unk_0D();                                                                                                                           // 0D
		virtual std::uint32_t GetGraphVariableCacheSize() const { return 0; }                                                                                     // 0E
		virtual void          Unk_0F();                                                                                                                           // 0F
		virtual void          Unk_10();                                                                                                                           // 10
		virtual bool          ShouldUpdateAnimation();                                                                                                            // 11
		virtual bool          GetGraphVariableImplFloat(const BSFixedString& a_variable, float& a_out) const;                                                     // 12
		virtual bool          GetGraphVariableImplInt(const BSFixedString& a_variable, std::int32_t& a_out) const;                                                // 13
		virtual bool          GetGraphVariableImplBool(const BSFixedString& a_variable, bool& a_out) const;                                                       // 14
		virtual void          PreUpdateAnimationGraphManager([[maybe_unused]] const BSTSmartPointer<BSAnimationGraphManager>& a_animGraphMgr) const { return; }   // 15
		virtual void          PostUpdateAnimationGraphManager([[maybe_unused]] const BSTSmartPointer<BSAnimationGraphManager>& a_animGraphMgr) const { return; }  // 16
		virtual void          PreLoadAnimationGraphManager([[maybe_unused]] const BSTSmartPointer<BSAnimationGraphManager>& a_animGraphMgr) { return; }           // 17
		virtual void          PostLoadAnimationGraphManager([[maybe_unused]] const BSTSmartPointer<BSAnimationGraphManager>& a_animGraphMgr) { return; }          // 18
	};
	static_assert(sizeof(IAnimationGraphManagerHolder) == 0x8);
}
