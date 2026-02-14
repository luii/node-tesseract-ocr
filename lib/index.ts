/*
 * Copyright 2025 Philipp Czarnetzki
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

/**
 * All available languages for tesseract
 * @readonly
 * @enum {string}
 */
export const Language = {
  afr: "afr",
  amh: "amh",
  ara: "ara",
  asm: "asm",
  aze: "aze",
  aze_cyrl: "aze_cyrl",
  bel: "bel",
  ben: "ben",
  bod: "bod",
  bos: "bos",
  bre: "bre",
  bul: "bul",
  cat: "cat",
  ceb: "ceb",
  ces: "ces",
  chi_sim: "chi_sim",
  chi_tra: "chi_tra",
  chr: "chr",
  cos: "cos",
  cym: "cym",
  dan: "dan",
  deu: "deu",
  deu_latf: "deu_latf",
  div: "div",
  dzo: "dzo",
  ell: "ell",
  eng: "eng",
  enm: "enm",
  epo: "epo",
  equ: "equ",
  est: "est",
  eus: "eus",
  fao: "fao",
  fas: "fas",
  fil: "fil",
  fin: "fin",
  fra: "fra",
  frm: "frm",
  fry: "fry",
  gla: "gla",
  gle: "gle",
  glg: "glg",
  grc: "grc",
  guj: "guj",
  hat: "hat",
  heb: "heb",
  hin: "hin",
  hrv: "hrv",
  hun: "hun",
  hye: "hye",
  iku: "iku",
  ind: "ind",
  isl: "isl",
  ita: "ita",
  ita_old: "ita_old",
  jav: "jav",
  jpn: "jpn",
  kan: "kan",
  kat: "kat",
  kat_old: "kat_old",
  kaz: "kaz",
  khm: "khm",
  kir: "kir",
  kmr: "kmr",
  kor: "kor",
  kor_vert: "kor_vert",
  kur: "kur",
  lao: "lao",
  lat: "lat",
  lav: "lav",
  lit: "lit",
  ltz: "ltz",
  mal: "mal",
  mar: "mar",
  mkd: "mkd",
  mlt: "mlt",
  mon: "mon",
  mri: "mri",
  msa: "msa",
  mya: "mya",
  nep: "nep",
  nld: "nld",
  nor: "nor",
  oci: "oci",
  ori: "ori",
  osd: "osd",
  pan: "pan",
  pol: "pol",
  por: "por",
  pus: "pus",
  que: "que",
  ron: "ron",
  rus: "rus",
  san: "san",
  sin: "sin",
  slk: "slk",
  slv: "slv",
  snd: "snd",
  spa: "spa",
  spa_old: "spa_old",
  sqi: "sqi",
  srp: "srp",
  srp_latn: "srp_latn",
  sun: "sun",
  swa: "swa",
  swe: "swe",
  syr: "syr",
  tam: "tam",
  tat: "tat",
  tel: "tel",
  tgk: "tgk",
  tha: "tha",
  tir: "tir",
  ton: "ton",
  tur: "tur",
  uig: "uig",
  ukr: "ukr",
  urd: "urd",
  uzb: "uzb",
  uzb_cyrl: "uzb_cyrl",
  vie: "vie",
  yid: "yid",
  yor: "yor",
} as const;

export type Language = (typeof Language)[keyof typeof Language];

/**
 * When Tesseract/Cube is initialized we can choose to instantiate/load/run
 * only the Tesseract part, only the Cube part or both along with the combiner.
 * The preference of which engine to use is stored in tessedit_ocr_engine_mode.
 * @readonly
 * @enum {number}
 */
export const OcrEngineModes = {
  /**
   * Run Tesseract only - fastest
   * @deprecated
   * @type {number}
   */
  OEM_TESSERACT_ONLY: 0,

  /**
   * Run just the LSTM line recognizer.
   * @type {nmumber}
   */
  OEM_LSTM_ONLY: 1,

  /**
   * Run the LSTM recognizer, but allow fallback
   * to Tesseract when things get difficult.
   * @deprecated
   * @type {number}
   */
  OEM_TESSERACT_LSTM_COMBINED: 2,

  /**
   * Specify this mode when calling init(),
   * to indicate that any of the above modes
   * should be automatically inferred from the
   * variables in the language-specific config,
   * command-line configs, or if not specified
   * in any of the above should be set to the
   * default OEM_TESSERACT_ONLY.
   * @type {number}
   * @default
   */
  OEM_DEFAULT: 3,
} as const;

