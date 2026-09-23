#include "RE/M/Misc.h"

namespace RE
{
	class IMessageBoxCallback;
	class MessageMenuManager;

	namespace
	{
		// Deliberately local to this binding: the shared BSStringT header does
		// not describe this 1.16.244 representation. The names below are local
		// descriptions, not claims about the engine's original type names.
		struct alignas(8) MessageBoxString
		{
			void Assign(const char* a_text)
			{
				using func_t = bool (*)(MessageBoxString*, const char*, std::size_t);
				static REL::Relocation<func_t> func{ ID::MessageMenuManager::RequestStringAssign };
				func(this, a_text, 0);
			}

			// Capacity includes the terminator. At capacity <= 12 storage holds
			// inline characters; above 12 its first qword holds the owned pointer.
			std::byte     storage[0xC]{}; // 00
			std::uint16_t capacity{ 1 };  // 0C
			std::uint16_t length{};       // 0E
		};
		static_assert(sizeof(MessageBoxString) == 0x10);
		static_assert(offsetof(MessageBoxString, capacity) == 0xC);
		static_assert(offsetof(MessageBoxString, length) == 0xE);

		struct MessageBoxRequest
		{
			MessageBoxRequest() = default;
			MessageBoxRequest(const MessageBoxRequest&) = delete;
			MessageBoxRequest& operator=(const MessageBoxRequest&) = delete;
			MessageBoxRequest(MessageBoxRequest&&) = delete;
			MessageBoxRequest& operator=(MessageBoxRequest&&) = delete;

			~MessageBoxRequest()
			{
				using func_t = void (*)(MessageBoxRequest*);
				static REL::Relocation<func_t> func{ ID::MessageMenuManager::RequestDestroy };
				func(this);
			}

			// The engine moves the strings/callback out and resets the source.
			// Its destructor also handles an unconsumed request. Keep the string
			// members trivially destructible so they are released exactly once.
			IMessageBoxCallback* callback{}; // 00 (owned intrusive pointer)
			MessageBoxString     title;      // 08
			MessageBoxString     text;       // 18
			MessageBoxString     unk28;      // 28 (empty for Debug.MessageBox)
			std::uint32_t        category{ 5 }; // 38 (Debug.MessageBox's category)
		};
		static_assert(sizeof(MessageBoxRequest) == 0x40);
		static_assert(offsetof(MessageBoxRequest, title) == 0x08);
		static_assert(offsetof(MessageBoxRequest, text) == 0x18);
		static_assert(offsetof(MessageBoxRequest, unk28) == 0x28);
		static_assert(offsetof(MessageBoxRequest, category) == 0x38);
		static_assert(std::is_trivially_destructible_v<MessageBoxString>);
	}

	void DebugMessageBox(const char* a_message)
	{
		static REL::Relocation<MessageMenuManager**> singleton{ ID::MessageMenuManager::Singleton };
		auto* manager = *singleton;
		if (!manager) {
			return;
		}

		MessageBoxRequest request;
		request.title.Assign("DEBUG");
		request.text.Assign(a_message);

		// The engine adds {"Accept", "$OK"}, moves this 0x40-byte request into
		// a 0x60-byte queued message, and opens MessageBoxMenu via UIMessageQueue.
		// The final bool suppresses matching queued text when true; Papyrus uses false.
		using func_t = void (*)(MessageMenuManager*, MessageBoxRequest*, bool);
		static REL::Relocation<func_t> show{ ID::MessageMenuManager::ShowOneButton };
		show(manager, &request, false);
	}
}
