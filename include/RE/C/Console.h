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
		[[nodiscard]] static volatile std::uint8_t* GetCombatAIGate()
		{
			static volatile std::uint8_t* const cached = []() -> volatile std::uint8_t* {
				const auto        handler = ID::Console::ToggleCombatAI.address();
				const auto* const code = reinterpret_cast<const std::uint8_t*>(handler);
				for (std::size_t i = 0; i + 7 <= 0x60; ++i) {
					if (code[i] == 0x40 && code[i + 1] == 0x88 && code[i + 2] == 0x3D) {
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
