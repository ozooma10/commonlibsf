#pragma once

#include "RE/B/BSInputEventReceiver.h"
#include "RE/B/BSInputEventUser.h"
#include "RE/B/BSTSingleton.h"

#include <cstddef>
#include <span>

namespace RE
{
	class MenuControls :
		public BSInputEventReceiver,             // 00
		public BSTSingletonSDM<MenuControls>      // 10
	{
	public:
		SF_RTTI_VTABLE(MenuControls);

		[[nodiscard]] static MenuControls* GetSingleton();
		void PerformInputProcessing(const InputEvent* a_queueHead) override; // 01

		// Convenience wrapper for vanilla's inlined insertion sequence. Borrows
		// a_handler; the caller must unregister it before destroying its storage.
		// An existing registration succeeds without inserting or enabling it again.
		// These operations do not lock. Use only during native initialization or
		// teardown, or another phase proven to exclude input dispatch.
		[[nodiscard]] bool RegisterHandler(BSInputEventUser* a_handler);
		void UnregisterHandler(BSInputEventUser* a_handler); // Native unordered removal; does not destroy/disable the handler.

		[[nodiscard]] std::uint32_t GetHandlerCount() const noexcept { return handlers.size; }
		// Borrowed view; invalidated by registration/removal or native teardown.
		[[nodiscard]] std::span<BSInputEventUser* const> GetHandlers() const noexcept
		{
			return { handlers.data, handlers.size };
		}

		// Engine-owned storage, 1.16.244. Use RegisterHandler/UnregisterHandler to
		// mutate it; the native allocator uses a matching sized-free operation.
		struct HandlerStorage
		{
			std::uint32_t size{};
			std::uint32_t capacity{};
			BSInputEventUser** data{};
		};
		static_assert(sizeof(HandlerStorage) == 0x10);
		static_assert(offsetof(HandlerStorage, data) == 0x08);

		HandlerStorage handlers;                    // 20
		BSInputEventUser* disconnectHandler{};       // 30 - separately owned vanilla handlers
		BSInputEventUser* deviceChangeHandler{};     // 38
		BSInputEventUser* quickSaveLoadHandler{};    // 40
		BSInputEventUser* menuOpenHandler{};         // 48
		BSInputEventUser* unk50{};                   // 50 - zeroed by the native initializer
		BSInputEventUser* gpuCaptureHandler{};       // 58
		BSInputEventUser* cpuCaptureHandler{};       // 60
		BSInputEventUser* cameraZoomHandler{};       // 68
		std::byte unk70[0x08]{};                    // 70

	protected:
		// The engine owns the singleton and its native construction/destruction.
		MenuControls() = default;
		~MenuControls() override = default;
	};
	static_assert(offsetof(MenuControls, handlers) == 0x20);
	static_assert(offsetof(MenuControls, disconnectHandler) == 0x30);
	static_assert(offsetof(MenuControls, cameraZoomHandler) == 0x68);
	static_assert(sizeof(MenuControls) == 0x78);
}
