# Native translation registration

`BSScaleformTranslator` exposes the partial engine wrapper, its synchronous wide
resource loader, and `RegisterTranslations(span<Translation>)`. Names for the
parser interface describe its recovered ABI rather than recovered C++ symbols.

The API is based on Starfield 1.16.244.0. Register during construction, before the
wrapper is published to `BSScaleformManager` or the GFx loader. The map has no
proven reader/writer synchronization. Holding an implementation reference does
not retain the wrapper or table. `BSScaleformManager::GetTranslator()` is borrowed
and returns null while the constructor is loading resources.

For plugins hooking that lifecycle, the verified constructor ID is `130697`;
its call at `+0x4C6` targets loader `130926`. Call the original stock load before
registering custom entries. Hook installation, version/call-site checks, and
registration policy belong to the plugin. CommonLibSF does not install hooks.

```cpp
const RE::BSScaleformTranslator::Translation labels[] = {
    { L"$Example_Action", L"An exact label with a literal \\n" }
};
const bool loaded = constructingTranslator->RegisterTranslations(labels);
```

Keys start with `$` and cannot contain TAB, CR, LF or BOM. Values are C strings;
the engine owns pooled copies after this synchronous call returns. Duplicate keys
use the last value. Values preserve tabs, newlines, Unicode and literal `\n`.
Embedded NUL keeps native C-string truncation semantics.

Registration loads grammar-safe placeholders through the native resource loader,
then resolves the new table and assigns exact values through native pooled-string
assignment (`139356`). Node pointers never survive a load or rehash. The partial
table is not a default `BSTHashMap`: its native pointer hash differs. Failure is
reported without promising transactional rollback.

`ScaleformImpl::Translate` (`130928`) returns an owned string. Its ABI differs
from the GFx virtual translation entry, named `TranslateInfo` (`130927`).

`SettingsDataModel::BindingDefinition` and `GetBindingDefinitions()` expose the
native panel metadata as a borrowed const view. It can be empty before panel
creation and invalidated by rebuilds. Read it on the native settings/UI lane;
do not retain its elements. The internal publication visitor's stack captures
are deliberately not a public engine class.

Evidence is recorded in the workspace OSF RE modules
`ui.translation_registration.json` and `platform.control_map.json`. The former
records the prior three fresh prototype smoke runs and conflict/save/restart
checks. They are provenance for the extracted ABI, not runtime acceptance of
every later refactor. Live language/translator replacement remains unverified.

`xmake run commonlibsf-translation-tests` checks the library against stubbed native
boundaries, including chunked reads, reallocation, duplicates, exact values and
pooled ownership. OSF Settings' presentation checks exercise the actual patched
CALL instructions and constructor registration order. Neither is a game test.
