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
}
