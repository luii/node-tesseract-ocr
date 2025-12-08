/*
* node-tesseract-ocr
* Copyright (C) 2025  Philipp Czarnetzki
* 
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU Affero General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
* 
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Affero General Public License for more details.
* 
* You should have received a copy of the GNU Affero General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

declare module 'node-tesseract-ocr' {

  /**
   * All available languages for tesseract
   * @readonly
   * @enum {string}
   */
  export enum AvailableLanguages {
    afr = 'afr',
    amh = 'amh',
    ara = 'ara',
    asm = 'asm',
    aze = 'aze',
    aze_cyrl = 'aze_cyrl',
    bel = 'bel',
    ben = 'ben',
    bod = 'bod',
    bos = 'bos',
    bre = 'bre',
    bul = 'bul',
    cat = 'cat',
    ceb = 'ceb',
    ces = 'ces',
    chi_sim = 'chi_sim',
    chi_tra = 'chi_tra',
    chr = 'chr',
    cos = 'cos',
    cym = 'cym',
    dan = 'dan',
    deu = 'deu',
    deu_latf = 'deu_latf',
    div = 'div',
    dzo = 'dzo',
    ell = 'ell',
    eng = 'eng',
    enm = 'enm',
    epo = 'epo',
    equ = 'equ',
    est = 'est',
    eus = 'eus',
    fao = 'fao',
    fas = 'fas',
    fil = 'fil',
    fin = 'fin',
    fra = 'fra',
    frm = 'frm',
    fry = 'fry',
    gla = 'gla',
    gle = 'gle',
    glg = 'glg',
    grc = 'grc',
    guj = 'guj',
    hat = 'hat',
    heb = 'heb',
    hin = 'hin',
    hrv = 'hrv',
    hun = 'hun',
    hye = 'hye',
    iku = 'iku',
    ind = 'ind',
    isl = 'isl',
    ita = 'ita',
    ita_old = 'ita_old',
    jav = 'jav',
    jpn = 'jpn',
    kan = 'kan',
    kat = 'kat',
    kat_old = 'kat_old',
    kaz = 'kaz',
    khm = 'khm',
    kir = 'kir',
    kmr = 'kmr',
    kor = 'kor',
    kor_vert = 'kor_vert',
    kur = 'kur',
    lao = 'lao',
    lat = 'lat',
    lav = 'lav',
    lit = 'lit',
    ltz = 'ltz',
    mal = 'mal',
    mar = 'mar',
    mkd = 'mkd',
    mlt = 'mlt',
    mon = 'mon',
    mri = 'mri',
    msa = 'msa',
    mya = 'mya',
    nep = 'nep',
    nld = 'nld',
    nor = 'nor',
    oci = 'oci',
    ori = 'ori',
    osd = 'osd',
    pan = 'pan',
    pol = 'pol',
    por = 'por',
    pus = 'pus',
    que = 'que',
    ron = 'ron',
    rus = 'rus',
    san = 'san',
    sin = 'sin',
    slk = 'slk',
    slv = 'slv',
    snd = 'snd',
    spa = 'spa',
    spa_old = 'spa_old',
    sqi = 'sqi',
    srp = 'srp',
    srp_latn = 'srp_latn',
    sun = 'sun',
    swa = 'swa',
    swe = 'swe',
    syr = 'syr',
    tam = 'tam',
    tat = 'tat',
    tel = 'tel',
    tgk = 'tgk',
    tha = 'tha',
    tir = 'tir',
    ton = 'ton',
    tur = 'tur',
    uig = 'uig',
    ukr = 'ukr',
    urd = 'urd',
    uzb = 'uzb',
    uzb_cyrl = 'uzb_cyrl',
    vie = 'vie',
    yid = 'yid',
    yor = 'yor'
  }

  /**
   * When Tesseract/Cube is initialized we can choose to instantiate/load/run
   * only the Tesseract part, only the Cube part or both along with the combiner.
   * The preference of which engine to use is stored in tessedit_ocr_engine_mode.
   * @readonly
   * @enum {number}
   */
  export enum OcrEngineMode {
    // Run Tesseract only - fastest; deprecated
    OEM_TESSERACT_ONLY = 0,
    // Run just the LSTM line recognizer.
    OEM_LSTM_ONLY = 1,
    // Run the LSTM recognizer, but allow fallback
    // to Tesseract when things get difficult.
    // deprecated
    OEM_TESSERACT_LSTM_COMBINED = 2,
    // Specify this mode when calling init_*(),
    // to indicate that any of the above modes
    // should be automatically inferred from the
    // variables in the language-specific config,
    // command-line configs, or if not specified
    // in any of the above should be set to the
    // default OEM_TESSERACT_ONLY.
    OEM_DEFAULT = 3,
  }


  /**
   * Possible modes for page layout analysis.
   * @readonly 
   * @enum {number}
   */
  export enum PageSegmentationMode {
    // Orientation and script detection only.
    PSM_OSD_ONLY = 0,
    // Automatic page segmentation with orientation and script detection. (OSD)
    PSM_AUTO_OSD = 1,
    // Automatic page segmentation, but no OSD, or OCR.
    PSM_AUTO_ONLY = 2,
    // Fully automatic page segmentation, but no OSD.
    PSM_AUTO = 3,
    // Assume a single column of text of variable sizes.
    PSM_SINGLE_COLUMN = 4,
    // Assume a single uniform block of vertically aligned text.
    PSM_SINGLE_BLOCK_VERT_TEXT = 5,
    // Assume a single uniform block of text. (Default.)
    PSM_SINGLE_BLOCK = 6,
    // Treat the image as a single text line.
    PSM_SINGLE_LINE = 7,
    // Treat the image as a single word.
    PSM_SINGLE_WORD = 8,
    // Treat the image as a single word in a circle.
    PSM_CIRCLE_WORD = 9,
    // Treat the image as a single character.
    PSM_SINGLE_CHAR = 10,
    // Find as much text as possible in no particular order.
    PSM_SPARSE_TEXT = 11,
    // Sparse text with orientation and script det.
    PSM_SPARSE_TEXT_OSD = 12,
    // Treat the image as a single text line, bypassing hacks that are Tesseract-specific.
    PSM_RAW_LINE = 13,
  }

  /**
   * Tesseract constructor options
   */
  export interface TesseractOptions {
    /**
     * Its generally safer to use as few languages as possible.
     * The more languages Tesseract needs to load the longer it takes to recognize a image.
     * @public
     */
    lang: Array<keyof typeof AvailableLanguages>;

    /**
     * Skip Ocr for when you only want to (for example) analyze the layout
     * @property {boolean} [skipOcr]
     */
    skipOcr?: boolean;

    /**
     * OCR Engine Modes
     * The engine mode cannot be changed after creating the instance
     * If another mode is needed, its advised to create a new instance.
     * @throws {Error} Will throw an error when oem mode is below 0 or over 3
     */
    oemMode?: OcrEngineMode;

    /**
     * Page Segmentation Modes
     * The page segmentation mode cannot be changed after creating the instance
     * If another mode is needed, its advised to create a new instance.
     * @throws {Error} Will throw a error when psm mode is below 0 or over 13
     */
    psm?: PageSegmentationMode;
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

  /**
   * Progress callback thats called during `recognize`
   */
  export type RecognizeProgressChangedCallback = (info: ProgressChangedInfo) => void;

  export interface RecognizeOptions {

    /**
     * @property {RecognizeProgressChangedCallback} [progressChanged]
     */
    progressChanged?: RecognizeProgressChangedCallback;
  }

  export interface RecognizeResult {

    /**
     * Returns the recognized text as utf8
     * @returns {string}
     */
    getText: () => string;

    /**
     * Returns the recognized text as tsv format (Tab seperated values)
     * @returns {string}
     */
    getTSV: () => string;

    /**
     * Returns the recognized results in an hOCR format.
     * hOCR is a derivative of the XML format and can be parsed as such.
     * The hOCR format contains layout information, so that
     * it could be overlayed over the source material for example.
     * @see https://en.wikipedia.org/wiki/HOCR
     * @returns {string}
     */
    getHOCR: () => string;

    /**
     * Returns the recognized results in an ALTO format.
     * ALTO is a derivative of the XML format and can be parsed as such.
     * The ALTO format contains layout information, so that 
     * it could be overlayed over the source material for example.
     * @see https://en.wikipedia.org/wiki/Analyzed_Layout_and_Text_Object
     * @returns {string}
     */
    getALTO: () => string;
  }

  export class Tesseract {
    constructor(options: TesseractOptions);

    /**
     * @throws  {Error} Will throw an error if no available underlying api is available
     * @throws  {Error} Will throw an error if skipOcr is on and this function is called
     * @throws  {Error} Will throw an error if the parameter at index 0 is not a buffer
     * @param   {Buffer<ArrayBuffer>} imageBuffer The image that should be recognized
     * @param   {RecognizeOptions}    [options]   Optional options, 
     * @returns {Promise<RecognizeResult>} Returns a result object containing 
     *                                     various functions to retrieve the results
     */
    recognize: (imageBuffer: Buffer<ArrayBuffer>, options?: RecognizeOptions) => Promise<RecognizeResult>;
  }

  export default Tesseract;
}
