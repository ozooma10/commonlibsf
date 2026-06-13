#pragma once

#include "RE/B/BSLock.h"
#include "RE/U/UserEvents.h"

namespace RE
{
	class BSInputEnableLayer;

	class BSInputEnableManager
	{
	public:
		struct IDebugNameFunctor
		{
			virtual ~IDebugNameFunctor() = default;

			virtual void operator()(BSFixedString& a_name) = 0;
		};

		template <class T>
		struct DebugNameFunctor : IDebugNameFunctor
		{
			DebugNameFunctor(const T a_functor) :
				m_functor(a_functor)
			{}

			virtual void operator()(BSFixedString& a_name) override
			{
				m_functor(a_name);
			}

		private:
			const T m_functor;
		};

		virtual ~BSInputEnableManager() = 0;

		static BSInputEnableManager* GetSingleton()
		{
			static REL::Relocation<BSInputEnableManager**> singleton{ ID::BSInputEnableManager::Singleton };
			return *singleton;
		}

		inline bool AllocateNewLayerImpl(BSInputEnableLayer** a_layer, IDebugNameFunctor* a_debugName)
		{
			using func_t = decltype(&BSInputEnableManager::AllocateNewLayerImpl);
			static REL::Relocation<func_t> func{ ID::BSInputEnableManager::AllocateNewLayerImpl };
			return func(this, a_layer, a_debugName);
		}

		inline bool AllocateNewLayer(BSInputEnableLayer** a_layer, const char* a_debugName)
		{
			DebugNameFunctor functor([a_debugName](BSFixedString& a_name) {
				BSFixedString name{ a_debugName };
				a_name = name;
			});

			return AllocateNewLayerImpl(a_layer, &functor);
		}

		inline static void EnableUserEvent(std::uint32_t* a_layer, USER_EVENT_FLAG a_flags, bool a_enable, USER_EVENT_SENDER_ID a_sender = USER_EVENT_SENDER_ID::None)
		{
			using func_t = decltype(&BSInputEnableManager::EnableUserEvent);
			static REL::Relocation<func_t> func{ ID::BSInputEnableManager::EnableUserEvent };
			func(a_layer, a_flags, a_enable, a_sender);
		}

		inline static void EnableOtherEvent(std::uint32_t* a_layer, OTHER_EVENT_FLAG a_flags, bool a_enable, USER_EVENT_SENDER_ID a_sender = USER_EVENT_SENDER_ID::None)
		{
			using func_t = decltype(&BSInputEnableManager::EnableOtherEvent);
			static REL::Relocation<func_t> func{ ID::BSInputEnableManager::EnableOtherEvent };
			func(a_layer, a_flags, a_enable, a_sender);
		}

		inline void ForceUserEventEnabled(USER_EVENT_FLAG a_flags, bool a_enabled)
		{
			BSAutoLock _(m_cacheLock);
			if (a_enabled)
				m_forcedUserEventFlags |= a_flags;
			else
				m_forcedUserEventFlags &= ~a_flags;
		}

		inline void ForceOtherEventEnabled(OTHER_EVENT_FLAG a_flags, bool a_enabled)
		{
			BSAutoLock _(m_cacheLock);
			if (a_enabled)
				m_forcedOtherEventFlags |= a_flags;
			else
				m_forcedOtherEventFlags &= ~a_flags;
		}

		// members
		std::byte            m_pad008[0x080];          // 0x008
		BSSpinLock           m_cacheLock;              // 0x088
		USER_EVENT_FLAG      m_cachedUserEventFlags;   // 0x090 AND of every active layer's user mask (the effective gate)
		OTHER_EVENT_FLAG     m_cachedOtherEventFlags;  // 0x094 AND of every active layer's other mask
		USER_EVENT_FLAG      m_forcedUserEventFlags;   // 0x098
		OTHER_EVENT_FLAG     m_forcedOtherEventFlags;  // 0x09C
		BSSpinLock           m_layerLock;              // 0x0A0
		std::uint32_t        m_layerCount;             // 0x0A8 number of layer slots (runtime 1.16.244: 100)
		std::uint32_t        m_pad0AC;                 // 0x0AC (likely m_layerCapacity)
		std::uint64_t*       m_layerEventFlags;        // 0x0B0 per-layer packed mask {user:lo32, other:hi32}; ~0ull = all enabled
		std::uint32_t        m_layerPoolCount;         // 0x0B8 (== m_layerCount; runtime 100)
		std::uint32_t        m_pad0BC;                 // 0x0BC (likely capacity)
		BSInputEnableLayer** m_layers;                 // 0x0C0 fixed pool of pre-allocated layers (AllocateNewLayer claims a free one)
		std::uint32_t        m_layerNameCount;         // 0x0C8 (== m_layerCount; runtime 100)
		std::uint32_t        m_pad0CC;                 // 0x0CC (likely capacity)
		BSFixedString*       m_layerNames;             // 0x0D0 per-layer debug names (set by AllocateNewLayer's functor)
		bool                 m_allocDisabled;          // 0x0D8 AllocateNewLayer no-ops while set
												       // members past 0x0D8 not yet mapped
	};
	static_assert(offsetof(BSInputEnableManager, m_cacheLock) == 0x088);
	static_assert(offsetof(BSInputEnableManager, m_layerLock) == 0x0A0);
	static_assert(offsetof(BSInputEnableManager, m_layerEventFlags) == 0x0B0);
	static_assert(offsetof(BSInputEnableManager, m_layers) == 0x0C0);
	static_assert(offsetof(BSInputEnableManager, m_layerNames) == 0x0D0);
}