export type OcrEngineMode =
  (typeof OcrEngineModes)[keyof typeof OcrEngineModes];

/**
 * Possible modes for page layout analysis.
 * @readonly
 * @enum {number}
 */
export const PageSegmentationModes = {
  // Orientation and script detection only.
  PSM_OSD_ONLY: 0,
  // Automatic page segmentation with orientation and script detection. (OSD)
  PSM_AUTO_OSD: 1,
  // Automatic page segmentation, but no OSD, or OCR.
  PSM_AUTO_ONLY: 2,
  // Fully automatic page segmentation, but no OSD.
  PSM_AUTO: 3,
  // Assume a single column of text of variable sizes.
  PSM_SINGLE_COLUMN: 4,
  // Assume a single uniform block of vertically aligned text.
  PSM_SINGLE_BLOCK_VERT_TEXT: 5,
  // Assume a single uniform block of text. (Default.)
  PSM_SINGLE_BLOCK: 6,
  // Treat the image as a single text line.
  PSM_SINGLE_LINE: 7,
  // Treat the image as a single word.
  PSM_SINGLE_WORD: 8,
  // Treat the image as a single word in a circle.
  PSM_CIRCLE_WORD: 9,
  // Treat the image as a single character.
  PSM_SINGLE_CHAR: 10,
  // Find as much text as possible in no particular order.
  PSM_SPARSE_TEXT: 11,
  // Sparse text with orientation and script det.
  PSM_SPARSE_TEXT_OSD: 12,
  // Treat the image as a single text line, bypassing hacks that are Tesseract-specific.
  PSM_RAW_LINE: 13,
} as const;

export type PageSegmentationMode =
  (typeof PageSegmentationModes)[keyof typeof PageSegmentationModes];

export const LogLevels = {
  ALL: "-2147483648",
  TRACE: "5000",
  DEBUG: "10000",
  INFO: "20000",
  WARN: "30000",
  ERROR: "40000",
  FATAL: "50000",
  OFF: "2147483647",
} as const;

export type LogLevel = (typeof LogLevels)[keyof typeof LogLevels];


import type {
  EnsureTrainedDataOptions,
  TesseractConstructor,
  TesseractInitOptions,
  TrainingDataDownloadProgress,
} from "./types";

export type {
  ConfigurationVariables,
  DetectOrientationScriptResult,
  EnsureTrainedDataOptions,
  InitOnlyConfigurationVariables,
  ProgressChangedInfo,
  SetVariableConfigVariables,
  TesseractBeginProcessPagesOptions,
  TesseractConstructor,
  TesseractInitOptions,
  TesseractInstance,
  TesseractSetRectangleOptions,
  TrainingDataDownloadProgress,
} from "./types";
export type NativeTesseract = import("./types").TesseractInstance;

import { existsSync, createWriteStream } from "node:fs";
import { mkdir, rename, rm, copyFile } from "node:fs/promises";
import os from "node:os";
import path from "node:path";
import { Readable, Transform } from "node:stream";
import { pipeline } from "node:stream/promises";
import { createGunzip } from "node:zlib";
import { lock } from "proper-lockfile";
import { isValidTraineddata } from "./utils";

const TESSDATA4_BEST = (lang: Language) =>
  `https://cdn.jsdelivr.net/npm/@tesseract.js-data/${lang}/4.0.0_best_int/`;
const TESSDATA4 = (lang: Language) =>
  `https://cdn.jsdelivr.net/npm/@tesseract.js-data/${lang}/4.0.0/`;

const DEFAULT_CACHE_DIR = path.join(
  os.homedir(),
  ".cache",
  "node-tesseract-ocr",
  "tessdata",
);

const rootFromSource = path.resolve(__dirname, "../../");
const bindingOptionsFromSource = path.resolve(
  rootFromSource,
  "binding-options.js",
);
const bindingOptionsPath = existsSync(bindingOptionsFromSource)
  ? bindingOptionsFromSource
  : path.resolve(process.cwd(), "binding-options.js");
const prebuildRoot = existsSync(bindingOptionsFromSource)
  ? rootFromSource
  : process.cwd();

const { Tesseract: NativeTesseract } = require("pkg-prebuilds")(
  prebuildRoot,
  require(bindingOptionsPath),
) as { Tesseract: TesseractConstructor };

