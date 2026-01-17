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
export const AvailableLanguages = {
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

export type AvailableLanguage =
  (typeof AvailableLanguages)[keyof typeof AvailableLanguages];

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

/**
 * Tesseract init options
 */
export interface TesseractInitOptions {
  /**
   * Its generally safer to use as few languages as possible.
   * The more languages Tesseract needs to load the longer it takes to recognize a image.
   * @public
   */
  lang?: AvailableLanguage[];

  /**
   * OCR Engine Modes
   * The engine mode cannot be changed after creating the instance
   * If another mode is needed, its advised to create a new instance.
   * @throws {Error} Will throw an error when oem mode is below 0 or over 3
   */
  oem?: OcrEngineMode;
  setOnlyNonDebugParams?: boolean;
  configs?: Array<string>;

  /**
   *
   */
  varsVec?: Array<string>;
  varsValues?: Array<string>;
}

export interface TesseractSetRectangleOptions {
  top: number;
  left: number;
  width: number;
  height: number;
}

export interface ProgressChangedInfo {
  /**
   * Chars in this buffer
   */
  progress: number;

  /**
   * Percent complete increasing (0-100)
   */
  percent: number;

  /**
   * States if the worker is still alive
   */
  ocrAlive: number;

  /**
   * top coordinate of the bbox of the current element that tesseract is processing
   */
  top: number;

  /**
   * right coordinate of the bbox of the current element that tesseract is processing
   */
  right: number;

  /**
   * bottom coordinate of the bbox of the current element that tesseract is processing
   */
  bottom: number;

  /**
   * left coordinate of the bbox of the current element that tesseract is processing
   */
  left: number;
}

export interface DetectOrientationScriptResult {
  /**
   * Orientation of the source image in degrees
   * Orientation refers to the way the source is rotated, **not** how the text is
   * aligned.
   * @example
   *   0 = 0°
   *   1 = 90°
   *   2 = 180°
   *   3 = 270°
   * @type {number}
   */
  orientationDegrees: number;
  /**
   * The confidence of tesseract for the orientation
   * @type {number}
   */
  orientationConfidence: number;
  scriptName: string;
  scriptConfidence: number;
}

export interface TesseractInstance {
  init: (options: TesseractInitOptions) => Promise<void>;
  initForAnalysePage: () => Promise<void>;
  analysePage: (mergeSimilarWords: boolean) => Promise<void>; // TODO: return pageiterator here
  setPageMode: (psm: PageSegmentationMode) => Promise<void>;
  setVariable: (name: string, value: string) => Promise<number>;
  getIntVariable: (name: string) => Promise<number>;
  getBoolVariable: (name: string) => Promise<number>;
  getDoubleVariable: (name: string) => Promise<number>;
  getStringVariable: (name: string) => Promise<string>;
  setImage: (buffer: Buffer<ArrayBuffer>) => Promise<void>;
  setRectangle: (options: TesseractSetRectangleOptions) => Promise<void>;
  setSourceResolution: (ppi: number) => Promise<void>;

  /**
   * @throws  {Error} Will throw an error if the parameter at index 0 is not a function
   * @param   {(info: ProgressChangedInfo) => void} progressCallback Callback will be called to inform the user about progress changes
   * @returns {Promise<void>}
   */
  recognize: (
    progressCallback: (info: ProgressChangedInfo) => void,
  ) => Promise<void>;
  detectOrientationScript: () => Promise<DetectOrientationScriptResult>;
  meanTextConf: () => Promise<number>;
  getUTF8Text: () => Promise<string>;
  getHOCRText: (
    progressCallback?: (info: ProgressChangedInfo) => void,
    pageNumber?: number,
  ) => Promise<string>;
  getTSVText: () => Promise<string>;
  getUNLVText: () => Promise<string>;
  getALTOText: (
    progressCallback?: (info: ProgressChangedInfo) => void,
    pageNumber?: number,
  ) => Promise<string>;
  getInitLanguages: () => Promise<AvailableLanguage>;
  getLoadedLanguages: () => Promise<AvailableLanguage[]>;
  getAvailableLanguages: () => Promise<AvailableLanguage[]>;
  clear: () => Promise<void>;
  end: () => Promise<void>;
}

export type Tesseract = TesseractInstance;
export type TesseractConstructor = new () => TesseractInstance;

const fs = require("node:fs");
const path = require("node:path");

const rootFromSource = path.resolve(__dirname, "../../");
const bindingOptionsFromSource = path.resolve(
  rootFromSource,
  "binding-options.js",
);
const bindingOptionsPath = fs.existsSync(bindingOptionsFromSource)
  ? bindingOptionsFromSource
  : path.resolve(process.cwd(), "binding-options.js");
const prebuildRoot = fs.existsSync(bindingOptionsFromSource)
  ? rootFromSource
  : process.cwd();

const { Tesseract } = require("pkg-prebuilds")(
  prebuildRoot,
  require(bindingOptionsPath),
) as { Tesseract: TesseractConstructor };

export { Tesseract };
export default Tesseract;
