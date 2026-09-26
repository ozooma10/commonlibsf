#pragma once

#include "REX/W32/D3D12.h"
#include "REX/W32/DXGI.h"
#include "REX/W32/DXGI_2.h"

namespace RE::CreationRendererPrivate
{
	// NAMING / EVIDENCE. Starfield's renderer is Creation Engine 2's
	// `CreationRenderer` / `CreationRendererPrivate` — confirmed in the binary's
	// RTTI (e.g. `GeometryRenderPass@CreationRendererPrivate@@`) and in assert
	// strings (`CreationRendererPrivate::GeometryRenderPass::ExecuteRenderPass`).
	// There is NO `BSGraphics` namespace in Starfield (0 occurrences in the image;
	// that is a Skyrim/CommonLibSSE name).
	//
	// The specific plumbing structs below are NON-polymorphic, so the image holds
	// NO RTTI for them (verified by exhaustive search — RTTI IS retained for other
	// renderer types). The authentic names here therefore come from engine ASSERT
	// strings, not RTTI:
	//   * DeviceProperties + pDxDevice/pDXGIFactory/pDxActiveGPU — from asserts
	//     like `arDeviceProperties.pDxDevice->CheckFeatureSupport(...)` and
	//     `CreateDXGIFactory2(0, ..., &arDeviceProperties.pDXGIFactory)`.
	//   * the graphics queue — `...->CreateSwapChainForHwnd(pgraphicsQueue, hwnd, ...)`.
	// What has NO authentic name in either RTTI or strings is left as an explicit
	// placeholder and NOT invented: the renderer-root struct's type and the two
	// queue-owner hop structs. Evidence record: OSF RE context_repo module
	// `rendering.graphics_core` (runtime-proven on game 1.16.244, 2026-06-12).
	struct DeviceProperties;

	namespace detail
	{
		// PLACEHOLDER types for the graphics-queue pointer chain
		// (root+0x28 -> +0x08 -> +0x60). Only the offsets and the terminal
		// ID3D12CommandQueue are proven; the engine's own names for these two hop
		// structs are unknown (no RTTI, not in any string). Partial layouts.
		struct QueueOwnerA;
		struct QueueOwnerB;
	}

	// Accessor over the renderer-root global (`g_RendererRoot`, REL::ID 944397).
	// Its engine TYPE NAME IS UNKNOWN — the struct is non-polymorphic (no RTTI) and
	// is not named in any assert / __FUNCSIG__ string — so `Renderer` here is a
	// CommonLibSF accessor name, NOT a verified engine type. From the root the live
	// ID3D12Device and the DIRECT graphics queue that drives the swap chain are
	// reachable.
	//
	// Layout runtime-proven on game 1.16.244 (2026-06-12): pDxDevice QI's
	// ID3D12Device; the queue QI's ID3D12CommandQueue, reports
	// D3D12_COMMAND_LIST_TYPE_DIRECT, and queue->GetDevice() is COM-identical to
	// pDxDevice; pDxActiveGPU's LUID matches the device's LUID.
	//
	// Example:
	//   if (auto* r = RE::CreationRendererPrivate::Renderer::GetSingleton()) {
	//       ID3D12Device*       device = r->GetDevice();
	//       ID3D12CommandQueue* queue  = r->GetGraphicsQueue();
	//   }
	class Renderer
	{
	public:
		[[nodiscard]] static Renderer* GetSingleton()
		{
			static REL::Relocation<Renderer**> singleton{ ID::CreationRendererPrivate::Renderer::Singleton };
			return *singleton;
		}

		[[nodiscard]] REX::W32::ID3D12Device* GetDevice() const;

		[[nodiscard]] REX::W32::ID3D12CommandQueue* GetGraphicsQueue() const;

		[[nodiscard]] REX::W32::IDXGIFactory2* GetDXGIFactory() const;
		[[nodiscard]] REX::W32::IDXGIAdapter*  GetAdapter() const;

		// members
		std::byte            pad00[0x28];       // 00
		detail::QueueOwnerA* queueOwnerA;       // 28
		DeviceProperties*    deviceProperties;  // 30 (arDeviceProperties)
	};
	static_assert(offsetof(Renderer, queueOwnerA) == 0x28);
	static_assert(offsetof(Renderer, deviceProperties) == 0x30);


