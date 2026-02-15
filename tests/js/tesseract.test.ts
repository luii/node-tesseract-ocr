/*
 * Copyright 2026 Philipp Czarnetzki
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express
 * or implied. See the License for the specific language governing
 * permissions and limitations under the License.
 */

import { readFileSync } from "node:fs";
import { mkdtemp, rm } from "node:fs/promises";
import os from "node:os";
import path from "node:path";
import { fileURLToPath } from "node:url";

import { afterEach, beforeEach, describe, expect, it, vi } from "vitest";

import Tesseract, {
  Language,
  PageSegmentationModes,
  TesseractInstance,
} from "../../lib/index";

const exampleImageUrl = new URL("../../example8.jpg", import.meta.url);
const exampleImage = readFileSync(fileURLToPath(exampleImageUrl));
const exampleImagePath = fileURLToPath(exampleImageUrl);

describe("tesseract api validation", () => {
  let tesseract: TesseractInstance;
  beforeEach(() => {
    tesseract = new Tesseract();
  });

  afterEach(async () => {
    await tesseract?.end();
  });

  it("returns the current tesseract version", async () => {
    await expect(tesseract.version()).resolves.toMatch(/^\d+\.\d+\.\d+/);
  });

  it("tracks initialized state across init/end", async () => {
    const local = new Tesseract();
    await expect(local.isInitialized()).resolves.toBe(false);
    await local.init({ ensureTraineddata: false });
    await expect(local.isInitialized()).resolves.toBe(true);
    await local.end();
    await expect(local.isInitialized()).rejects.toMatchObject({
      code: "ERR_WORKER_CLOSED",
    });
  });

  it("accepts init without options", async () => {
    await expect(
      tesseract.init({ ensureTraineddata: false }),
    ).resolves.toBeUndefined();
  });

  it("skips sparse languages (e.g. `undefined`) in `langs`", async () => {
    await expect(
      // @ts-expect-error - sparse languages array
      tesseract.init({ langs: [, "eng"], ensureTraineddata: false }),
    ).resolves.toBeUndefined();
    await expect(tesseract.getInitLanguages()).resolves.toBe("eng");
  });

  it("rejects init with invalid lang type", async () => {
    // @ts-expect-error - testing runtime validation for invalid type
    await expect(tesseract.init({ langs: 123 })).rejects.toThrow(
      "options.langs is not iterable",
    );
  });

  it("rejects init with invalid oem type", async () => {
    // @ts-expect-error - testing runtime validation for invalid type
    await expect(tesseract.init({ oem: "x" })).rejects.toThrow(
      "init(options): options.oem must be a number",
    );
  });

  it("rejects init with unsupported oem value", async () => {
    // @ts-expect-error - testing runtime validation for invalid type
    await expect(tesseract.init({ oem: 999 })).rejects.toThrow(
      "init(options): options.oem is out of supported range",
    );
  });

  it("rejects init when only a vars key is provided", async () => {
    await expect(
      tesseract.init({ vars: { allow_blob_division: undefined } }),
    ).rejects.toThrow("init(options): options.vars must contain only strings");
  });

  it("rejects setInputName with invalid type", async () => {
    // @ts-expect-error - testing runtime validation for invalid type
    await expect(tesseract.setInputName(123)).rejects.toThrow(
      "setInputName(inputName?): inputName must be a string",
    );
  });

  it("rejects setInputImage with invalid type", async () => {
    // @ts-expect-error - testing runtime validation for invalid type
    await expect(tesseract.setInputImage("x")).rejects.toThrow(
      "setInputImage(buffer?): buffer must be a Buffer",
    );
  });

  it("rejects setInputImage with empty buffer", async () => {
    await expect(tesseract.setInputImage(Buffer.alloc(0))).rejects.toThrow(
      "setInputImage(buffer?): buffer is empty",
    );
  });

  it("rejects setOutputName with invalid type", async () => {
    // @ts-expect-error - testing runtime validation for invalid type
    await expect(tesseract.setOutputName(123)).rejects.toThrow(
      "setOutputName(outputName): outputName must be a string",
    );
  });

  it("rejects setOutputName with empty value at runtime", async () => {
    await tesseract.init({ langs: [Language.eng] });
    await expect(tesseract.setOutputName("")).rejects.toThrow(
      "setOutputName: output name is empty",
    );
  });

  it("rejects setPageMode with non-number", async () => {
    // @ts-expect-error - testing runtime validation for invalid type
    await expect(tesseract.setPageMode("x")).rejects.toThrow(
      "setPageMode(psm?): psm must be a number",
    );
  });

  it("rejects setPageMode with out-of-range value", async () => {
    await expect(tesseract.setPageMode(999 as any)).rejects.toThrow(
      "setPageMode(psm?): psm is out of range",
    );
  });

  it("rejects setImage with empty buffer", async () => {
    await expect(tesseract.setImage(Buffer.alloc(0))).rejects.toThrow(
      "setImage(buffer): buffer is empty",
    );
  });

  it("rejects setImage with undecodable image buffer", async () => {
    await expect(
      tesseract.setImage(Buffer.from("not-an-image")),
    ).rejects.toThrow("setImage(buffer): failed to decode image buffer");
  });

  it("rejects recognize with non-function callback", async () => {
    // @ts-expect-error - testing runtime validation for invalid type
    await expect(tesseract.recognize(123)).rejects.toThrow(
      "recognize(progressCallback?): progressCallback must be a function",
    );
  });

  it("rejects addProcessPage with invalid filename type", async () => {
    // @ts-expect-error - testing runtime validation for invalid type
    await expect(tesseract.addProcessPage(exampleImage, 123)).rejects.toThrow(
      "addProcessPage(buffer, filename?): filename must be a string",
    );
  });

  it("rejects beginProcessPages with invalid options payload", async () => {
    // @ts-expect-error - testing runtime validation for invalid type
    await expect(tesseract.beginProcessPages(123)).rejects.toThrow(
      "beginProcessPages(options): options must be an object",
    );
  });

  it("rejects beginProcessPages with missing title", async () => {
    await expect(
      // @ts-expect-error - testing runtime validation for invalid shape
      tesseract.beginProcessPages({ outputBase: "out" }),
    ).rejects.toThrow(
      "beginProcessPages(options): options.title is required and must be a string",
    );
  });

  it("rejects beginProcessPages with invalid timeout type", async () => {
    await expect(
      // @ts-expect-error - testing runtime validation for invalid type
      tesseract.beginProcessPages({ title: "x", timeout: "1000" }),
    ).rejects.toThrow(
      "beginProcessPages(options): options.timeout must be a number",
    );
  });

  it("rejects beginProcessPages with invalid textonly type", async () => {
    await expect(
      // @ts-expect-error - testing runtime validation for invalid type
      tesseract.beginProcessPages({ title: "x", textonly: "true" }),
    ).rejects.toThrow(
      "beginProcessPages(options): options.textonly must be a boolean",
    );
  });

  it("rejects addProcessPage when called without an active session", async () => {
    await tesseract.init({ langs: [Language.eng] });
    await expect(tesseract.addProcessPage(exampleImage)).rejects.toThrow(
      "addProcessPage: called without an active session",
    );
  });

  it("rejects addProcessPage with undecodable image buffer", async () => {
    await tesseract.init({ langs: [Language.eng] });
    await tesseract.beginProcessPages({
      title: "x",
      outputBase: path.join(os.tmpdir(), "tess-invalid-page"),
      timeout: 0,
      textonly: false,
    });
    await expect(
      tesseract.addProcessPage(Buffer.from("not-an-image")),
    ).rejects.toThrow("addProcessPage: failed to decode image buffer");
    await tesseract.abortProcessPages();
  });

  it("rejects document.addPage when called without an active session", async () => {
    await tesseract.init({ langs: [Language.eng] });
    await expect(tesseract.document.addPage(exampleImage)).rejects.toThrow(
      "addProcessPage: called without an active session",
    );
  });

  it("rejects finishProcessPages when called without an active session", async () => {
    await tesseract.init({ langs: [Language.eng] });
    await expect(tesseract.finishProcessPages()).rejects.toThrow(
      "finishProcessPages: called without an active session",
    );
  });

  it("rejects document.finish when called without an active session", async () => {
    await tesseract.init({ langs: [Language.eng] });
    await expect(tesseract.document.finish()).rejects.toThrow(
      "finishProcessPages: called without an active session",
    );
  });

  it("rejects getProcessPagesStatus with arguments", async () => {
    // @ts-expect-error - testing runtime validation for invalid call
    await expect(tesseract.getProcessPagesStatus(1)).rejects.toThrow(
      "getProcessPagesStatus(): expected no arguments",
    );
  });

  it("rejects setVariable with invalid argument count", async () => {
    await expect(
      // @ts-expect-error - testing runtime validation for invalid call
      tesseract.setVariable("tessedit_char_whitelist"),
    ).rejects.toThrow("setVariable(name, value): expected exactly 2 arguments");
  });

  it("rejects setDebugVariable with non-string value", async () => {
    await expect(
      // @ts-expect-error - testing runtime validation for invalid type
      tesseract.setDebugVariable("dawg_debug_level", 1),
    ).rejects.toThrow("setDebugVariable(name, value): value must be a string");
  });

  it("rejects setDebugVariable with empty value at runtime", async () => {
    await tesseract.init({ langs: [Language.eng] });
    await expect(
      tesseract.setDebugVariable("dawg_debug_level", ""),
    ).rejects.toThrow("setDebugVariable: variable value is empty");
  });

  it("rejects getIntVariable with non-string name", async () => {
    // @ts-expect-error - testing runtime validation for invalid type
    await expect(tesseract.getIntVariable(1)).rejects.toThrow(
      "getIntVariable(name): name must be a string",
    );
  });

  it("rejects getBoolVariable with non-string name", async () => {
    // @ts-expect-error - testing runtime validation for invalid type
    await expect(tesseract.getBoolVariable(1)).rejects.toThrow(
      "getBoolVariable(name): name must be a string",
    );
  });

  it("rejects getDoubleVariable with non-string name", async () => {
    // @ts-expect-error - testing runtime validation for invalid type
    await expect(tesseract.getDoubleVariable(1)).rejects.toThrow(
      "getDoubleVariable(name): name must be a string",
    );
  });

  it("rejects getStringVariable with non-string name", async () => {
    // @ts-expect-error - testing runtime validation for invalid type
    await expect(tesseract.getStringVariable(1)).rejects.toThrow(
      "getStringVariable(name): name must be a string",
    );
  });

  it("rejects getIntVariable for unknown variable at runtime", async () => {
    await tesseract.init({ langs: [Language.eng] });
    await expect(
      tesseract.getIntVariable("does_not_exist" as any),
    ).rejects.toThrow(
      "getIntVariable: variable 'does_not_exist' was not found",
    );
  });

  it("rejects setRectangle with missing numeric fields", async () => {
    // @ts-expect-error - testing runtime validation for invalid shape
    await expect(tesseract.setRectangle({ left: 0, top: 0 })).rejects.toThrow(
      "setRectangle(rectangle): rectangle.left/top/width/height must be numbers",
    );
  });

  it("rejects setSourceResolution with invalid type", async () => {
    // @ts-expect-error - testing runtime validation for invalid type
    await expect(tesseract.setSourceResolution("300")).rejects.toThrow(
      "setSourceResolution(ppi): ppi must be a number",
    );
  });

  it("rejects clearPersistentCache when arguments are provided", async () => {
    // @ts-expect-error - testing runtime validation for invalid call
    await expect(tesseract.clearPersistentCache(1)).rejects.toThrow(
      "clearPersistentCache(): expected no arguments",
    );
  });

  it("rejects clearAdaptiveClassifier when arguments are provided", async () => {
    // @ts-expect-error - testing runtime validation for invalid call
    await expect(tesseract.clearAdaptiveClassifier(1)).rejects.toThrow(
      "clearAdaptiveClassifier(): expected no arguments",
    );
  });

  it("rejects getDataPath when arguments are provided", async () => {
    // @ts-expect-error - testing runtime validation for invalid call
    await expect(tesseract.getDataPath(1)).rejects.toThrow(
      "getDataPath(): expected no arguments",
    );
  });

  it("rejects getSourceYResolution when arguments are provided", async () => {
    // @ts-expect-error - testing runtime validation for invalid call
    await expect(tesseract.getSourceYResolution(1)).rejects.toThrow(
      "getSourceYResolution(): expected no arguments",
    );
  });

  it("rejects getInputImage when arguments are provided", async () => {
    // @ts-expect-error - testing runtime validation for invalid call
    await expect(tesseract.getInputImage(1)).rejects.toThrow(
      "getInputImage(): expected no arguments",
    );
  });

  it("rejects getInputImage when no input image exists", async () => {
    await tesseract.init({ langs: [Language.eng] });
    await expect(tesseract.getInputImage()).rejects.toThrow(
      "getInputImage: TessBaseAPI::GetInputImage returned null",
    );
  });

  it("rejects getThresholdedImage when no thresholded image exists", async () => {
    await tesseract.init({ langs: [Language.eng] });
    await expect(tesseract.getThresholdedImage()).rejects.toThrow(
      "getThresholdedImage: TessBaseAPI::GetThresholdedImage returned null",
    );
  });

  it("rejects getInitLanguages before init", async () => {
    await expect(tesseract.getInitLanguages()).rejects.toThrow(
      "getInitLanguages: call init(...) first",
    );
  });

  it("rejects analyseLayout with invalid argument type", async () => {
    // @ts-expect-error - testing runtime validation for invalid type
    await expect(tesseract.analyseLayout(1)).rejects.toThrow(
      "analyseLayout(mergeSimilarWords?): mergeSimilarWords must be a boolean",
    );
  });

  it("rejects getPAGEText with invalid callback type", async () => {
    // @ts-expect-error - testing runtime validation for invalid type
    await expect(tesseract.getPAGEText(1)).rejects.toThrow(
      "getPAGEText(progressCallback?, pageNumber?): progressCallback must be a function",
    );
  });

  it("rejects getHOCRText with invalid pageNumber type", async () => {
    // @ts-expect-error - testing runtime validation for invalid type
    await expect(tesseract.getHOCRText(undefined, "1")).rejects.toThrow(
      "getHOCRText(progressCallback?, pageNumber?): pageNumber must be a number",
    );
  });

  it("rejects getTSVText with invalid pageNumber type", async () => {
    // @ts-expect-error - testing runtime validation for invalid type
    await expect(tesseract.getTSVText("1")).rejects.toThrow(
      "getTSVText(pageNumber?): pageNumber must be a number",
    );
  });

  it("rejects getALTOText with invalid pageNumber type", async () => {
    // @ts-expect-error - testing runtime validation for invalid type
    await expect(tesseract.getALTOText("1")).rejects.toThrow(
      "getALTOText(pageNumber?): pageNumber must be a number",
    );
  });

  it("includes error metadata for argument validation errors", async () => {
    // @ts-expect-error - testing runtime validation for invalid type
    const error = await tesseract
      .setSourceResolution("300")
      .catch((err) => err);
    expect(error).toMatchObject({
      code: "ERR_INVALID_ARGUMENT",
      method: "setSourceResolution",
    });
  });

  it("includes error metadata for range validation errors", async () => {
    const error = await tesseract.setPageMode(999 as any).catch((err) => err);
    expect(error).toMatchObject({
      code: "ERR_OUT_OF_RANGE",
      method: "setPageMode",
    });
  });

  it("includes error metadata for runtime errors", async () => {
    await tesseract.init({ langs: [Language.eng] });
    const error = await tesseract.setOutputName("").catch((err) => err);
    expect(error).toMatchObject({
      code: "ERR_TESSERACT_RUNTIME",
      method: "setOutputName",
    });
  });

  it("rejects isInitialized when arguments are provided", async () => {
    await expect(
      // @ts-expect-error - testing runtime validation for invalid call
      tesseract.isInitialized(1),
    ).rejects.toThrow("isInitialized(): expected no arguments");
  });
});

