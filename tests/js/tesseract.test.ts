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

  it("accepts init without options", async () => {
    const instance = new Tesseract();
    await expect(
      instance.init({ ensureTraineddata: false }),
    ).resolves.toBeUndefined();
  });

  it("skips sparse languages (e.g. `undefined`) in `langs`", async () => {
    const instance = new Tesseract();
    await expect(
      // @ts-expect-error - sparse languages array
      instance.init({ langs: [, "eng"], ensureTraineddata: false }),
    ).resolves.toBeUndefined();
    await expect(
      // @ts-expect-error - sparse languages array
      instance.init({ langs: ["deu", , "eng"], ensureTraineddata: false }),
    ).resolves.toBeUndefined();
    await expect(
      instance.init({ langs: ["deu", "eng"], ensureTraineddata: false }),
    ).resolves.toBeUndefined();
    await expect(instance.getInitLanguages()).resolves.toBe("deu+eng");
    await instance.end();
  });

  it("rejects init with invalid lang type", async () => {
    const instance = new Tesseract();
    // @ts-expect-error - testing runtime validation for invalid type
    await expect(instance.init({ langs: 123 })).rejects.toThrow(
      "options.langs is not iterable",
    );
  });

  it("rejects init with invalid oem type", async () => {
    const instance = new Tesseract();
    // @ts-expect-error - testing runtime validation for invalid type
    await expect(instance.init({ oem: "x" })).rejects.toThrow(
      "init(options): options.oem must be a number",
    );
  });

  it("rejects init with unsupported oem value", async () => {
    const instance = new Tesseract();
    // @ts-expect-error - testing runtime validation for invalid type
    await expect(instance.init({ oem: 999 })).rejects.toThrow(
      "init(options): options.oem is out of supported range",
    );
  });

  it("rejects init when only a vars key is provided", async () => {
    const instance = new Tesseract();
    await expect(
      instance.init({ vars: { allow_blob_division: undefined } }),
    ).rejects.toThrow("init(options): options.vars must contain only strings");
  });

  it("rejects setPageMode with non-number", async () => {
    const instance = new Tesseract();
    // @ts-expect-error - testing runtime validation for invalid type
    await expect(instance.setPageMode("x")).rejects.toThrow(
      "setPageMode(psm?): psm must be a number",
    );
  });

  it("rejects setImage with empty buffer", async () => {
    const instance = new Tesseract();
    await expect(instance.setImage(Buffer.alloc(0))).rejects.toThrow(
      "setImage(buffer): buffer is empty",
    );
  });

  it("rejects recognize with non-function callback", async () => {
    const instance = new Tesseract();
    // @ts-expect-error - testing runtime validation for invalid type
    await expect(instance.recognize(123)).rejects.toThrow(
      "recognize(progressCallback?): progressCallback must be a function",
    );
  });

  it("rejects addProcessPage with invalid filename type", async () => {
    const instance = new Tesseract();
    // @ts-expect-error - testing runtime validation for invalid type
    await expect(instance.addProcessPage(exampleImage, 123)).rejects.toThrow(
      "addProcessPage(buffer, filename?): filename must be a string",
    );
  });
});

describe("tesseract api integration", () => {
  it("initializes and ends", async () => {
    const instance = new Tesseract();
    await instance.init({ langs: [Language.eng] });
    await instance.end();
  });

  it("accepts valid page mode", async () => {
    const instance = new Tesseract();
    await instance.init({ langs: [Language.eng] });
    await instance.setPageMode(PageSegmentationModes.PSM_SINGLE_LINE);
    await instance.end();
  });

  it("sets image and recognizes text", async () => {
    const progressCallbackSpy = vi.fn();
    const instance = new Tesseract();
    await instance.init({
      langs: [Language.eng],
    });
    await instance.setImage(exampleImage);
    await instance.recognize(progressCallbackSpy);
    const text = await instance.getUTF8Text();
    expect(text).toBeTypeOf("string");
    expect(text.trim().length).toBeGreaterThan(0);
    expect(progressCallbackSpy).toHaveBeenCalled();
    await instance.end();
  });

  it("supports HOCR and orientation detection", async () => {
    const progressCallbackSpy = vi.fn();
    const instance = new Tesseract();
    await instance.init({ langs: [Language.eng] });
    await instance.setImage(exampleImage);
    const hocr = await instance.getHOCRText(progressCallbackSpy);
    expect(hocr).toBeTypeOf("string");
    expect(progressCallbackSpy).toHaveBeenCalled();
    const osd = await instance.detectOrientationScript();
    expect(osd.orientationDegrees).toBeTypeOf("number");
    expect(osd.orientationConfidence).toBeTypeOf("number");
    expect(osd.scriptName).toBeTypeOf("string");
    expect(osd.scriptConfidence).toBeTypeOf("number");
    await instance.end();
  });

  it("sets and reads variables", async () => {
    const instance = new Tesseract();
    await instance.init({ langs: [Language.eng] });
    await instance.setVariable("tessedit_char_whitelist", "abc");
    const value = await instance.getStringVariable("tessedit_char_whitelist");
    expect(value).toBeTypeOf("string");
    expect(value).toEqual("abc");
    await instance.end();
  });

  it("supports rectangle and source resolution", async () => {
    const instance = new Tesseract();
    await instance.init({ langs: [Language.eng] });
    await instance.setImage(exampleImage);
    await instance.setRectangle({ left: 0, top: 0, width: 50, height: 50 });
    await instance.setSourceResolution(300);
    await instance.end();
  });

  it("should set `osd` as available languages by default", async () => {
    const instance = new Tesseract();
    await instance.init({ dataPath: "./traineddata-local", langs: [] });
    await expect(instance.getAvailableLanguages()).resolves.toContain("osd");
    await instance.end();
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
    const instance = new Tesseract();
    const status = await instance.getProcessPagesStatus();
    expect(status).toStrictEqual({
      active: false,
      healthy: false,
      processedPages: 0,
      nextPageIndex: 0,
      outputBase: "",
      timeoutMillisec: 0,
      textonly: false,
    });
    await instance.end();
  });

  it("exposes the same status object via document facade", async () => {
    const instance = new Tesseract();
    await expect(instance.document.status()).resolves.toStrictEqual(
      await instance.getProcessPagesStatus(),
    );
    await instance.end();
  });

  it("tracks processed pages during an active multipage session", async () => {
    const instance = new Tesseract();
    const outputBase = path.join(tempDir, "out");
    await instance.init({ langs: [Language.eng] });

    await instance.document.begin({
      outputBase,
      title: "test-doc",
      timeout: 0,
      textonly: false,
    });

    await expect(instance.document.status()).resolves.toMatchObject({
      active: true,
      healthy: true,
      processedPages: 0,
      nextPageIndex: 0,
      outputBase,
      timeoutMillisec: 0,
      textonly: false,
    });

    await instance.document.addPage(exampleImage, exampleImagePath);
    await expect(instance.getProcessPagesStatus()).resolves.toMatchObject({
      active: true,
      processedPages: 1,
      nextPageIndex: 1,
    });

    await instance.document.abort();
    await expect(instance.getProcessPagesStatus()).resolves.toStrictEqual({
      active: false,
      healthy: false,
      processedPages: 0,
      nextPageIndex: 0,
      outputBase: "",
      timeoutMillisec: 0,
      textonly: false,
    });

    await instance.end();
  });
});