	// Renderer::pDeviceProperties (root + 0x30). Engine name: arDeviceProperties
	// (assert strings). Only the three terminal interface pointers are surfaced;
	// the real struct is ~0x590 and only partially mapped. Other authentic members
	// exist (e.g. pDxAllocator (D3D12MA) and pDeviceLostFence) at offsets not yet
	// proven — left out rather than guessed. Extend it, do not assume completeness.
	struct DeviceProperties
	{
		std::byte                pad000[0x408];  // 000
		REX::W32::IDXGIFactory2* dxgiFactory;    // 408 (pDXGIFactory)
		REX::W32::IDXGIAdapter*  dxActiveGPU;    // 410 (pDxActiveGPU)
		REX::W32::ID3D12Device*  dxDevice;       // 418 (pDxDevice)
	};
	static_assert(offsetof(DeviceProperties, dxgiFactory) == 0x408);
	static_assert(offsetof(DeviceProperties, dxActiveGPU) == 0x410);
	static_assert(offsetof(DeviceProperties, dxDevice) == 0x418);

	namespace detail
	{
		struct QueueOwnerA
		{
			std::byte    pad00[0x08];  // 00
			QueueOwnerB* inner;        // 08
		};
		static_assert(offsetof(QueueOwnerA, inner) == 0x08);

		struct QueueOwnerB
		{
			std::byte                     pad00[0x60];    // 00
			REX::W32::ID3D12CommandQueue* graphicsQueue;  // 60 (pgraphicsQueue)
		};
		static_assert(offsetof(QueueOwnerB, graphicsQueue) == 0x60);
	}

	inline REX::W32::ID3D12Device* Renderer::GetDevice() const
	{
		return deviceProperties ? deviceProperties->dxDevice : nullptr;
	}

	inline REX::W32::ID3D12CommandQueue* Renderer::GetGraphicsQueue() const
	{
		if (!queueOwnerA || !queueOwnerA->inner) {
			return nullptr;
		}
		return queueOwnerA->inner->graphicsQueue;
	}

	inline REX::W32::IDXGIFactory2* Renderer::GetDXGIFactory() const
	{
		return deviceProperties ? deviceProperties->dxgiFactory : nullptr;
	}

	inline REX::W32::IDXGIAdapter* Renderer::GetAdapter() const
	{
		return deviceProperties ? deviceProperties->dxActiveGPU : nullptr;
	}

	// RENDER-GRAPH PASS PLUMBING. A render pass's slot-7 ExecuteRenderPass is
	// `void(void* pass, GraphContext* graph, PassIO* io)`. Everything below is
	// non-polymorphic and unnamed in RTTI and assert strings: the type names are
	// descriptive CommonLibSF names, NOT engine names, and only the listed members
	// are mapped. Every pointer is borrowed for the duration of one Execute call.
	// Evidence: OSF RE context_repo module `rendering.ui_pass` (2026-09-25 notes),
	// runtime-proven on game 1.16.244 with frame generation OFF; FG and upscaler
	// graph variants have not been checked.
	struct CommandContext;

	namespace detail
	{
		// PLACEHOLDERS for unnamed hop structs; partial layouts.
		struct GraphBatch;          // GraphContext + 0x138
		struct CommandContextOwner;  // CommandContext + 0x30; may be DeviceProperties (unproven)
	}

	// 12-byte graph resource handle; GraphResourcePool::Resolve compares all 12 bytes.
	struct GraphResourceHandle
	{
		std::uint32_t words[3];  // 00
	};
	static_assert(sizeof(GraphResourceHandle) == 0xC);

	// One pass input/output. Only the leading handle is decoded.
	struct PassIOEntry
	{
		GraphResourceHandle handle;     // 00
		std::byte           pad0C[0x14];  // 0C
	};
	static_assert(sizeof(PassIOEntry) == 0x20);

	// Pass entry array. Entries are stored inline when capacity is negative,
	// otherwise behind the pointer at +0x10. Observed sizes: ScaleformBegin 3,
	// ScaleformEnd 1, ScaleformComposite 2.
	struct PassIOArray
	{
		[[nodiscard]] std::span<const PassIOEntry> GetEntries() const noexcept
		{
			const auto* data = capacity < 0 ?
				reinterpret_cast<const PassIOEntry*>(&storage) :
				reinterpret_cast<const PassIOEntry*>(storage);
			return { data, data ? size : 0 };
		}

		// members
		std::uint32_t  size;      // 00
		std::uint32_t  pad04;     // 04
		std::int32_t   capacity;  // 08
		std::uint32_t  pad0C;     // 0C
		std::uintptr_t storage;   // 10 (first inline entry, or the entry pointer)
	};
	static_assert(offsetof(PassIOArray, capacity) == 0x08);
	static_assert(offsetof(PassIOArray, storage) == 0x10);

