#pragma once

namespace RE
{
	enum class FormType;

	struct FORM_ENUM_STRING
	{
	public:
		[[nodiscard]] static std::span<FORM_ENUM_STRING, 220> GetFormEnumString()
		{
			static REL::Relocation<FORM_ENUM_STRING(*)[220]> formEnumString{ ID::FORM_ENUM_STRING::FormEnumString };
			return { *formEnumString };
		}

		// members
		const char*   formString;  // 00
		std::uint32_t signature;   // 08
		FormType      formType;    // 0C
	};
	static_assert(offsetof(FORM_ENUM_STRING, formType) == 0xC);
	static_assert(sizeof(FORM_ENUM_STRING) == 0x10);
}
