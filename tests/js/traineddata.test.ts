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

import { mkdir, mkdtemp, readFile, rm, writeFile } from "node:fs/promises";
import os from "node:os";
import path from "node:path";
import { ReadableStream } from "node:stream/web";
import { gzipSync } from "node:zlib";
import { afterEach, beforeEach, describe, expect, it, vi } from "vitest";
import Tesseract, { Language } from "../../lib/index";
import { isValidTraineddata } from "../../lib/utils";

vi.mock("pkg-prebuilds", () => {
  const factory = () => ({
    Tesseract: class {
      async init() {}
      async end() {}
    },
  });
  return factory as unknown as () => {
    Tesseract: new () => { init(): Promise<void>; end(): Promise<void> };
  };
});

describe("traineddata utils", () => {
  let tempDir: string;

  beforeEach(async () => {
    tempDir = await mkdtemp(path.join(os.tmpdir(), "tess-utils-"));
  });

  afterEach(async () => {
    await rm(tempDir, { recursive: true, force: true });
  });

  it("validates traineddata files correctly", async () => {
    const missing = path.join(tempDir, "missing.traineddata");
    await expect(isValidTraineddata(missing)).resolves.toBe(false);

    const empty = path.join(tempDir, "empty.traineddata");
    await writeFile(empty, "");
    await expect(isValidTraineddata(empty)).resolves.toBe(false);

    const filled = path.join(tempDir, "filled.traineddata");
    await writeFile(filled, "ok");
    await expect(isValidTraineddata(filled)).resolves.toBe(true);
  });
});

describe("ensureTrainingData", () => {
  let dataDir: string;
  let cacheDir: string;
  const lang = "eng" as Language;
  const downloadBaseUrl = "https://example.com/";
  const originalEnv = process.env.TESSDATA_PREFIX;

  beforeEach(async () => {
    dataDir = await mkdtemp(path.join(os.tmpdir(), "tess-data-"));
    cacheDir = await mkdtemp(path.join(os.tmpdir(), "tess-cache-"));
  });

  afterEach(async () => {
    await rm(dataDir, { recursive: true, force: true });
    await rm(cacheDir, { recursive: true, force: true });
    if (originalEnv === undefined) {
      delete process.env.TESSDATA_PREFIX;
    } else {
      process.env.TESSDATA_PREFIX = originalEnv;
    }
    vi.unstubAllGlobals();
  });

  it("uses cached traineddata before downloading", async () => {
    const cachedPath = path.join(cacheDir, `${lang}.traineddata`);
    await mkdir(cacheDir, { recursive: true });
    await writeFile(cachedPath, "cached");

    const fetchSpy = vi.fn();
    vi.stubGlobal("fetch", fetchSpy);

    const instance = new Tesseract();
    const resultPath = await instance.ensureTrainingData({
      lang,
      dataPath: dataDir,
      cachePath: cacheDir,
      downloadBaseUrl,
    });

    expect(fetchSpy).not.toHaveBeenCalled();
    expect(resultPath).toBe(path.join(dataDir, `${lang}.traineddata`));
    await expect(readFile(resultPath, "utf8")).resolves.toBe("cached");
  });

  it("downloads, decompresses, and reports progress", async () => {
    const payload = Buffer.from("traineddata");
    const gzipped = gzipSync(payload);
    const mid = Math.floor(gzipped.length / 2);
    const stream = new ReadableStream<Uint8Array>({
      start(controller) {
        controller.enqueue(gzipped.subarray(0, mid));
        controller.enqueue(gzipped.subarray(mid));
        controller.close();
      },
    });

    vi.stubGlobal(
      "fetch",
      vi.fn().mockResolvedValue(
        // @ts-expect-error
        new Response(stream, {
          status: 200,
          headers: { "content-length": String(gzipped.length) },
        }),
      ),
    );

    process.env.TESSDATA_PREFIX = "KEEP";
    const progress = vi.fn();
    const instance = new Tesseract();
    const resultPath = await instance.ensureTrainingData(
      {
        lang,
        dataPath: dataDir,
        cachePath: cacheDir,
        downloadBaseUrl,
      },
      progress,
    );

    expect(process.env.TESSDATA_PREFIX).toBe("KEEP");
    await expect(readFile(resultPath, "utf8")).resolves.toBe("traineddata");
    expect(progress).toHaveBeenCalled();
    const lastCall = progress.mock.calls.at(-1)?.[0];
    expect(lastCall?.downloadedBytes).toBe(gzipped.length);
    expect(lastCall?.totalBytes).toBe(gzipped.length);
    expect(lastCall?.percent).toBeCloseTo(100, 1);
  });
});