	// The graph's current resource pool. Records are 0xD8 bytes.
	struct GraphResourcePool
	{
		// Returns the record's borrowed payload (no AddRef), or null when no record
		// matches. For texture IO entries the payload is a GraphRenderTarget; other
		// IO roles are not decoded.
		[[nodiscard]] void* Resolve(const GraphResourceHandle& a_handle)
		{
			using func_t = void* (*)(GraphResourcePool*, const GraphResourceHandle*);
			static REL::Relocation<func_t> func{ ID::CreationRendererPrivate::GraphResourcePool::Resolve };
			return func(this, &a_handle);
		}

		// members
		std::byte     pad00[0x08];  // 00
		std::uint32_t recordCount;  // 08
		std::uint32_t pad0C;        // 0C
		std::byte*    records;      // 10
	};
	static_assert(offsetof(GraphResourcePool, recordCount) == 0x08);
	static_assert(offsetof(GraphResourcePool, records) == 0x10);

	// Execute's third argument.
	struct PassIO
	{
		PassIOArray*       entries;  // 00
		GraphResourcePool* pool;     // 08
	};
	static_assert(offsetof(PassIO, pool) == 0x08);

	struct GraphTexture
	{
		std::byte                 pad00[0x38];  // 00
		REX::W32::ID3D12Resource* resource;     // 38
	};
	static_assert(offsetof(GraphTexture, resource) == 0x38);

	// Resolved payload of a color-target IO entry.
	struct GraphRenderTarget
	{
		std::byte     pad00[0x08];     // 00
		void*         rtvDescriptors;  // 08 (*this is the CPU RTV the native pass binds)
		std::byte     pad10[0x04];     // 10
		std::uint32_t width;           // 14
		std::uint32_t height;          // 18
		std::byte     pad1C[0x3C];     // 1C
		GraphTexture* texture;         // 58
	};
	static_assert(offsetof(GraphRenderTarget, rtvDescriptors) == 0x08);
	static_assert(offsetof(GraphRenderTarget, width) == 0x14);
	static_assert(offsetof(GraphRenderTarget, height) == 0x18);
	static_assert(offsetof(GraphRenderTarget, texture) == 0x58);

	// Execute's second argument.
	struct GraphContext
	{
		// Acquires and initializes a context on a cache miss, so it MUTATES graph
		// state: call it only where a native pass would.
		[[nodiscard]] CommandContext* GetOrCreateCommandContext()
		{
			using func_t = CommandContext* (*)(GraphContext*);
			static REL::Relocation<func_t> func{ ID::CreationRendererPrivate::GraphContext::GetOrCreateCommandContext };
			return func(this);
		}

		// The context a native pass already opened, or null. Side-effect free; on
		// a hit GetOrCreateCommandContext returns this same pointer.
		[[nodiscard]] CommandContext* GetOpenCommandContext() const noexcept;

		// members
		std::byte          pad000[0x138];  // 000
		detail::GraphBatch* batch;         // 138
	};
	static_assert(offsetof(GraphContext, batch) == 0x138);

	struct CommandContext
	{
		// The engine's own reset: zeros cachedBindings and, unless the command pool
		// flags byte (+0x20) has bits other than 0x02 set, rebinds the native
		// descriptor heap pair. Unique static proof; not yet exercised at runtime.
		void InvalidateCachedBindingsAndBindHeaps()
		{
			using func_t = void (*)(CommandContext*);
			static REL::Relocation<func_t> func{ ID::CreationRendererPrivate::CommandContext::InvalidateCachedBindingsAndBindHeaps };
			func(this);
		}

		// members
		void*                                cachedBindings[6];  // 00 (root/layout, PSO, four binding groups)
		detail::CommandContextOwner*         owner;              // 30
		void*                                commandPool;        // 38
		std::byte                            pad40[0x20];        // 40
		REX::W32::ID3D12GraphicsCommandList* commandList;        // 60
	};
	static_assert(offsetof(CommandContext, owner) == 0x30);
	static_assert(offsetof(CommandContext, commandPool) == 0x38);
	static_assert(offsetof(CommandContext, commandList) == 0x60);

	namespace detail
	{
		struct GraphBatch
		{
			std::byte       pad000[0x120];  // 000
			CommandContext* cachedContext;  // 120
		};
		static_assert(offsetof(GraphBatch, cachedContext) == 0x120);
	}

	inline CommandContext* GraphContext::GetOpenCommandContext() const noexcept
	{
		return batch ? batch->cachedContext : nullptr;
	}
}
