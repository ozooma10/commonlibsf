#pragma once

#include "RE/B/BSLock.h"

namespace RE
{
	// The engine's native "run this once on the game thread" primitive: the
	// BSService command stream embedded at Main+0x40. Producers on any thread
	// post a delegate through TaskQueue::QueueTask; the frame loop drains the
	// stream once per frame on the game thread under an fTaskQueueBudgetMS
	// (default 5 ms) budget. This is Starfield's equivalent of the FO4/SSE
	// TaskQueueInterface / BSTaskPool UI-task path.
	//
	// The BSService namespace itself is RTTI-proven (BSService::ICommand,
	// BSService::Detail::TCmd<...>); the class names below are authored — the
	// engine's own names for the delegate base and the stream holder are
	// unknown. Layout and behavior runtime-proven on 1.16.242 (TaskQueueProbe,
	// 2026-06-10).
	namespace BSService
	{
		// Ref-counted delegate consumed by the task queue. Subclass and
		// override Run(); construct with new and hand ownership to
		// TaskQueue::AddTask. The work executes when the queued reference is
		// released to zero — on the game thread during the per-frame drain on
		// the normal path, or inline on the caller's thread when queueing is
		// disabled (early boot) or the caller already is the draining thread.
		class QueuedDelegate
		{
		public:
			virtual ~QueuedDelegate() = default;  // 00 - never invoked by the engine on the queued path

			// 01 - the engine's entry point: called when the last reference is
			// released (via the TCmd invoke thunk on the draining thread). Must
			// dispose of the delegate before returning.
			virtual void RunAndDispose()  // 01
			{
				Run();
				delete this;
			}

			virtual void Unk02() {}  // 02 - never observed
			virtual void Unk03() {}  // 03 - never observed

			// 04 - title-profiler context getter; must return null so the
			// profiler path is skipped.
			virtual void* GetProfilerContext() { return nullptr; }  // 04

			virtual void Unk05() {}  // 05 - defensive padding, never observed
			virtual void Unk06() {}  // 06
			virtual void Unk07() {}  // 07

			// >= 08: beyond the engine's vtable - our extension slot.
			virtual void Run() = 0;

			std::uint32_t IncRef() const
			{
				REX::TAtomicRef myRefCount{ refCount };
				return ++myRefCount;
			}

			// Mirrors the engine call sites: dropping the last reference runs
			// the delegate (and destroys it) on the current thread.
			void Release() const
			{
				REX::TAtomicRef myRefCount{ refCount };
				if (--myRefCount == 0) {
					const_cast<QueuedDelegate*>(this)->RunAndDispose();
				}
			}

			// members
			mutable volatile std::uint32_t refCount{ 1 };       // 08 - QueueTask steals one reference on enqueue
			std::uint32_t                  pad0C{ 0 };          // 0C
			std::byte                      pad10[0x18]{};       // 10
			BSReadWriteLock                lock;                // 28
			const void*                    profilerCategory{};  // 30 - ptr to a profiler category table (engine reads [ptr+8]); keep null
			std::byte                      pad38[0xE8]{};       // 38 - profiler ptrs @ 110/118 must stay zero
		};
		static_assert(offsetof(QueuedDelegate, refCount) == 0x08);
		static_assert(offsetof(QueuedDelegate, lock) == 0x28);
		static_assert(offsetof(QueuedDelegate, profilerCategory) == 0x30);
		static_assert(sizeof(QueuedDelegate) == 0x120);

		namespace detail
		{
			template <class Fn>
			class QueuedFunctorDelegate :
				public QueuedDelegate
			{
			public:
				explicit QueuedFunctorDelegate(Fn a_fn) :
					_fn(std::move(a_fn))
				{}

				void Run() override { _fn(); }

			private:
				Fn _fn;
			};
		}

		// Holder of the main-thread command stream ([this+0] is the stream at
		// Main+0x40). Reached through a global pointer, not UI() or Main.
		class TaskQueue
		{
		public:
			[[nodiscard]] static TaskQueue* GetSingleton()
			{
				static REL::Relocation<TaskQueue**> singleton{ ID::BSService::TaskQueue::Singleton };
				return *singleton;
			}

			// Raw engine call. STEALS the caller's reference when it enqueues
			// (a_task is nulled); when queueing is disabled or the caller is
			// the thread currently draining, the reference is left and the
			// caller's own Release() runs the delegate inline. Prefer AddTask.
			void QueueTask(QueuedDelegate*& a_task)
			{
				using func_t = void (*)(TaskQueue*, QueuedDelegate**);
				static REL::Relocation<func_t> func{ ID::BSService::TaskQueue::QueueTask };
				func(this, std::addressof(a_task));
			}

			// Fire-and-forget: takes ownership of a_task (refCount 1, from
			// new). Runs once on the game thread during the next budgeted
			// drain, or inline right away on the fallback paths.
			//
			// NOTE: code already executing inside the drain (queued tasks,
			// engine command handlers such as console commands) always hits
			// the inline fallback — the task runs synchronously before AddTask
			// returns, and nested posts cannot defer to a later drain. To
			// force next-drain execution from such a context, post from a
			// different thread.
			void AddTask(QueuedDelegate* a_task)
			{
				if (!a_task) {
					return;
				}
				QueueTask(a_task);
				if (a_task) {
					a_task->Release();
				}
			}

			template <class Fn>
				requires(std::invocable<std::remove_cvref_t<Fn>&>)
			void AddTask(Fn&& a_fn)
			{
				AddTask(new detail::QueuedFunctorDelegate<std::remove_cvref_t<Fn>>(std::forward<Fn>(a_fn)));
			}
		};
	}
}
