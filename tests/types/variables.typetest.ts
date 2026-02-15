import type { TesseractInstance } from "../../lib/index";

declare const t: TesseractInstance;

// valid debug variable usage
t.setDebugVariable("dawg_debug_level", "1");
t.setDebugVariable("debug_file", "/tmp/tess-debug.log");

// @ts-expect-error debug names only
t.setDebugVariable("tessedit_char_whitelist", "abc");
// @ts-expect-error debug values are stringly typed
t.setDebugVariable("dawg_debug_level", 1);

// valid setVariable usage
t.setVariable("tessedit_char_whitelist", "abc");
t.setVariable("textord_dotmatrix_gap", "3");
t.setVariable("repair_unchopped_blobs", "1");

// @ts-expect-error debug name is excluded from setVariable
t.setVariable("dawg_debug_level", "1");
// @ts-expect-error init-only name is excluded from setVariable
t.setVariable("load_system_dawg", "1");

// typed getters
t.getIntVariable("textord_dotmatrix_gap");
t.getDoubleVariable("textord_dotmatrix_gap");
t.getBoolVariable("repair_unchopped_blobs");
t.getStringVariable("tessedit_char_whitelist");

// @ts-expect-error bool name not allowed for int getter
t.getIntVariable("repair_unchopped_blobs");
// @ts-expect-error non-bool name not allowed for bool getter
t.getBoolVariable("textord_dotmatrix_gap");
// @ts-expect-error string name not allowed for numeric getter
t.getDoubleVariable("tessedit_char_whitelist");
// @ts-expect-error numeric name not allowed for string getter
t.getStringVariable("textord_dotmatrix_gap");
