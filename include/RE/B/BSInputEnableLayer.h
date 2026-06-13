#pragma once

#include "RE/B/BSInputEnableManager.h"

namespace RE
{
	// Ownership: AllocateNewLayer hands back a pooled layer at m_refCount == 2 (1 for
	// the manager's pool slot, 1 for the caller). Disable events on it, then DecRef
	// once when done: that drops it to 1, which runs LayerFreed (resets the layer's
	// mask to all-enabled, recomputes the cached aggregate, fires the change events)
	// and returns the layer to the pool — controls are restored with no leak. The
	// 1 -> 0 transition (object free) only happens on manager teardown.
	class BSInputEnableLayer
	{
	public:
		// Delegates to the engine release (ID 45194): decrements m_refCount and, at
		// 2 -> 1, runs the inlined BSInputEnableManager::LayerFreed. Do NOT reimplement
		// the decrement here — the engine function performs it.
		inline void DecRef()
		{
			using func_t = decltype(&BSInputEnableLayer::DecRef);
			static REL::Relocation<func_t> func{ ID::BSInputEnableLayer::DecRef };
			func(this);
		}

		inline void EnableUserEvent(USER_EVENT_FLAG a_flags, bool a_enable, USER_EVENT_SENDER_ID a_sender = USER_EVENT_SENDER_ID::None)
		{
			BSInputEnableManager::EnableUserEvent(&m_layerID, a_flags, a_enable, a_sender);
		}

		inline void EnableOtherEvent(OTHER_EVENT_FLAG a_flags, bool a_enable, USER_EVENT_SENDER_ID a_sender = USER_EVENT_SENDER_ID::None)
		{
			BSInputEnableManager::EnableOtherEvent(&m_layerID, a_flags, a_enable, a_sender);
		}

		inline std::uint32_t GetLayerID() const
		{
			return m_layerID;
		}

		inline void IncRef()
		{
			REX::W32::InterlockedIncrement(&m_refCount);
		}

	private:
		// members
		std::uint32_t m_layerID;
		std::uint32_t m_refCount;
		std::uint64_t unk08;
		std::uint64_t unk10;
		std::uint64_t unk18;
	};
}
