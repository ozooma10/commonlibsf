#pragma once

#include "RE/B/BSCoreTypes.h"
#include "RE/E/Events.h"
#include "RE/IDs_VTABLE.h"

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <memory>
#include <optional>

namespace RE
{
	// Non-owning 1.16.244 views over the live Starmap menu/state layout.
	// Callers must prove that their pointers name the corresponding engine
	// objects before using these accessors.
	class StarMap::GalaxyState
	{
	public:
		inline static constexpr auto PRIMARY_VTABLE = VTABLE::StarMap__GalaxyState[4];

		[[nodiscard]] TESFormID GetSelectedSystem() const noexcept
		{
			TESFormID result = 0;
			std::memcpy(std::addressof(result), bytes() + 0x880, sizeof(result));
			return result;
		}

		[[nodiscard]] bool IsQuickSelectOpen() const noexcept
		{
			std::uint8_t result = 0;
			std::memcpy(std::addressof(result), bytes() + 0x8F8, sizeof(result));
			return result != 0;
		}

		void SetQuickSelectOpen(bool a_open) noexcept
		{
			const std::uint8_t value = a_open ? 1 : 0;
			std::memcpy(bytes() + 0x8F8, std::addressof(value), sizeof(value));
		}

		void SelectSystem(TESFormID a_systemFormID, bool a_enterSystem = false)
		{
			using func_t = void (*)(GalaxyState*, TESFormID, bool);
			static REL::Relocation<func_t> func{ ID::StarMap::SelectGalaxySystem };
			func(this, a_systemFormID, a_enterSystem);
		}

		void CloseQuickSelect(void* a_dataModel)
		{
			using func_t = void (*)(GalaxyState*, void*);
			static REL::Relocation<func_t> func{ ID::StarMap::CloseGalaxyQuickSelect };
			func(this, a_dataModel);
		}

	private:
		[[nodiscard]] std::byte* bytes() noexcept
		{
			return reinterpret_cast<std::byte*>(this);
		}

		[[nodiscard]] const std::byte* bytes() const noexcept
		{
			return reinterpret_cast<const std::byte*>(this);
		}
	};

	class StarMap::Route
	{
	public:
		struct Point
		{
			std::uint32_t unk00;      // 00
			TESFormID    endpoint;    // 04
			std::byte    pad08[0x20]; // 08
		};
		static_assert(offsetof(Point, endpoint) == 0x04);
		static_assert(sizeof(Point) == 0x28);

		struct Snapshot
		{
			bool          alternate{ false };
			std::uint32_t pointCount{ 0 };
			TESFormID     endpoint{ 0 };

			friend bool operator==(const Snapshot&, const Snapshot&) = default;
		};

		[[nodiscard]] std::optional<Snapshot> GetSnapshot(std::uint32_t a_maxPoints) const noexcept
		{
			Snapshot result;
			std::uint8_t alternate = 0;
			std::memcpy(std::addressof(alternate), bytes() + 0x60, sizeof(alternate));
			result.alternate = alternate != 0;
			if (result.alternate) {
				std::memcpy(std::addressof(result.endpoint), bytes() + 0x3C, sizeof(result.endpoint));
				return result;
			}

			Point* points = nullptr;
			std::memcpy(std::addressof(result.pointCount), bytes() + 0x28, sizeof(result.pointCount));
			std::memcpy(std::addressof(points), bytes() + 0x30, sizeof(points));
			if (result.pointCount > a_maxPoints || (result.pointCount != 0 && !points)) {
				return std::nullopt;
			}
			if (result.pointCount != 0) {
				std::memcpy(std::addressof(result.endpoint),
					std::addressof(points[result.pointCount - 1].endpoint), sizeof(result.endpoint));
			}
			return result;
		}

	private:
		[[nodiscard]] const std::byte* bytes() const noexcept
		{
			return reinterpret_cast<const std::byte*>(this);
		}
	};

	class StarMap::StarMapMenu
	{
	public:
		inline static constexpr auto PRIMARY_VTABLE = VTABLE::StarMap__StarMapMenu[12];

		[[nodiscard]] void* GetDataModel() noexcept
		{
			return bytes() + 0x1B8;
		}

		[[nodiscard]] GalaxyState* GetGalaxyState() const noexcept
		{
			GalaxyState* result = nullptr;
			std::memcpy(std::addressof(result), bytes() + 0x1240, sizeof(result));
			return result;
		}

		[[nodiscard]] Route* GetRoute() noexcept
		{
			return reinterpret_cast<Route*>(bytes() + 0x1258);
		}

		[[nodiscard]] const Route* GetRoute() const noexcept
		{
			return reinterpret_cast<const Route*>(bytes() + 0x1258);
		}

	private:
		[[nodiscard]] std::byte* bytes() noexcept
		{
			return reinterpret_cast<std::byte*>(this);
		}

		[[nodiscard]] const std::byte* bytes() const noexcept
		{
			return reinterpret_cast<const std::byte*>(this);
		}
	};

	static_assert(StarMap::GalaxyState::PRIMARY_VTABLE.id() == 446425);
	static_assert(StarMap::StarMapMenu::PRIMARY_VTABLE.id() == 446845);
}
