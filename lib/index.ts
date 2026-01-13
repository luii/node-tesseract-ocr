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

const { Tesseract } = require("pkg-prebuilds")(
  __dirname,
  require("./binding-options"),
);

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

export { Tesseract };
export default Tesseract;
