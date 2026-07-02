#pragma once

#include "RE/IDs.h"

namespace RE
{
	// Engine-cursor / OS-cursor-capture state (partial layout; evidence in
	// context_repo module ui.menu_cursor, proven on 1.16.244). A static
	// instance is published through the ID::MenuCursor::Singleton data slot.
	//
	// The per-frame windowing update (fn 99486) reads this object to decide
	// the OS-cursor capture: freeCursorRefCount == 0 -> ClipCursor to a
	// zero-area rect at screen center (the gameplay pin; there is no
	// per-frame SetCursorPos — the clip alone holds the hardware cursor);
	// freeCursorRefCount > 0 with the clipInset floats zero ->
	// ClipCursor(NULL) (OS cursor fully released). warpRequested is a
	// one-shot: consumed and cleared by the same function, which then warps
	// the OS cursor to (cursorPosX/Y - cursorPosOffX/Y) via
	// ClientToScreen + SetCursorPos (menu-open centering).
	//
	// The only engine writers of freeCursorRefCount are CursorMenu's
	// ProcessMessage kShow (increment) / kHide (decrement) — there is no
	// per-frame recompute, so a plugin may take its own reference on the
	// game thread to release the OS cursor WITHOUT summoning the CursorMenu
	// Scaleform arrow (live-proven 2026-07-02; balance your own +1/-1).
	class MenuCursor
	{
	public:
		[[nodiscard]] static MenuCursor* GetSingleton()
		{
			static REL::Relocation<MenuCursor**> singleton{ ID::MenuCursor::Singleton };
			return *singleton;
		}

		// members
		std::byte     pad000[0x60];         // 000 — vptr at +0x00, BSInputEventReceiver-style subobject at +0x10 (translated-input receiver, module platform.input_windowing)
		float         cursorPosX;           // 060 — engine cursor position (screen px)
		float         cursorPosY;           // 064
		float         cursorPosOffX;        // 068 — subtracted from pos for the OS-cursor warp
		float         cursorPosOffY;        // 06C
		std::byte     pad070[0x08];         // 070 — +0x74/+0x76 input-handler state bytes
		float         clipInsetLeft;        // 078 — normalized clip-inset rect; all zero = no sub-rect clip
		float         clipInsetTop;         // 07C
		float         clipInsetRight;       // 080
		float         clipInsetBottom;      // 084
		std::byte     pad088[0x08];         // 088
		std::uint32_t freeCursorRefCount;   // 090 — >0 releases the OS-cursor clip (CursorMenu kShow/kHide; plugin-referenceable)
		bool          warpRequested;        // 094 — one-shot: warp the OS cursor to the engine cursor next frame
		bool          cursorActive;         // 095 — CursorMenu kShow=1 / kHide=0
	};
	static_assert(offsetof(MenuCursor, freeCursorRefCount) == 0x90);
	static_assert(offsetof(MenuCursor, warpRequested) == 0x94);
}
