import { readFileSync } from "node:fs";
import { fileURLToPath } from "node:url";

import { describe, expect, it, vi } from "vitest";

import Tesseract, {
  AvailableLanguages,
  OcrEngineModes,
  PageSegmentationModes,
} from "../../lib/index";

const exampleImageUrl = new URL("../../example8.jpg", import.meta.url);
const exampleImage = readFileSync(fileURLToPath(exampleImageUrl));

describe("tesseract api validation", () => {
  it("rejects init without options", async () => {
    const instance = new Tesseract();
    // @ts-expect-error - testing runtime validation for missing options
    await expect(instance.init()).rejects.toThrow(
      "Expected required argument at index 0 to be of type object",
    );
    await instance.end();
  });

  it("rejects init with invalid lang type", async () => {
    const instance = new Tesseract();
    // @ts-expect-error - testing runtime validation for invalid type
    await expect(instance.init({ lang: 123 })).rejects.toThrow(
      "Option 'lang' must be of type string",
    );
    await instance.end();
  });

  it("rejects init with invalid oem type", async () => {
    const instance = new Tesseract();
    // @ts-expect-error - testing runtime validation for invalid type
    await expect(instance.init({ oem: "x" })).rejects.toThrow(
      "Option 'oem' must be of type number",
    );
    await instance.end();
  });

  it("rejects init with unsupported oem value", async () => {
    const instance = new Tesseract();
    // @ts-expect-error - testing runtime validation for invalid type
    await expect(instance.init({ oem: 999 })).rejects.toThrow(
      "Unsupported OCR Engine Mode",
    );
    await instance.end();
  });

  it("rejects init when only varsVec or varsValues is provided", async () => {
    const instance = new Tesseract();
    await expect(instance.init({ varsVec: ["a"] })).rejects.toThrow(
      "Options 'varsVec' and 'varsValues' must be provided together",
    );
    await instance.end();
  });

  it("rejects init when varsVec and varsValues lengths differ", async () => {
    const instance = new Tesseract();
    await expect(
      instance.init({ varsVec: ["a"], varsValues: ["b", "c"] }),
    ).rejects.toThrow("'varsVec' and 'varsValues' must have the same length");
    await instance.end();
  });

  it("rejects setPageMode with non-number", async () => {
    const instance = new Tesseract();
    // @ts-expect-error - testing runtime validation for invalid type
    await expect(instance.setPageMode("x")).rejects.toThrow(
      "Expected page segmentation mode to be of type number",
    );
    await instance.end();
  });

  it("rejects setImage with empty buffer", async () => {
    const instance = new Tesseract();
    await expect(instance.setImage(Buffer.alloc(0))).rejects.toThrow(
      "SetImage: buffer is empty",
    );
    await instance.end();
  });

  it("rejects recognize with non-function callback", async () => {
    const instance = new Tesseract();
    // @ts-expect-error - testing runtime validation for invalid type
    await expect(instance.recognize(123)).rejects.toThrow(
      "Expected progress callback to be of type function",
    );
    await instance.end();
  });
});

describe("tesseract api integration", () => {
  it("initializes and ends", async () => {
    const instance = new Tesseract();
    await instance.init({ lang: AvailableLanguages.eng });
    await instance.end();
  });

  it("accepts valid page mode", async () => {
    const instance = new Tesseract();
    await instance.init({ lang: AvailableLanguages.eng });
    await instance.setPageMode(PageSegmentationModes.PSM_SINGLE_LINE);
    await instance.end();
  });

  it("sets image and recognizes text", async () => {
    const progressCallbackSpy = vi.fn();
    const instance = new Tesseract();
    await instance.init({
      lang: AvailableLanguages.eng,
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
    await instance.init({ lang: AvailableLanguages.eng });
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
    await instance.init({ lang: AvailableLanguages.eng });
    await instance.setVariable("tessedit_char_whitelist", "abc");
    const value = await instance.getStringVariable("tessedit_char_whitelist");
    expect(value).toBeTypeOf("string");
    expect(value).toEqual("abc");
    await instance.end();
  });

  it("supports rectangle and source resolution", async () => {
    const instance = new Tesseract();
    await instance.init({ lang: AvailableLanguages.eng });
    await instance.setImage(exampleImage);
    await instance.setRectangle({ left: 0, top: 0, width: 50, height: 50 });
    await instance.setSourceResolution(300);
    await instance.end();
  });
});
