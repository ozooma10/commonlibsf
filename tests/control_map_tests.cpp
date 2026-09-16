#include "SFSE/Impl/PCH.h"
#include "RE/C/ControlMap.h"

#include <cstring>
#include <iostream>
#include <stdexcept>

namespace
{
	enum class Operation { Load, Parse, Resolve };

	struct Call
	{
		Operation       operation;
		RE::ControlMap* map;
		const char*     text{};
	};
	std::vector<Call> calls;

	void Load(RE::ControlMap* a_map) { calls.push_back({ Operation::Load, a_map }); }
	void Parse(RE::ControlMap* a_map, const char* a_text) { calls.push_back({ Operation::Parse, a_map, a_text }); }
	void Resolve(RE::ControlMap* a_map) { calls.push_back({ Operation::Resolve, a_map }); }

	struct RowCall
	{
		void*                        output;
		const char*                  event;
		std::array<const char*, 3>    tokens;
		std::array<std::uint32_t, 3>  codes;
		std::array<bool, 3>           visible;
		std::uint32_t                controlMask;
		std::uint32_t                groupMask;
		bool                         required;
	};
	std::vector<RowCall> rowCalls;
	std::vector<void*> destroyedStrings;
	constexpr char tokenResult[] = "engine\trow\n";  // Exercises the full 12-byte inline buffer, including NUL.
	constexpr char numericResult[] = "native-generated numeric mapping row\n";

	// Model the observed native output ABI independently of the private wrapper type.
	void* MakeString(void* a_output, const char* a_text)
	{
		const auto size = std::strlen(a_text) + 1;
		std::memset(a_output, 0, 0x10);
		if (size > 12) {
			auto* data = new char[size];
			std::memcpy(data, a_text, size);
			std::memcpy(a_output, &data, sizeof(data));
		} else {
			std::memcpy(a_output, a_text, size);
		}
		const auto capacity = static_cast<std::uint16_t>(size);
		std::memcpy(static_cast<char*>(a_output) + 0xC, &capacity, sizeof(capacity));
		return a_output;
	}

	void DestroyString(void* a_output)
	{
		std::uint16_t capacity;
		std::memcpy(&capacity, static_cast<char*>(a_output) + 0xC, sizeof(capacity));
		if (capacity > 12) {
			char* data;
			std::memcpy(&data, a_output, sizeof(data));
			std::memset(data, 0xDD, capacity);
			delete[] data;
		}
		std::memset(a_output, 0xDD, 0x10);
		destroyedStrings.push_back(a_output);
	}

	void* FormatTokens(void* a_output, const char* a_event,
		const char* a_keyboard, const char* a_mouse, const char* a_gamepad,
		bool a_keyboardVisible, bool a_mouseVisible, bool a_gamepadVisible,
		std::uint32_t a_controlMask, std::uint32_t a_groupMask, bool a_required)
	{
		rowCalls.push_back({ a_output, a_event, { a_keyboard, a_mouse, a_gamepad }, {},
			{ a_keyboardVisible, a_mouseVisible, a_gamepadVisible }, a_controlMask, a_groupMask, a_required });
		return MakeString(a_output, tokenResult);
	}

	void* FormatCodes(void* a_output, const char* a_event,
		std::uint32_t a_keyboard, std::uint32_t a_mouse, std::uint32_t a_gamepad,
		bool a_keyboardVisible, bool a_mouseVisible, bool a_gamepadVisible,
		std::uint32_t a_controlMask, std::uint32_t a_groupMask, bool a_required)
	{
		rowCalls.push_back({ a_output, a_event, {}, { a_keyboard, a_mouse, a_gamepad },
			{ a_keyboardVisible, a_mouseVisible, a_gamepadVisible }, a_controlMask, a_groupMask, a_required });
		return MakeString(a_output, numericResult);
	}
}

// Substitute only the native entry points. The real CommonLibSF wrappers execute.
namespace REL
{
	IDDB::IDDB() = default;