class Tesseract extends NativeTesseract {
  constructor() {
    super();
  }
  async init(options: TesseractInitOptions = {}) {
    options.langs ??= [];
    options.ensureTraineddata ??= true;
    options.cachePath ??= DEFAULT_CACHE_DIR;
    options.dataPath ??= process.env.TESSDATA_PREFIX ?? options.cachePath;
    options.progressCallback ??= undefined;

    const cachePath = path.resolve(options.cachePath);
    const dataPath = path.resolve(options.dataPath);

    if (options.ensureTraineddata) {
      for (const lang of [...options.langs, Language.osd]) {
        const downloadBaseUrl =
          options.oem === OcrEngineModes.OEM_LSTM_ONLY
            ? TESSDATA4_BEST(lang)
            : TESSDATA4(lang);

        lang &&
          (await this.ensureTrainingData(
            { lang, dataPath, cachePath, downloadBaseUrl },
            options.progressCallback,
          ));
      }
    }

    return super.init(options);
  }

  async ensureTrainingData(
    { lang, dataPath, cachePath, downloadBaseUrl }: EnsureTrainedDataOptions,
    progressCallback?: (info: TrainingDataDownloadProgress) => void,
  ) {
    const traineddataPath = path.join(dataPath, `${lang}.traineddata`);
    const cacheTraineddataPath = path.join(cachePath, `${lang}.traineddata`);

    if (await isValidTraineddata(cacheTraineddataPath)) {
      if (traineddataPath !== cacheTraineddataPath) {
        await mkdir(dataPath, { recursive: true });
        await copyFile(cacheTraineddataPath, traineddataPath);
      }
      return traineddataPath;
    }
    if (await isValidTraineddata(traineddataPath)) {
      return traineddataPath;
    }

    await mkdir(dataPath, { recursive: true });

    const release = await lock(traineddataPath, {
      lockfilePath: `${traineddataPath}.lock`,
      stale: 10 * 60 * 1000,
      update: 30 * 1000,
      realpath: false,
      retries: { retries: 50, minTimeout: 200, maxTimeout: 2000 },
    });

    try {
      if (await isValidTraineddata(traineddataPath)) {
        return traineddataPath;
      }
      if (
        traineddataPath !== cacheTraineddataPath &&
        (await isValidTraineddata(cacheTraineddataPath))
      ) {
        await copyFile(cacheTraineddataPath, traineddataPath);
        return traineddataPath;
      }

      const url = new URL(`${lang}.traineddata.gz`, downloadBaseUrl).toString();
      const response = await fetch(url);

      if (!response.ok || !response.body) {
        throw new Error(
          `Failed to download traineddata for ${lang}: ${response.status} ${response.statusText}`,
        );
      }

      const tmpPath = path.join(
        os.tmpdir(),
        [
          "node-tesseract-ocr",
          lang,
          "traineddata",
          process.pid,
          Date.now(),
          Math.random().toString(36).slice(2),
        ].join("-"),
      );
      const totalBytesHeader = response.headers.get("content-length");
      const totalBytes = totalBytesHeader
        ? Number(totalBytesHeader)
        : undefined;
      let downloadedBytes = 0;
      const progressStream = new Transform({
        transform(chunk, _, callback) {
          if (progressCallback) {
            downloadedBytes += chunk.length;
            const percent =
              typeof totalBytes === "number" && Number.isFinite(totalBytes)
                ? (downloadedBytes / totalBytes) * 100
                : undefined;
            progressCallback({
              lang,
              url,
              downloadedBytes,
              totalBytes: Number.isFinite(totalBytes) ? totalBytes : undefined,
              percent,
            });
          }
          callback(null, chunk);
        },
      });

      try {
        await pipeline(
          Readable.fromWeb(response.body),
          progressStream,
          createGunzip(),
          createWriteStream(tmpPath),
        );
        try {
          await rename(tmpPath, traineddataPath);
        } catch (error) {
          if ((error as { code?: string }).code === "EXDEV") {
            await copyFile(tmpPath, traineddataPath);
            await rm(tmpPath, { force: true });
          } else {
            throw error;
          }
        }
      } catch (error) {
        await rm(tmpPath, { force: true });
        throw error;
      }

      return traineddataPath;
    } finally {
      await release();
    }
  }
}

export { Tesseract, NativeTesseract };
export default Tesseract;
