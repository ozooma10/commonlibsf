#pragma once

#include "RE/B/BSFixedString.h"
#include "RE/B/BSInputEventUser.h"
#include "RE/B/BSTArray.h"
#include "RE/B/BSTSingleton.h"

#include <array>
#include <span>
#include <string_view>

namespace RE
{
	class ControlMap :
		public BSTSingletonSDM<ControlMap>  // 000
	{
	public:
		SF_RTTI_VTABLE(ControlMap);

		enum class InputContextID : std::uint8_t
		{
			kMainGameplay = 0x00,
			kConsoleOpening = 0x01,
			kBasicMenuNav = 0x02,
			kVirtualController = 0x03,
			kMenu3DModelRotation = 0x04,
			kMenu3DModelZoom = 0x05,
			kQuickContainer = 0x06,
			kMultiActivate_A = 0x07,
			kMultiActivate_X = 0x08,
			kMultiActivate_Y = 0x09,
			kMultiActivate_B = 0x0A,
			kLeftThumbstick = 0x0B,
			kRightThumbstick = 0x0C,
			kCursor = 0x0D,
			kCursor_LeftStick = 0x0E,
			kBook = 0x0F,
			kDataSlateButtons = 0x10,
			kConsole = 0x11,
			kDebugOverlay = 0x12,
			kTFC = 0x13,
			kDebug = 0x14,
			kDebugCamera = 0x15,
			kWorkshop = 0x16,
			kWorkshop_QuickMenu = 0x17,
			kTerminal = 0x18,
			kPhotoMode = 0x19,
			kSecurity = 0x1A,
			kScope = 0x1B,
			kSitWait = 0x1C,
			kDataMenu = 0x1D,
			kDataMenu_Shortcuts = 0x1E,
			kShieldFocus = 0x1F,
			kZeroG = 0x20,
			kShipHUD = 0x21,
			kShipHUD_Cancel = 0x22,
			kShip_QuickContainer = 0x23,
			kShipHUD_Targeting = 0x24,
			kShipHUD_Alt = 0x25,
			kSpaceship_Interaction = 0x26,
			kShipFlightCam_FreeRot = 0x27,
			kStarMap = 0x28,
			kStarMap_JumpWindow = 0x29,
			kStarMap_SystemInspect = 0x2A,
			kSurfaceMap = 0x2B,
			kSpaceshipEditor = 0x2C,
			kShipBuilder_Empty = 0x2D,
			kShipBuilder_Add = 0x2E,
			kShipBuilder_Edit = 0x2F,
			kShipBuilder_Color = 0x30,
			kShipBuilder_FlightCheck = 0x31,
			kShipBuilder_ExitConfirm = 0x32,
			kShipBuilder_Selection = 0x33,
			kShipBuilder_Upgrade = 0x34,
			kShipInfo = 0x35,
			kShipInfo_BuySell = 0x36,
			kShipInfo_Modify = 0x37,
			kMonocle = 0x38,
			kMonocleKBMZoom = 0x39,
			kLevelUp = 0x3A,
			kNewMission = 0x3B,
			kMovementOverride = 0x3C,
			kTextInput = 0x3D,
			kMissionFromStarMap = 0x3E,
			kSpaceshipEditorSimplified = 0x3F,
			kShipBuilder_Add_Simple = 0x40,
			kShipBuilder_Empty_Simple = 0x41,
			kShipBuilder_Edit_Simple = 0x42,
			kShipBuilder_Selection_Simple = 0x43,
			kShipHUD_Monocle = 0x44,
			kBasicMenuNav_JustCancel = 0x45,
			kStarMap_QuickSelect = 0x46,
			kCreations = 0x47,
			kLogin = 0x48,
			kVehicle = 0x49,
			kVehicle_Hangar = 0x4A,
			kAlmanac = 0x4B,
			kOpenAlmanac = 0x4C,
			kShipHUD_CruiseMode = 0x4D,
			kStopContextProcessing = 0x4E,
			kCount = 0x4F,
			kNone = 0x50
		};

		enum class BindingSlot : std::uint8_t
		{
			kMain = 0,
			kAlternate = 1,
			kUnbound = 2
		};

		struct UserEventMapping
		{
			BSFixedString eventID;            // 00
			std::uint32_t keyCode;            // 08
			std::uint32_t modifierKeyCode;    // 0C
			BindingSlot   bindingSlot;        // 10
			std::uint8_t  unk11;              // 11
			std::uint16_t unk12;              // 12
			std::uint8_t  sortIndex;          // 14
			std::byte     pad15[3];           // 15
			std::uint32_t contextMask;        // 18
			std::uint8_t  metadata;           // 1C
			bool          visibleInControls;  // 1D
			bool          defaultWasUnbound;  // 1E
			std::uint8_t  unk1F;              // 1F
			bool          required;           // 20
			std::byte     pad21[7];           // 21
		};
		static_assert(offsetof(UserEventMapping, keyCode) == 0x08);
		static_assert(offsetof(UserEventMapping, bindingSlot) == 0x10);
		static_assert(offsetof(UserEventMapping, sortIndex) == 0x14);
		static_assert(offsetof(UserEventMapping, contextMask) == 0x18);
		static_assert(offsetof(UserEventMapping, visibleInControls) == 0x1D);
		static_assert(offsetof(UserEventMapping, required) == 0x20);
		static_assert(sizeof(UserEventMapping) == 0x28);

		struct InputContext
		{
			std::array<BSTArray<UserEventMapping>, 3> deviceMappings;  // 00
		};
		static_assert(sizeof(InputContext) == 0x30);

		struct ActiveInputContext
		{
			InputContextID inputContextID;  // 00
			std::byte      pad01[0x0F];     // 01
		};
		static_assert(sizeof(ActiveInputContext) == 0x10);

		static constexpr std::size_t MAPPABLE_INPUT_CONTEXT_COUNT = 0x4E;
		static constexpr std::size_t INPUT_CONTEXT_NAME_COUNT = 0x51;

		[[nodiscard]] static ControlMap* GetSingleton()
		{
			static REL::Relocation<ControlMap**> singleton{ ID::ControlMap::Singleton };
			return *singleton;
		}

		[[nodiscard]] InputContext* GetInputContext(InputContextID a_id) noexcept
		{
			const auto index = static_cast<std::size_t>(a_id);
			return index < inputContexts.size() ? inputContexts[index] : nullptr;
		}

		[[nodiscard]] const InputContext* GetInputContext(InputContextID a_id) const noexcept
		{
			const auto index = static_cast<std::size_t>(a_id);
			return index < inputContexts.size() ? inputContexts[index] : nullptr;
		}

		[[nodiscard]] std::span<const UserEventMapping> GetMappings(
			InputContextID a_id, InputEvent::DeviceType a_device) const noexcept
		{
			const auto  device = static_cast<std::size_t>(a_device);
			const auto* inputContext = GetInputContext(a_id);
			if (!inputContext || device >= inputContext->deviceMappings.size()) {
				return {};
			}

			const auto& mappings = inputContext->deviceMappings[device];
			return { mappings.data(), mappings.size() };
		}

		[[nodiscard]] std::span<const ActiveInputContext> GetActiveInputContexts() const noexcept
		{
			return { activeInputContexts.data(), activeInputContexts.size() };
		}

		[[nodiscard]] static std::string_view GetInputContextName(InputContextID a_id) noexcept
		{
			const auto index = static_cast<std::size_t>(a_id);
			if (index >= INPUT_CONTEXT_NAME_COUNT) {
				return {};
			}

			static REL::Relocation<const char* const*> names{ ID::ControlMap::InputContextNameTable };
			const auto*                                name = names.get()[index];
			return name ? std::string_view{ name } : std::string_view{};
		}

		// members
		std::array<InputContext*, MAPPABLE_INPUT_CONTEXT_COUNT> inputContexts;        // 010
		std::byte                                               pad280[0x28];         // 280
		BSTArray<ActiveInputContext>                            activeInputContexts;  // 2A8
		std::byte                                               pad2B8[0xE8];         // 2B8
	};
	static_assert(offsetof(ControlMap, inputContexts) == 0x10);
	static_assert(offsetof(ControlMap, activeInputContexts) == 0x2A8);
	static_assert(sizeof(ControlMap) == 0x3A0);
}
