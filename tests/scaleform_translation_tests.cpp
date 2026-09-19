#include "SFSE/Impl/PCH.h"
#include "RE/B/BSScaleformManager.h"
#include "RE/B/BSScaleformTranslator.h"

#include <iostream>
#include <map>
#include <memory>
#include <stdexcept>

namespace
{
	using Translator = RE::BSScaleformTranslator;
	std::map<std::wstring, RE::BSStringPool::Entry*> pool;
	std::unique_ptr<Translator::Entry[]> table;
	unsigned loads{}, reads{}, assignments{};
	bool finishLoad{ true }, omitKey{};

	void GetString(RE::BSStringPool::Entry*& a_result, const wchar_t* a_text, bool a_caseSensitive)
	{
		if (!a_caseSensitive) throw std::runtime_error("translation strings must be case-sensitive");
		const auto found = pool.find(a_text);
		if (found != pool.end()) {
			a_result = found->second;
			a_result->Acquire();
			return;
		}
		const auto bytes = std::wcslen(a_text) * sizeof(wchar_t);
		auto* storage = new std::byte[sizeof(RE::BSStringPool::Entry) + bytes + sizeof(wchar_t)]{};
		a_result = std::construct_at(reinterpret_cast<RE::BSStringPool::Entry*>(storage));
		a_result->_length = static_cast<std::uint32_t>(bytes);
		a_result->_refCount = 1;
		std::memcpy(a_result + 1, a_text, bytes + sizeof(wchar_t));
		pool.emplace(a_text, a_result);
	}

	void Release(RE::BSStringPool::Entry*& a_entry)
	{
		if (a_entry && --a_entry->_refCount == 0) {
			pool.erase(a_entry->data<wchar_t>());
			delete[] reinterpret_cast<std::byte*>(a_entry);
		}
		a_entry = nullptr;
	}

	RE::BSFixedStringWCS* Assign(RE::BSFixedStringWCS* a_result, const wchar_t* a_text)
	{
		++assignments;
		*a_result = a_text;
		return a_result;
	}

	// The fixture supplies the engine boundary. The real library performs stream
	// adaptation, placeholder creation, post-load lookup and exact-value assignment.
	void Load(Translator* a_translator, Translator::StreamParser* a_parser)
	{
		++loads;
		if (!a_parser->source->Ready()) throw std::runtime_error("stream unavailable");
		auto* stream = a_parser->source->Stream();
		std::wstring resource;
		wchar_t chunk[7];
		while (const auto count = stream->DoRead(chunk, sizeof(chunk))) {
			++reads;
			resource.append(chunk, count / sizeof(wchar_t));
			stream->absolutePosition += count;
		}
		if (stream->absolutePosition != a_parser->totalBytes) throw std::runtime_error("incorrect parser byte count or stream position");
		if (!finishLoad) return;
		a_parser->source->Finish();

		std::map<std::wstring, std::wstring> values;
		for (std::size_t i = 0; i < a_translator->capacity; ++i) {
			const auto& entry = a_translator->entries[i];
			if (entry.nextIndex != -1) values.emplace(entry.key.c_str(), entry.value.c_str());
		}
		for (std::size_t start = 0; start < resource.size();) {
			const auto end = resource.find(L'\n', start);
			const auto line = resource.substr(start, end - start);
			const auto tab = line.find(L'\t');
			if (tab == line.npos || line.front() != L'$') throw std::runtime_error("invalid resource line");
			const auto key = line.substr(0, tab);
			if (!(omitKey && key == L"$Omitted")) {
				auto value = line.substr(tab + 1);
				if (!value.empty() && value.back() == L'\r') value.pop_back();
				values[key] = std::move(value);
			}
			if (end == resource.npos) break;
			start = end + 1;
		}
		std::size_t capacity = 8;
		while (capacity < values.size() * 2) capacity *= 2;
		auto next = std::make_unique<Translator::Entry[]>(capacity);
		for (std::size_t i = 0; i < capacity; ++i) next[i].nextIndex = -1;
		std::size_t index = 0;
		for (const auto& [key, value] : values) {
			next[index].key = key.c_str();
			next[index].value = value.c_str();
			next[index].nextIndex = static_cast<std::int32_t>(capacity);
			++index;
		}
		table = std::move(next);  // Always invalidate the old entries, even on replay.
		a_translator->entries = table.get();
		a_translator->capacity = capacity;
		a_translator->free = capacity - values.size();
	}

	std::wstring Value(const Translator& a_translator, const wchar_t* a_key)
	{
		for (const auto& entry : std::span(a_translator.entries, a_translator.capacity)) {
			if (entry.nextIndex != -1 && std::wstring_view(entry.key.c_str()) == a_key) return entry.value.c_str();
		}
		throw std::runtime_error("missing registered translation");
	}

	RE::BSFixedStringWCS* Translate(Translator::ScaleformImpl*, RE::BSFixedStringWCS* a_result, const wchar_t* a_key)
	{
		return std::construct_at(a_result, a_key);
	}
}

