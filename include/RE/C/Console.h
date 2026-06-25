#pragma once

#include "RE/G/GameMenuBase.h"
#include "RE/IDs.h"

namespace RE
{
	class Console :
		public GameMenuBase  // 00
	{
	public:
		SF_RTTI_VTABLE(Console);
		SF_MENU_NAME("Console");

		// Enable/disable ALL combat AI globally - the effect of the `tcai` console
		// command. a_enabled = false makes every NPC break off combat (the engine's
		// target-acquisition loop is gated); true restores normal behavior. The gate is
		// a raw global with no AddrLib ID, so it is resolved off the ToggleCombatAI
		// handler's `mov byte [rip+disp], dil` write (which IS indexed), making it
		// relocate across game builds. (RE: osf-re gameplay.defeat_damage_hook; 1.16.244.)
		static void SetCombatAIProcessing(bool a_enabled)
		{
			if (auto* const gate = GetCombatAIGate()) {
				*gate = a_enabled ? std::uint8_t{ 0 } : std::uint8_t{ 1 };
			}
		}

		[[nodiscard]] static bool IsCombatAIProcessingEnabled()
		{
			const auto* const gate = GetCombatAIGate();
			return gate == nullptr || *gate == 0;
		}

	private:
		// Parse the ToggleCombatAI handler for its gate-write and cache the gate address.
		[[nodiscard]] static volatile std::uint8_t* GetCombatAIGate()
		{
			static volatile std::uint8_t* const cached = []() -> volatile std::uint8_t* {
				const auto        handler = ID::Console::ToggleCombatAI.address();
				const auto* const code = reinterpret_cast<const std::uint8_t*>(handler);
				for (std::size_t i = 0; i + 7 <= 0x60; ++i) {
					if (code[i] == 0x40 && code[i + 1] == 0x88 && code[i + 2] == 0x3D) {  // mov byte [rip+disp],dil
						const auto disp = *reinterpret_cast<const std::int32_t*>(code + i + 3);
						return reinterpret_cast<volatile std::uint8_t*>(
							handler + i + 7 + static_cast<std::uintptr_t>(static_cast<std::intptr_t>(disp)));
					}
				}
				return nullptr;
			}();
			return cached;
		}
	};
}
