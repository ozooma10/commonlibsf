# IMenu priority and input contexts

These fields and the append path were checked against the installed
Starfield 1.16.244.0 executable. The matching Address Library maps ID 121667
to `0x1421EA260`.

| Offset | Declaration | Evidence |
| --- | --- | --- |
| `0xD1` | `bool inputContextsChanged` | PauseMenu writes byte `1` after each requested-context append. |
| `0xD8` | `InputContextStorage inputContexts` | Three pointers at `0xD8`, `0xE0`, and `0xE8`: begin, end, capacity end. Elements are one-byte `ControlMap::InputContextID` values. |
| `0x110` | `std::uint8_t depthPriority` | PauseMenu writes byte `0x0B`; the active-menu array sorts by this byte. |

The neighboring unknown bytes remain represented separately. Header assertions
check the offsets, the storage size, and its trivial destruction.

## Usage

Configure the priority and requested contexts in `PostCreate()`, before stacking:

```cpp
depthPriority = 0x0C;  // One above PauseMenu.
AddInputContext(InputContextID::kBasicMenuNav);
AddInputContext(InputContextID::kLeftThumbstick);
AddInputContext(InputContextID::kVirtualController);
```

`AddInputContext` appends in order and sets `inputContextsChanged`. It preserves
the native append behavior, including allowing duplicate entries. It does not
push directly onto ControlMap's active context stack. The engine applies the
requested list.

`GetInputContexts()` returns a read-only span into the engine-owned storage.
Do not retain it across list mutations or menu destruction. These helpers do
not synchronize concurrent access.

`IsInputContextAppendSupported()` preserves the plugin's existing registration
check of the growth routine's first 13 bytes. It is a prologue check, not proof
of compatibility with another game version.

## Input propagation flag

`IMenu::kBlocksLowerMenuInput` replaces `Flag4`, keeping its value `1 << 4`.
The name describes the behavior found in static analysis of Starfield 1.16.244:
`UI::PerformInputProcessing` marks an event handled at the end of a menu's
iteration when this bit is set, preventing delivery to lower menus. Cursor
position forwarding also stops at this bit. The supporting research is in
the OSF RE context module `ui.menu_input`.

This is a descriptive name, not a recovered Bethesda symbol. The separate
bit 8 (`kModal`) selects the top application-modal menu. The reduced flag set
used by OSF Settings still needs an in-game input and open/close check.

## Native allocation and lifetime

PauseMenu's initializer at `0x141668C60` appends context bytes `0x02`, `0x0B`,
and `0x03`. With spare capacity it writes through the end pointer and advances
that pointer. Otherwise it calls `0x1421EA260` with the vector address, insertion
position, and address of the new byte. Our helper follows both paths.

The growth routine allocates through `0x1402BA260` and frees through
`0x1402BA340`, passing the old capacity to the latter. This pair includes a
size-dependent pool path. Consequently, the menu helper retains native growth
instead of using `BSTHeapSTLVector`'s generic `RE::malloc`/`RE::free` mutation.

IMenu's native destructor frees the list at `0x142551999`–`0x1425519C2`, using
the same size-aware free routine, then clears all three pointers. The C++
`InputContextStorage` supplies no allocator or destructor.

## PostCreate (virtual slot 0x07)

`PostCreate` replaces the placeholder `Unk07`. It is a descriptive name for
post-creation menu setup, not a recovered Bethesda source symbol.

On the open path, UI resolves the menu instance, invokes primary virtual slot
`0x07` at `0x14254089A`, then calls the stack-admission routine
`0x1425417A0` (ID 130449) at `0x1425408AC`. Another call to the same menu slot
appears at `0x142540FEE` in UI's setup processing.

PauseMenu's primary vtable is `0x144C909A8`; entry `+0x38` points to
`0x141668C60`, the initializer described above. Besides priority and input
contexts, that override initializes menu data and registers event listeners.
The name therefore describes general menu setup rather than only input setup.
The base declaration remains a no-op in the same virtual slot.

This callback precedes admission on the verified open path; it does not mean
that the menu has already joined the stack. Use `OnAddedToMenuStack()` for that
notification. No once-only or all-path scheduling guarantee is inferred here.

Validation covers static disassembly and compilation with layout assertions;
the updated plugin still needs an in-game open/close check.

## Menu factory signature

`UI::UIMenuEntry::Create_t` returns `Scaleform::Ptr<IMenu>` by value with no
explicit parameters. MSVC's x64 ABI passes hidden return storage in `RCX` for
this non-trivial type and returns the same storage address in `RAX`.

In Starfield 1.16.244, the factory call at `0x142540D2C` receives the address of
a stack temporary in `RCX`, then the caller reads the menu pointer through
`RAX`. The native factory at `0x141666E80` similarly saves `RCX`, constructs a
menu, writes its pointer into that storage, and returns the storage address.
The former explicit output-pointer signature described this ABI mechanically;
returning the smart pointer lets the compiler supply it.

The MSVC 14.51 releasedbg build of `OSFSettingsMenu::Create()` confirms the
same convention: it saves `RCX` in `RDI`, stores the menu at `[RDI]`, and
returns `RDI` in `RAX`. The DLL builds successfully; runtime testing remains.