namespace REL
{
	IDDB::IDDB() = default;
	std::uint64_t IDDB::offset(std::uint64_t a_id) const
	{
		std::uintptr_t address{};
		if (a_id == RE::ID::BSScaleformTranslator::LoadTranslations.id()) address = reinterpret_cast<std::uintptr_t>(&Load);
		else if (a_id == RE::ID::BSScaleformTranslator::ScaleformImpl::Translate.id()) address = reinterpret_cast<std::uintptr_t>(&Translate);
		else if (a_id == RE::ID::BSFixedStringWCS::Assign.id()) address = reinterpret_cast<std::uintptr_t>(&Assign);
		else if (a_id == RE::ID::BSStringPool::GetEntryW.id()) address = reinterpret_cast<std::uintptr_t>(&GetString);
		else if (a_id == RE::ID::BSStringPool::Entry::Release.id()) address = reinterpret_cast<std::uintptr_t>(&Release);
		else throw std::runtime_error("unexpected relocation " + std::to_string(a_id));
		return address - REX::FModule::GetExecutingModule().GetBaseAddress();
	}
}

int main()
{
	unsigned checks{};
	const auto check = [&](bool a_passed, const char* a_message) {
		if (!a_passed) throw std::runtime_error(a_message);
		++checks;
	};
	try {
		Translator translator{};
		check(translator.RegisterTranslations({}) && loads == 0, "empty registration does not call the engine");
		check(translator.LoadTranslations(std::wstring_view{}) && loads == 0, "empty resource does not call the engine");
		const Translator::Translation invalid[] = {{ L"$Good", L"one" }, { L"$Injected\n$key", L"two" }};
		check(!translator.RegisterTranslations(invalid) && loads == 0, "validate every key before loading anything");
		const Translator::Translation nullKey[] = {{ nullptr, L"value" }};
		check(!translator.RegisterTranslations(nullKey) && loads == 0, "null keys rejected");
		const Translator::Translation missingPrefix[] = {{ L"Key", L"value" }};
		check(!translator.RegisterTranslations(missingPrefix) && loads == 0, "resource keys require dollar prefix");
		check(translator.LoadTranslations(L"$Vanilla\tOriginal\n"), "native resource loader finishes borrowed stream");
		const auto readsBefore = reads;
		{
			const std::wstring exact = L"\tCR\rLF\nTAB\t literal\\n $dollar <tag> Caf\u00e9 \U0001f680";
			const std::wstring longValue(700, L'\u00e9');
			const Translator::Translation labels[] = {
				{ L"$Exact", exact.c_str() }, { L"$Long", longValue.c_str() },
				{ L"$Duplicate", L"first" }, { L"$Duplicate", L"last" },
				{ L"$Empty", L"" }, { L"$Null", nullptr }
			};
			check(translator.RegisterTranslations(labels), "register through native loader and assignment");
			check(Value(translator, L"$Exact") == exact && Value(translator, L"$Long") == longValue,
				"exact values bypass resource escape and line parsing");
			check(Value(translator, L"$Duplicate") == L"last", "last duplicate wins");
			check(Value(translator, L"$Empty").empty() && Value(translator, L"$Null").empty(), "empty values preserved");
			check(Value(translator, L"$Vanilla") == L"Original", "unrelated translations preserved");
			check(reads > readsBefore + 1 && translator.capacity > 8, "chunked stream and table growth exercised");
			const auto count = translator.capacity - translator.free;
			check(translator.RegisterTranslations(labels) && translator.capacity - translator.free == count, "replay does not add duplicate keys");
		}
		check(Value(translator, L"$Long") == std::wstring(700, L'\u00e9'), "engine owns values after caller buffers die");
		const wchar_t* borrowedValue{};
		for (const auto& entry : std::span(translator.entries, translator.capacity)) {
			if (entry.nextIndex != -1 && std::wstring_view(entry.key.c_str()) == L"$Long") borrowedValue = entry.value.c_str();
		}
		const Translator::Translation borrowed[] = {{ L"$Long", borrowedValue }};
		check(translator.RegisterTranslations(borrowed) && Value(translator, L"$Long") == std::wstring(700, L'\u00e9'),
			"input borrowed from an overwritten table value survives loading");
		const auto assignedBefore = assignments;
		finishLoad = false;
		const Translator::Translation unfinished[] = {{ L"$Unfinished", L"value" }};
		check(!translator.RegisterTranslations(unfinished) && assignments == assignedBefore, "incomplete load never assigns value slots");
		finishLoad = true;
		omitKey = true;
		const Translator::Translation omitted[] = {{ L"$Omitted", L"value" }};
		check(!translator.RegisterTranslations(omitted), "missing loaded key is reported");
		Translator::ScaleformImpl impl{};
		check(std::wstring_view(impl.Translate(L"$CasePreserved").c_str()) == L"$CasePreserved", "typed translation wrapper preserves result ownership");
		alignas(void*) std::byte managerStorage[0x28]{};
		std::memcpy(managerStorage + 0x20, &translator.impl, sizeof(void*));
		auto* manager = reinterpret_cast<RE::BSScaleformManager*>(managerStorage);
		check(!manager->GetTranslator(), "manager exposes null before publication");
		auto* published = &translator;
		std::memcpy(managerStorage + 0x20, &published, sizeof(published));
		check(manager->GetTranslator() == &translator, "manager returns borrowed published wrapper");
		table.reset();
		check(pool.empty(), "all pooled references released");
		std::cout << checks << " translation helper checks passed\n";
		return 0;
	} catch (const std::exception& error) {
		std::cerr << error.what() << '\n';
		return 1;
	}
}
