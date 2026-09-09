#pragma once

#include "RE/I/IMenu.h"
#include "RE/S/ScaleformGFxASMovieRootBase.h"

namespace RE
{
	class HUDModeType
	{
	public:
		HUDModeType(const char* a_str) :
			str(a_str)
		{}

		// members
		BSFixedString str;  // 00
	};

	class GameMenuBase :
		public IMenu  // 00
	{
	public:
		static constexpr const char* CODE_OBJ_NAME{ "BGSCodeObj" };
		static constexpr const char* ON_CODE_OBJ_CREATED_FUNC{ "onCodeObjCreate" };

		SF_RTTI_VTABLE(GameMenuBase);

		GameMenuBase()
		{
			// 1.16.244 native factories inline GameMenuBase initialization after
			// the complete IMenu ctor (e.g. 0x1415E1F81 and 0x141666F0B).
			// ID 130577 is a movie-manager removal routine, not a constructor.
			using func_t = IMenu* (*)(IMenu*);
			static REL::Relocation<func_t> func{ ID::IMenu::ctor };
			func(this);
			unk128 = 0;
			unk130 = 0;  // GameMenuBase pause-request state at +0x130 starts inactive.
			// The concrete derived ctor installs its own vtables after this returns.
		}

		virtual ~GameMenuBase() = default;  // 00

		// override
		virtual void OnAddedToMenuStack() override
		{
			using func_t = decltype(&GameMenuBase::OnAddedToMenuStack);
			static REL::Relocation<func_t> func{ ID::GameMenuBase::OnAddedToMenuStack };
			return func(this);
		}

		virtual void OnRemovedFromMenuStack() override
		{
			using func_t = decltype(&GameMenuBase::OnRemovedFromMenuStack);
			static REL::Relocation<func_t> func{ ID::GameMenuBase::OnRemovedFromMenuStack };
			return func(this);
		}

		// add
		virtual bool Unk1B() { return true; }  //1B

		// Do not set this to true unless your menu SWF has Beth's AS3 event backend.
		// Otherwise, it will cause a CTD.
		virtual bool UseEventDispatcher()
		{
			return false;
		}

		// custom override
		virtual bool LoadMovie(bool a_addEventDispatcher, bool a_arg2) override
		{
			a_addEventDispatcher = UseEventDispatcher();
			bool result = IMenu::LoadMovie(a_addEventDispatcher, a_arg2);
			if (result && !a_addEventDispatcher && uiMovie && uiMovie->asMovieRoot) {
				uiMovie->asMovieRoot->GetVariable(&menuObj, GetRootPath());
				if (menuObj.IsObject() && menuObj.HasMember(CODE_OBJ_NAME)) {
					MapCodeObjectFunctions();
					menuObj.Invoke(ON_CODE_OBJ_CREATED_FUNC);
				}
			}
			return result;
		}

		void RegisterNativeFunction(const char* a_name, std::uint64_t a_idx)
		{
			if (!uiMovie) {
				return;
			}

			RE::Scaleform::GFx::Value codeObjVal;
			menuObj.GetMember(CODE_OBJ_NAME, &codeObjVal);
			if (!codeObjVal.IsObject()) {
				return;
			}

			RE::Scaleform::GFx::Value funcVal;
			uiMovie->asMovieRoot->CreateFunction(&funcVal, this, reinterpret_cast<void*>(a_idx));
			codeObjVal.SetMember(a_name, funcVal);
		}
	};
}