describe("tesseract api integration", () => {
  it("initializes and ends", async () => {
    const tesseract = new Tesseract();
    await tesseract.init({ langs: [Language.eng] });
    await tesseract.end();
  });

  it("accepts valid page mode", async () => {
    const tesseract = new Tesseract();
    await tesseract.init({ langs: [Language.eng] });
    await tesseract.setPageMode(PageSegmentationModes.PSM_SINGLE_LINE);
    await tesseract.end();
  });

  it("sets image and recognizes text", async () => {
    const progressCallbackSpy = vi.fn();
    const tesseract = new Tesseract();
    await tesseract.init({
      langs: [Language.eng],
    });
    await tesseract.setImage(exampleImage);
    await tesseract.recognize(progressCallbackSpy);
    const text = await tesseract.getUTF8Text();
    expect(text).toBeTypeOf("string");
    expect(text.trim().length).toBeGreaterThan(0);
    expect(progressCallbackSpy).toHaveBeenCalled();
    await tesseract.end();
  });

  it("supports HOCR and orientation detection", async () => {
    const progressCallbackSpy = vi.fn();
    const tesseract = new Tesseract();
    await tesseract.init({ langs: [Language.eng] });
    await tesseract.setImage(exampleImage);
    const hocr = await tesseract.getHOCRText(progressCallbackSpy);
    expect(hocr).toBeTypeOf("string");
    expect(progressCallbackSpy).toHaveBeenCalled();
    const osd = await tesseract.detectOrientationScript();
    expect(osd.orientationDegrees).toBeTypeOf("number");
    expect(osd.orientationConfidence).toBeTypeOf("number");
    expect(osd.scriptName).toBeTypeOf("string");
    expect(osd.scriptConfidence).toBeTypeOf("number");
    await tesseract.end();
  });

  it("sets and reads variables", async () => {
    const tesseract = new Tesseract();
    await tesseract.init({ langs: [Language.eng] });
    await tesseract.setVariable("tessedit_char_whitelist", "abc");
    const value = await tesseract.getStringVariable("tessedit_char_whitelist");
    expect(value).toBeTypeOf("string");
    expect(value).toEqual("abc");
    await tesseract.end();
  });

  it("supports rectangle and source resolution", async () => {
    const tesseract = new Tesseract();
    await tesseract.init({ langs: [Language.eng] });
    await tesseract.setImage(exampleImage);
    await tesseract.setRectangle({ left: 0, top: 0, width: 50, height: 50 });
    await tesseract.setSourceResolution(300);
    await tesseract.end();
  });

  it("should set `osd` as available languages by default", async () => {
    const tesseract = new Tesseract();
    await tesseract.init({ dataPath: "./traineddata-local", langs: [] });
    await expect(tesseract.getAvailableLanguages()).resolves.toContain("osd");
    await tesseract.end();
  });
});

