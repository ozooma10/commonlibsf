# Native control-map construction

The smallest recovered entry point that constructs mappings from definitions is
`ControlMap::ParseMappings(const char*)`. Construction of an individual action
is inlined inside that function. No complete single-action registration call
was found in the inspected construction path.

CommonLibSF exposes these native operations, with descriptive recovered names:

| Method | Address Library ID | RVA | Behavior |
| --- | --- | --- | --- |
| `FormatMappingRow(event, tokens, ...)` | 124100 | 0x022DB400 | Formats one defaults row using native binding-token strings. |
| `FormatMappingRow(event, codes, ...)` | 124101 | 0x022DB5E0 | Formats numeric device codes, then calls 124100. |
| `LoadMappings()` | 124116 | 0x022EA8F0 | Builds the baked default text, calls the parser, releases the text. |
| `ParseMappings(text)` | 124117 | 0x022EAA80 | Clears existing mappings and constructs contexts, device mappings, group indexes and link records from complete defaults. |
| `ResolveLinkedMappings()` | 124124 | 0x022ED270 | Resolves inherited bindings, updates affected mappings and restores their ordering. |

Loading, parsing and resolving are initialization/reset primitives. Loading or parsing invalidates
existing mapping/context views and replaces player bindings. Parsing does not
apply saved overrides or resolve inherited bindings. They are not an API for
adding one action to a running map, and `LoadMappings` is not a complete Controls
menu reset (the menu performs additional notification and UI work).

The native constructor performs:

```text
LoadMappings
    Build baked default text
    ParseMappings
Load saved overrides, if the main context exists
ResolveLinkedMappings
```

The native Controls reset calls `LoadMappings`, then `ResolveLinkedMappings`,
before its notification/device/UI work. When intercepting the existing defaults
parser call, let the original caller continue that sequence. Do not add a second
finalization call inside the parser hook.

The plugin integration installs a call-site hook at `LoadMappings + 0x45`
(RVA `0x022EA935`). It prefixes native-formatted schema rows to the original
defaults, then calls the original parser once with the same `ControlMap`.
The parser starts in MainGameplay (context zero), so the rows join that context
without changing any existing separators or later contexts. The defaults loader
is also used by Controls reset, which reintroduces the declarations naturally.

`SFSE::InputMap::GetKeyboardVirtualKey` reads the engine's baked UTF-16 keyboard
table (ID 361050, RVA `0x04A56010`), allowing default names to be resolved before
the keyboard device exists. This is a table lookup helper, not a native function
wrapper. It returns `0xFFFFFFFF` for unknown names. The table and the call-site
bytes (`E8 46 01 00 00`, targeting the parser) were rechecked in the installed
1.16.244.0 executable.

## Formatting a row

`FormatMappingRow` calls the same formatter used by the engine's baked-defaults
builder. It returns an owned `std::string` containing a tab-separated row with
the native line ending. It does not access the `ControlMap` singleton or change
any mappings.

The arguments are the event name, keyboard/mouse/gamepad bindings, one Controls
visibility flag per device, control mask, group mask and required flag. String
arguments must be NUL-terminated. The token overload preserves native token
syntax; the numeric overload lets the engine format the codes as hexadecimal.
Keyboard codes are virtual-key codes, and `0xFF` denotes an unbound device.

For example, this formats an F10 keyboard row:

```cpp
const auto row = RE::ControlMap::FormatMappingRow(
    "osfsettings/openMenu", 0x79u, 0xFFu, 0xFFu,
    true, false, false, 0x401u, 0u, false);
```

The engine returns a 16-byte temporary string with a storage discriminator at
offset `0xC`: values at most 12 select inline storage, larger values select the
pointer at offset zero. This differs from the existing `BSStringT` layout.
The wrapper keeps that representation private, copies the text into the return
value and invokes native destructor 35706 (RVA `0x002B7B60`) through a scoped
owner. No engine allocation is passed to the C++ allocator for release.

## Why the smaller candidates are insufficient

- The mapping constructor (124118, RVA 0x022EBDF0) initializes one record: retained
  default event string, unbound key/chord, slot, masks and flags. It takes only
  the record address and never receives a `ControlMap` or context. It does not
  insert or register anything.
