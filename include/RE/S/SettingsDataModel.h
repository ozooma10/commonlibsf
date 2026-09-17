#pragma once

#include "RE/B/BSInputEventSingleUser.h"
#include "RE/B/BSTEvent.h"
#include "RE/I/IDataModel.h"

#include <cstddef>

namespace RE
{
	class SettingsPanel_RemapMode;
	class SettingsPanel_SaveControls;
	class SettingsPanel_SaveSettings;
	class SettingsPanel_SliderChanged;
	class SettingsPanel_StepperChanged;
	class SettingsPanel_CheckBoxChanged;
	class SettingsPanel_ResetToDefaults;
	class SettingsPanel_OpenSettings;
	class SettingsPanel_OpenCategory;
	class SettingsPanel_ClearBinding;
	class SettingsPanel_RemapConfirmed;
	class SettingsPanel_ValidateControls;
	class SettingsPanel_LinkClicked;

	// 1.16.244: RTTI and constructor 0x150F310 establish the bases; allocation at
	// 0x15AC7EE establishes size 0x940. UI data members remain opaque.
	class SettingsDataModel :
		public IDataModel,                                   // 000
		public BSInputEventSingleUser,                       // 010 (BSInputEventUser at 020)
		public BSTSingletonExplicit<SettingsDataModel>,      // 0C8 (empty)
		public BSTEventSink<SettingsPanel_RemapMode>,         // 060
		public BSTEventSink<SettingsPanel_SaveControls>,      // 068
		public BSTEventSink<SettingsPanel_SaveSettings>,      // 070
		public BSTEventSink<SettingsPanel_SliderChanged>,     // 078
		public BSTEventSink<SettingsPanel_StepperChanged>,    // 080
		public BSTEventSink<SettingsPanel_CheckBoxChanged>,   // 088
		public BSTEventSink<SettingsPanel_ResetToDefaults>,  // 090
		public BSTEventSink<SettingsPanel_OpenSettings>,     // 098
		public BSTEventSink<SettingsPanel_OpenCategory>,     // 0A0
		public BSTEventSink<SettingsPanel_ClearBinding>,     // 0A8
		public BSTEventSink<SettingsPanel_RemapConfirmed>,   // 0B0
		public BSTEventSink<SettingsPanel_ValidateControls>, // 0B8
		public BSTEventSink<SettingsPanel_LinkClicked>       // 0C0
	{
	public:
		SF_RTTI_VTABLE(SettingsDataModel);

		~SettingsDataModel() override;

		[[nodiscard]] static SettingsDataModel* GetSingleton()
		{
			static REL::Relocation<SettingsDataModel**> singleton{ ID::SettingsDataModel::Singleton };
			return *singleton;
		}

		// Native global operation, not an instance method. Call on the native input/settings thread.
		static void CancelRemap()
		{
			using func_t = decltype(&SettingsDataModel::CancelRemap);
			static REL::Relocation<func_t> func{ ID::SettingsDataModel::CancelRemap };
			func();
		}

		// Event payloads and handlers have not been exposed yet.
		// members
		std::byte unk0C8[0x878];  // 0C8
	};
	static_assert(offsetof(SettingsDataModel, unk08) == 0x8);
	static_assert(offsetof(SettingsDataModel, currInputTimeCount) == 0x18);
	static_assert(offsetof(SettingsDataModel, inputEventHandlingEnabled) == 0x58);
	static_assert(offsetof(SettingsDataModel, unk0C8) == 0xC8);
	static_assert(sizeof(SettingsDataModel) == 0x940);
}