describe("tesseract worker lifecycle errors", () => {
  it("rejects new commands with ERR_WORKER_CLOSED after end() is enqueued", async () => {
    const tesseract = new Tesseract();
    await tesseract.end();

    await expect(tesseract.version()).rejects.toMatchObject({
      message: "Worker is closing",
      code: "ERR_WORKER_CLOSED",
    });
  });

  it("keeps already queued commands valid but rejects commands queued after end", async () => {
    const tesseract = new Tesseract();

    const beforeEndA = tesseract.version();
    const beforeEndB = tesseract.version();
    const endPromise = tesseract.end();
    const afterEnd = tesseract.version().catch((error) => error);

    await expect(beforeEndA).resolves.toMatch(/^\d+\.\d+\.\d+/);
    await expect(beforeEndB).resolves.toMatch(/^\d+\.\d+\.\d+/);
    await expect(endPromise).resolves.toBeUndefined();
    await expect(afterEnd).resolves.toMatchObject({
      code: "ERR_WORKER_CLOSED",
    });
  });
});

describe("tesseract multipage status api", () => {
  let tempDir: string;

  beforeEach(async () => {
    tempDir = await mkdtemp(path.join(os.tmpdir(), "tess-pages-"));
  });

  afterEach(async () => {
    await rm(tempDir, { recursive: true, force: true });
  });

  it("returns inactive default status when no session is active", async () => {
    const tesseract = new Tesseract();
    const status = await tesseract.getProcessPagesStatus();
    expect(status).toStrictEqual({
      active: false,
      healthy: false,
      processedPages: 0,
      nextPageIndex: 0,
      outputBase: "",
      timeoutMillisec: 0,
      textonly: false,
    });
    await tesseract.end();
  });

  it("exposes the same status object via document facade", async () => {
    const tesseract = new Tesseract();
    await expect(tesseract.document.status()).resolves.toStrictEqual(
      await tesseract.getProcessPagesStatus(),
    );
    await tesseract.end();
  });

  it("tracks processed pages during an active multipage session", async () => {
    const tesseract = new Tesseract();
    const outputBase = path.join(tempDir, "out");
    await tesseract.init({ langs: [Language.eng] });

    await tesseract.document.begin({
      outputBase,
      title: "test-doc",
      timeout: 0,
      textonly: false,
    });

    await expect(tesseract.document.status()).resolves.toMatchObject({
      active: true,
      healthy: true,
      processedPages: 0,
      nextPageIndex: 0,
      outputBase,
      timeoutMillisec: 0,
      textonly: false,
    });

    await tesseract.document.addPage(exampleImage, exampleImagePath);
    await expect(tesseract.getProcessPagesStatus()).resolves.toMatchObject({
      active: true,
      processedPages: 1,
      nextPageIndex: 1,
    });

    await tesseract.document.abort();
    await expect(tesseract.getProcessPagesStatus()).resolves.toStrictEqual({
      active: false,
      healthy: false,
      processedPages: 0,
      nextPageIndex: 0,
      outputBase: "",
      timeoutMillisec: 0,
      textonly: false,
    });

    await tesseract.end();
  });
});
