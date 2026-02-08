import { readFileSync } from "node:fs";
import { fileURLToPath } from "node:url";

import { afterEach, beforeEach, describe, expect, it, vi } from "vitest";

import Tesseract, {
  Language,
  PageSegmentationModes,
  TesseractInstance,
} from "../../lib/index";

const exampleImageUrl = new URL("../../example8.jpg", import.meta.url);
const exampleImage = readFileSync(fileURLToPath(exampleImageUrl));

describe("tesseract api validation", () => {
  let tesseract: TesseractInstance;
  beforeEach(() => {
    tesseract = new Tesseract();
  });

  afterEach(async () => {
    await tesseract?.end();
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
      "Option 'oem' must be of type number",
    );
  });

  it("rejects init with unsupported oem value", async () => {
    const instance = new Tesseract();
    // @ts-expect-error - testing runtime validation for invalid type
    await expect(instance.init({ oem: 999 })).rejects.toThrow(
      "Unsupported OCR Engine Mode",
    );
  });

  it("rejects init when only a vars key is provided", async () => {
    const instance = new Tesseract();
    await expect(
      instance.init({ vars: { allow_blob_division: undefined } }),
    ).rejects.toThrow("'vars' must contain only strings");
  });

  it("rejects setPageMode with non-number", async () => {
    const instance = new Tesseract();
    // @ts-expect-error - testing runtime validation for invalid type
    await expect(instance.setPageMode("x")).rejects.toThrow(
      "Expected page segmentation mode to be of type number",
    );
  });

  it("rejects setImage with empty buffer", async () => {
    const instance = new Tesseract();
    await expect(instance.setImage(Buffer.alloc(0))).rejects.toThrow(
      "SetImage: buffer is empty",
    );
  });

  it("rejects recognize with non-function callback", async () => {
    const instance = new Tesseract();
    // @ts-expect-error - testing runtime validation for invalid type
    await expect(instance.recognize(123)).rejects.toThrow(
      "Expected progress callback to be of type function",
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
    await expect(instance.getAvailableLanguages()).resolves.toStrictEqual([
      "osd",
    ]);
    await instance.end();
  });
});
