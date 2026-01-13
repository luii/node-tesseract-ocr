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

export declare const AvailableLanguages: {};

declare module "@luii/node-tesseract-ocr" {
  export interface TesseractInitOptions {}

  /**
   * Tesseract init options
   */
  export interface TesseractInitOptions {
    /**
     * Its generally safer to use as few languages as possible.
     * The more languages Tesseract needs to load the longer it takes to recognize a image.
     * @public
     */
    lang?: AvailableLanguages;

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

  export class Tesseract {
    constructor();
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
    getInitLanguages: () => Promise<any>;
    getLoadedLanguages: () => Promise<any>;
    getAvailableLanguages: () => Promise<any>;
    clear: () => Promise<void>;
    end: () => Promise<void>;
  }

  export default Tesseract;
}
