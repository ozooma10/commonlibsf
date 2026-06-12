#pragma once

#include "RE/B/BSCoreTypes.h"
#include "RE/I/INIPrefSettingCollection.h"
#include "RE/I/INISettingCollection.h"
#include "RE/N/NiSmartPointer.h"
#include "RE/S/Setting.h"
#include "RE/T/TESObjectREFR.h"

namespace RE
{
	inline void DebugNotification(const char* a_notification, const char* a_soundToPlay = nullptr, bool a_cancelIfAlreadyQueued = true, bool a_arg4 = false)
	{
		using func_t = decltype(&DebugNotification);
		static REL::Relocation<func_t> func{ ID::Misc::DebugNotification };
		return func(a_notification, a_soundToPlay, a_cancelIfAlreadyQueued, a_arg4);
	}

	// Engine impl behind the Papyrus native Game.FadeOutGame (same parameter
	// order as the Papyrus declaration). Thread-safe: it only enqueues a
	// FaderData kShow message to "FaderMenu" via UIMessageQueue; the Scaleform
	// work happens on the UI pump. abStayFaded holds the screen at black until
	// the next fade-in request; a fade-in (a_fadingOut = false) always
	// releases, even mid-fade. Engine arg order differs from Papyrus:
	// (fadingOut, blackFade, fadeDuration, stayFaded, secsBeforeFade,
	// doneCallback, setGlobalFadedFlag, flags).
	inline void FadeOutGame(bool a_fadingOut, bool a_blackFade, float a_secsBeforeFade, float a_fadeDuration, bool a_stayFaded)
	{
		using func_t = void(bool, bool, float, bool, float, void*, bool, std::uint32_t);
		static REL::Relocation<func_t> func{ ID::Misc::FadeOutGame };
		return func(a_fadingOut, a_blackFade, a_fadeDuration, a_stayFaded, a_secsBeforeFade, nullptr, true, 1);
	}

	inline Setting* GetINISetting(const std::string_view a_name)
	{
		const auto iniPrefs = INIPrefSettingCollection::GetSingleton();
		auto       setting = iniPrefs ? iniPrefs->GetSetting(a_name) : nullptr;
		if (!setting) {
			const auto ini = INISettingCollection::GetSingleton();
			setting = ini ? ini->GetSetting(a_name) : nullptr;
		}

		return setting;
	}

	inline void PlayMenuSound(const char* a_editorID, const char* a_arg3 = nullptr, float a_arg4 = 0.0f)
	{
		std::uint32_t arg1 = 0;

		using func_t = void(std::uint32_t&, const char*, const char*, float);
		static REL::Relocation<func_t> func{ ID::Misc::PlayMenuSound };
		return func(arg1, a_editorID, a_arg3, a_arg4);
	}
}