	std::uint64_t IDDB::offset(std::uint64_t a_id) const
	{
		std::uintptr_t address{};
		switch (a_id) {
		case 35706: address = reinterpret_cast<std::uintptr_t>(&DestroyString); break;
		case 124100: address = reinterpret_cast<std::uintptr_t>(&FormatTokens); break;
		case 124101: address = reinterpret_cast<std::uintptr_t>(&FormatCodes); break;
		case 124116: address = reinterpret_cast<std::uintptr_t>(&Load); break;
		case 124117: address = reinterpret_cast<std::uintptr_t>(&Parse); break;
		case 124124: address = reinterpret_cast<std::uintptr_t>(&Resolve); break;
		default: throw std::runtime_error("Unexpected engine relocation: " + std::to_string(a_id));
		}
		return address - REX::FModule::GetExecutingModule().GetBaseAddress();
	}
}

int main()
{
	unsigned   checks{};
	const auto check = [&](bool a_passed, const char* a_message) {
		if (!a_passed) {
			throw std::runtime_error(a_message);
		}
		++checks;
	};
	try {
		RE::ControlMap first{}, second{};
		const char     defaults[] = "Jump\t0x20\t0xff\t0xff\t1\t0\t0\t0x401\t0x0\t0\r\n";
		first.ParseMappings(defaults);
		check(calls.size() == 1 && calls.back().operation == Operation::Parse &&
			      calls.back().map == &first && calls.back().text == defaults,
			"parser forwards the exact map and text in one native call");

		const char empty[] = "";
		second.ParseMappings(empty);
		check(calls.size() == 2 && calls.back().operation == Operation::Parse &&
			      calls.back().map == &second && calls.back().text == empty,
			"cached relocation forwards the new instance and leaves empty-input behavior to the engine");

		first.LoadMappings();
		check(calls.size() == 3 && calls.back().operation == Operation::Load && calls.back().map == &first,
			"loader invokes the native defaults operation without synthesizing parser or finalizer calls");
		second.ResolveLinkedMappings();
		check(calls.size() == 4 && calls.back().operation == Operation::Resolve && calls.back().map == &second,
			"link finalization forwards the target instance without reloading defaults");

		const char event[] = "osfsettings/openMenu";
		const std::array<const char*, 3> tokens{ "0x79", "0xff", "!Jump" };
		const auto tokenRow = RE::ControlMap::FormatMappingRow(event, tokens[0], tokens[1], tokens[2],
			true, false, true, 0x80000401u, 0x40000002u, false);
		check(rowCalls.size() == 1 && rowCalls.back().event == event && rowCalls.back().tokens == tokens &&
			      rowCalls.back().visible == std::array{ true, false, true } &&
			      rowCalls.back().controlMask == 0x80000401u && rowCalls.back().groupMask == 0x40000002u &&
			      !rowCalls.back().required,
			"token formatter receives the original pointers, per-device flags and full masks");
		check(tokenRow == tokenResult && destroyedStrings.size() == 1 && destroyedStrings.back() == rowCalls.back().output,
			"inline native text is copied before its temporary is destroyed exactly once");

		const std::array<std::uint32_t, 3> codes{ 0x79, 0xFF, 0x80000000u };
		const auto numericRow = RE::ControlMap::FormatMappingRow(event, codes[0], codes[1], codes[2],
			false, true, false, 0x10u, 0x80000001u, true);
		check(rowCalls.size() == 2 && rowCalls.back().event == event && rowCalls.back().codes == codes &&
			      rowCalls.back().visible == std::array{ false, true, false } &&
			      rowCalls.back().controlMask == 0x10u && rowCalls.back().groupMask == 0x80000001u && rowCalls.back().required,
			"numeric formatter receives full device codes, per-device flags and masks without custom conversion");
		check(numericRow == numericResult && destroyedStrings.size() == 2 && destroyedStrings.back() == rowCalls.back().output &&
			      calls.size() == 4,
			"heap-backed native text is copied and destroyed exactly once without changing the control map");
		std::cout << checks << '/' << checks << " ControlMap interface checks passed\n";
		return 0;
	} catch (const std::exception& error) {
		std::cerr << "FAILED: " << error.what() << '\n';
		return 1;
	}
}