- Device-token parsing (124119, RVA 0x022EBE50) produces temporary key/slot tokens
  and link metadata. The caller still supplies visibility, slot assignment,
  group membership and insertion.
- Generic array insertion (123860, RVA 0x022CC160) makes space at an index chosen
  by the caller. Within the defaults parser, the signed key/chord binary search
  is inline at RVA 0x022EBAC2; the generic insertion call is at 0x022EBB5A;
  string retention and record copying follow at 0x022EBB7D. This is not a native
  `AddAction` function.
- Group-index construction at RVA 0x022EB75E..0x022EB956 is also in the parser.
  Appending a public `UserEventMapping` would bypass these construction steps.

The existing public mapping/context layouts are partial views. This change
does not turn them into a complete writable construction interface. In
particular, the existing `defaultWasUnbound` name at mapping +0x1E is incomplete:
the parser sets it for the 0x7FFFFFFF inherited-binding sentinel, not ordinary
0xFF unbound keys.

## Evidence and verification

Rechecked on 2026-09-16 against installed Starfield **1.16.244.0** and matching
`versionlib-1-16-244-0.bin`. The installed executable and the unpacked image used
by the saved Ghidra decompilation both have SHA-256:

```text
7e9adb1414a8e1b325e5e1f097b9b17b78deb7eebeda37a333351a43a60f9d28
```

Fresh Address Library queries confirmed the IDs above. Fresh disassembly
confirmed the mapping constructor, parser insertion path and these native edges:

- Defaults loader: RVA 0x022EA935 calls the parser.
- ControlMap constructor: RVAs 0x022EA6FD, 0x022EA70C and 0x022EA714 call the
  defaults loader, saved-override loader and link resolver respectively.
- Controls reset: RVAs 0x0151A125 and 0x0151A12D call the loader and resolver.

The byte-matched Ghidra outputs were read from the local OSF RE checkout under
`tools/ghidra/out/native-hotkeys-20260915/`: `0x1422eaa80.c`, `0x1422ea8f0.c`,
`0x1422ea4c0.c`, `0x1422ebe50.c`, `0x1422ed270.c`, and `0x14151a100.c`.
The parser clears prior state before processing rows and implements the
construction logic in its own body. This establishes the boundary for this
path; it is not an exhaustive proof that no other registration path exists.

Fresh read-only Ghidra decompilation and disassembly also confirmed both row
formatters and their temporary-string destructor. Formatter arguments are the
output address followed by the ten public arguments, with byte-sized flags and
32-bit codes/masks; both return the output address. The numeric overload calls
the token overload at RVA `0x022DB6B7`. Ghidra's `.text` SHA-256 matched the
executable (`d57f792c1319e2c4e4b0e96be4f39eda77fdfe2f53ff9e819c86d79131e292fa`).
The decompiler mislabeled the formatter's `sprintf_s` buffers as wide strings;
the disassembly and literal bytes establish narrow `%#x` formatting.

The focused tests exercise real CommonLibSF wrappers with substitute native
entry points. They check relocation IDs, receiver/text forwarding and the
separation of loading, parsing and finalization. Formatter checks cover every
argument and copying/releasing inline and heap-backed native output. All eight
checks pass. They do not execute Starfield's formatter/parser or prove in-game
registration, rebinding, reset or persistence.

From the CommonLibSF checkout:

```text
xmake build --project=. commonlibsf-control-map-tests
xmake run --project=. commonlibsf-control-map-tests
```

The plugin's `osfsettings-registration-tests` target executes its real hook
against a synthetic x64 loader with the same call-site offset. It substitutes
schema data, the formatter, keyboard table and original parser. Twelve checks
cover lookup, schema defaults, unbound/invalid declarations, namespacing, native
flags, preservation of every vanilla byte, one parser call per load/reset, and
repeat installation. These are host tests, not in-game registration proof.

The next runtime check is a fresh startup showing `osfsettings/openMenu` in
`GetMappings(kMainGameplay, kKeyboard)` with VK `0x79`, followed by a Controls
reset with one definition still present. Activation and localized Controls
labels are outside this registration checkpoint.
