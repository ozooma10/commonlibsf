#pragma once

#include "RE/B/BSTArray.h"
#include "RE/B/BSTSingleton.h"

#include <cstdint>
#include <cstring>

namespace RE
{
	enum class BSEventNotifyControl : std::uint32_t
	{
		kContinue,
		kStop
	};

	using EventResult = BSEventNotifyControl;

	namespace BSTEventDetail
	{
		class SinkBase
		{
		public:
			virtual ~SinkBase() = 0;
		};

		class SourceBase
		{
		public:
			virtual ~SourceBase() = 0;
		};

		inline SinkBase::~SinkBase() = default;
		inline SourceBase::~SourceBase() = default;
	}

	template <class>
	class BSTEventSource;

	// A typed acquiring getter plus the identities needed to validate its result.
	// Use source ID 0 when the global source's exact address is not known.
	template <class Event>
	class BSTEventSourceBinding
	{
	public:
		constexpr BSTEventSourceBinding(REL::ID a_getter, REL::ID a_vtable, REL::ID a_source = REL::ID{ 0 }) noexcept :
			_getter(a_getter), _vtable(a_vtable), _source(a_source)
		{}

		[[nodiscard]] constexpr REL::ID GetGetterID() const noexcept { return _getter; }
		[[nodiscard]] constexpr REL::ID GetVtableID() const noexcept { return _vtable; }
		[[nodiscard]] constexpr REL::ID GetSourceID() const noexcept { return _source; }

		[[nodiscard]] BSTEventSource<Event>* Get() const
		{
			using func_t = BSTEventSource<Event>* (*)();
			REL::Relocation<func_t> func{ _getter };
			return func();
		}

		[[nodiscard]] bool Matches(const BSTEventSource<Event>* a_source) const
		{
			if (!a_source || (_source.id() != 0 && reinterpret_cast<std::uintptr_t>(a_source) != _source.address())) {
				return false;
			}

			std::uintptr_t vtable = 0;
			std::memcpy(&vtable, a_source, sizeof(vtable));
			return vtable == _vtable.address();
		}

	private:
		REL::ID _getter;
		REL::ID _vtable;
		REL::ID _source;
	};

	template <class Event>
	class BSTEventSink : public BSTEventDetail::SinkBase
	{
	public:
		~BSTEventSink() override = default;  // 00

		// add
		virtual BSEventNotifyControl ProcessEvent(const Event& a_event, BSTEventSource<Event>* a_source) = 0;  // 01
	};
	static_assert(sizeof(BSTEventSink<void*>) == 0x8);

	template <class Event>
	class BSTEventSource : public BSTEventDetail::SourceBase
	{
	public:
		~BSTEventSource() override = default;  // 00

		// The engine's Notify takes a type-erased functor, NOT the event itself:
		// the dispatcher invokes visitor->vtbl[2](visitor, sink) for every
		// registered sink. Passing the raw event here (the old void* signature)
		// treated the event's first qword as a vtable.
		void Notify(const Event& a_event)
		{
			NotifyVisitor visitor{ &a_event, this };
			using func_t = void (*)(BSTEventSource*, void*);
			static REL::Relocation<func_t> func{ ID::BSTEventSource::Notify };
			func(this, &visitor);
		}

		void RegisterSink(BSTEventSink<Event>* a_sink)
		{
			using func_t = decltype(&BSTEventSource::RegisterSink);
			static REL::Relocation<func_t> func{ ID::BSTEventSource::RegisterSink };
			return func(this, a_sink);
		}

		void UnregisterSink(BSTEventSink<Event>* a_sink)
		{
			using func_t = decltype(&BSTEventSource::UnregisterSink);
			static REL::Relocation<func_t> func{ ID::BSTEventSource::UnregisterSink };
			return func(this, a_sink);
		}

	private:
		// Mirrors the engine's stl::local_function visitor layout. The notify
		// dispatcher only ever calls vtable slot 2 (Invoke) on this object;
		// slots 0/1 are never used on the notify path (1.16.244 dispatcher
		// 0x1422C91C0, osf-re systems.events).
		struct NotifyVisitor
		{
			NotifyVisitor(const Event* a_event, BSTEventSource* a_source) noexcept :
				event(a_event), source(a_source)
			{}

			virtual ~NotifyVisitor() = default;  // 00

			virtual void Unk01() {}  // 01 - reserved, never called during Notify

			virtual BSEventNotifyControl Invoke(BSTEventDetail::SinkBase* a_sink)  // 02
			{
				return static_cast<BSTEventSink<Event>*>(a_sink)->ProcessEvent(*event, source);
			}

			// members
			const Event*    event;   // 08
			BSTEventSource* source;  // 10
		};

	public:
		// members
		BSTArray<BSTEventSink<Event>> sinks;  // 08
		std::uint32_t                 unk18;  // 18
		std::uint32_t                 unk1C;  // 1C
		std::uint32_t                 unk20;  // 20
		std::uint32_t                 unk24;  // 24
	};
	static_assert(sizeof(BSTEventSource<void*>) == 0x28);

	class BSTGlobalEvent
	{
	public:
		template <class Event>
		class EventSource :
			public BSTSingletonImplicit<EventSource<Event>>,
			public BSTEventSource<Event>
		{
		public:
			~EventSource() override;  // 00

			virtual BSEventNotifyControl ProcessEvent(const Event& a_event, BSTEventSource<Event>* a_source);  // 01
		};

		virtual ~BSTGlobalEvent();  // 00
	};
	static_assert(sizeof(BSTGlobalEvent) == 0x08);

	template <class Event>
	class BSTEventSourceLazyInit
	{
	public:
		// members
		BSTEventSource<Event>* eventSource;  // 00
	};
	static_assert(sizeof(BSTEventSourceLazyInit<void*>) == 0x8);
}
