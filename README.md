# node-tesseract-ocr

Native C++ addon for Node.js that exposes Tesseract OCR (`libtesseract-dev`) to JavaScript/TypeScript.

## Table of Contents

- [Features](#features)
- [Prerequisites](#prerequisites)
- [Install](#install)
  - [Install additional training data](#install-additional-training-data)
- [Build](#build)
- [Start](#start)
- [Scripts](#scripts)
- [Examples](#examples)
- [Public API](#public-api)
  - [Enums](#enums)
  - [Types](#types)
  - [Tesseract API](#tesseract-api)
- [License](#license)

## Features

- Native bindings to Tesseract (prebuilds via `pkg-prebuilds`)
- Access to Tesseract enums and configuration from TypeScript
- Progress callback and multiple output formats
- Lazy download of missing traineddata (configurable)

## Prerequisites

- nodejs
- node-addon-api
- c++ build toolchain (e.g. build-essentials)
- libtesseract-dev (exactly `5.5.2`)
- libleptonica-dev
- Tesseract training data (eng, deu, ...) or let the library handle that

> See [Install](#install)

## Install

```bash
sudo apt update
sudo apt install -y nodejs npm build-essential pkg-config libtesseract-dev libleptonica-dev tesseract-ocr-eng
```

Verify the required Tesseract version:

```bash
pkg-config --modversion tesseract
# expected: 5.5.2
```

If your distro ships another version, install/build `tesseract 5.5.2` and ensure `pkg-config` resolves that installation.

```bash
git clone git@github.com:luii/node-tesseract-ocr.git
cd node-tesseract-ocr
npm install
```

### Install additional training data

On Debian/Ubuntu, language data is provided as packages named `tesseract-ocr-<lang>`.
Install additional languages as needed, for example:

```bash
sudo apt install -y tesseract-ocr-deu tesseract-ocr-eng tesseract-ocr-jpn
```

If you install traineddata files manually, make sure `TESSDATA_PREFIX` points to the directory that contains them (for example `/usr/share/tessdata`).

If traineddata is missing, this package will download it lazily during `init` by default. You can control this behavior via `ensureTraineddata`, `cachePath`, and `dataPath`.

## Build

```bash
# Debug build (native addon + TS outputs)
npm run build:debug

# Release build
npm run build:release
```

## Start

Set `TESSDATA_PREFIX` to your traineddata directory (usually `/usr/share/tesseract-ocr/5/tessdata` or `/usr/share/tessdata`).

```sh
env TESSDATA_PREFIX=/usr/share/tessdata node path/to/your/app.js
```

If you prefer automatic downloads, you can skip setting `TESSDATA_PREFIX` and let the default cache directory handle traineddata on first use.

## Scripts

```bash
# Build native addon + TS outputs (debug / release)
npm run build:debug
npm run build:release

# Run the JS example (builds debug first)
npm run example:recognize

# Tests
npm run test:cpp
npm run test:js
npm run test:js:watch
```

## Examples

### Run Included Example

```sh
env TESSDATA_PREFIX=/usr/share/tessdata npm run example:recognize
```

### Basic OCR (Local Traineddata)

You can find a similar example in the `examples/` folder of the project.

```ts
import fs from "node:fs";
import Tesseract, { OcrEngineModes } from "node-tesseract-ocr";

process.env.TESSDATA_PREFIX = "/usr/share/tessdata/";

async function main() {
  const tesseract = new Tesseract();
  await tesseract.init({
    langs: ["eng"],
  });

  const buffer = fs.readFileSync("example1.png");
  await tesseract.setImage(buffer);
  await tesseract.recognize((info) => {
    console.log(`Progress: ${info.percent}%`);
  });

  const text = await tesseract.getUTF8Text();
  console.log(text);

  await tesseract.end();
}

main().catch((err) => {
  console.error(err);
  process.exit(1);
});
```

### Lazy Traineddata Download (Default)

```ts
import fs from "node:fs";
import Tesseract from "node-tesseract-ocr";

async function main() {
  const tesseract = new Tesseract();
  await tesseract.init({
    langs: ["eng"],
    ensureTraineddata: true
    dataPath: './tessdata-local'
  });

  const buffer = fs.readFileSync("example1.png");
  await tesseract.setImage(buffer);
  await tesseract.recognize();
  const text = await tesseract.getUTF8Text();
  console.log(text);

  await tesseract.end();
}

main().catch((err) => {
  console.error(err);
  process.exit(1);
});
```

## Public API

### Enums

#### `Language`

Mapping of available Tesseract language codes. Most are [_3-letter ISO 639-2/T style_](https://en.wikipedia.org/wiki/List_of_ISO_639-2_codes) (e.g. `eng`, `deu`, `jpn`), with Tesseract-specific variants such as `chi_sim`, `deu_latf`, or `osd`. The value must match the installed traineddata filename (without the `.traineddata` suffix). You can pass a single code via `TesseractInitOptions.lang`.

> [!IMPORTANT]
> **If you join codes with a plus sign (e.g. `deu+eng`), Tesseract will look for multiple languages in the same image (here: German and English).**

#### `OcrEngineMode`

Full list of OCR engine modes from Tesseract.

| Name                          | Value | Deprecated | Description                                                |
| ----------------------------- | ----- | ---------- | ---------------------------------------------------------- |
| `OEM_TESSERACT_ONLY`          | 0     | Yes        | Run Tesseract only (fastest).                              |
| `OEM_LSTM_ONLY`               | 1     | No         | Run only the LSTM line recognizer.                         |
| `OEM_TESSERACT_LSTM_COMBINED` | 2     | Yes        | Run LSTM with fallback to Tesseract.                       |
| `OEM_DEFAULT`                 | 3     | No         | Infer engine mode from configs; default is Tesseract-only. |

#### `PageSegmentationMode`

Full list of page segmentation modes from Tesseract.

| Name                         | Value | Deprecated | Description                                               |
| ---------------------------- | ----- | ---------- | --------------------------------------------------------- |
| `PSM_OSD_ONLY`               | 0     | No         | Orientation and script detection only.                    |
| `PSM_AUTO_OSD`               | 1     | No         | Automatic page segmentation with OSD.                     |
| `PSM_AUTO_ONLY`              | 2     | No         | Automatic page segmentation, no OSD or OCR.               |
| `PSM_AUTO`                   | 3     | No         | Fully automatic page segmentation, no OSD.                |
| `PSM_SINGLE_COLUMN`          | 4     | No         | Assume a single column of text of variable sizes.         |
| `PSM_SINGLE_BLOCK_VERT_TEXT` | 5     | No         | Assume a single uniform block of vertically aligned text. |
| `PSM_SINGLE_BLOCK`           | 6     | No         | Assume a single uniform block of text (default).          |
| `PSM_SINGLE_LINE`            | 7     | No         | Treat the image as a single text line.                    |
| `PSM_SINGLE_WORD`            | 8     | No         | Treat the image as a single word.                         |
| `PSM_CIRCLE_WORD`            | 9     | No         | Treat the image as a single word in a circle.             |
| `PSM_SINGLE_CHAR`            | 10    | No         | Treat the image as a single character.                    |
| `PSM_SPARSE_TEXT`            | 11    | No         | Find as much text as possible in no particular order.     |
| `PSM_SPARSE_TEXT_OSD`        | 12    | No         | Sparse text with orientation and script detection.        |
| `PSM_RAW_LINE`               | 13    | No         | Single text line, bypassing Tesseract-specific hacks.     |

### Types

#### `TesseractInitOptions`

| Field                   | Type                                                                                                  | Optional | Default                                | Description                             |
| ----------------------- | ----------------------------------------------------------------------------------------------------- | -------- | -------------------------------------- | --------------------------------------- |
| `langs`                 | [`Language[]`](#availablelanguages)                                                                   | Yes      | `undefined`                            | Languages to load as an array.          |
| `oem`                   | [`OcrEngineMode`](#ocrenginemode)                                                                     | Yes      | `undefined`                            | OCR engine mode.                        |
| `vars`                  | `Partial<Record<keyof ConfigurationVariables, ConfigurationVariables[keyof ConfigurationVariables]>>` | Yes      | `undefined`                            | Variables to set.                       |
| `configs`               | `Array<string>`                                                                                       | Yes      | `undefined`                            | Tesseract config files to apply.        |
| `setOnlyNonDebugParams` | `boolean`                                                                                             | Yes      | `undefined`                            | If true, only non-debug params are set. |
| `ensureTraineddata`     | `boolean`                                                                                             | Yes      | `true`                                 | Download missing traineddata lazily.    |
| `cachePath`             | `string`                                                                                              | Yes      | `~/.cache/node-tesseract-ocr/tessdata` | Cache directory for downloads.          |
| `dataPath`              | `string`                                                                                              | Yes      | `TESSDATA_PREFIX` or `cachePath`       | Directory used by Tesseract for data.   |
| `progressCallback`      | `(info: TrainingDataDownloadProgress) => void`                                                        | Yes      | `undefined`                            | Download progress callback.             |

#### `TesseractSetRectangleOptions`

| Field    | Type     | Optional | Default | Description       |
| -------- | -------- | -------- | ------- | ----------------- |
| `top`    | `number` | No       | n/a     | Top coordinate.   |
| `left`   | `number` | No       | n/a     | Left coordinate.  |
| `width`  | `number` | No       | n/a     | Rectangle width.  |
| `height` | `number` | No       | n/a     | Rectangle height. |

#### `ProgressChangedInfo`

| Field      | Type     | Optional | Default | Description                                |
| ---------- | -------- | -------- | ------- | ------------------------------------------ |
| `progress` | `number` | No       | n/a     | Chars in the current buffer.               |
| `percent`  | `number` | No       | n/a     | Percent complete (0-100).                  |
| `ocrAlive` | `number` | No       | n/a     | Non-zero if worker is alive.               |
| `top`      | `number` | No       | n/a     | Top coordinate of current element bbox.    |
| `right`    | `number` | No       | n/a     | Right coordinate of current element bbox.  |
| `bottom`   | `number` | No       | n/a     | Bottom coordinate of current element bbox. |
| `left`     | `number` | No       | n/a     | Left coordinate of current element bbox.   |

#### `TesseractProcessPagesStatus`

| Field             | Type      | Optional | Default | Description                                           |
| ----------------- | --------- | -------- | ------- | ----------------------------------------------------- |
| `active`          | `boolean` | No       | n/a     | Whether a multipage session is currently active.      |
| `healthy`         | `boolean` | No       | n/a     | Whether the renderer is healthy.                      |
| `processedPages`  | `number`  | No       | n/a     | Number of pages already processed in this session.    |
| `nextPageIndex`   | `number`  | No       | n/a     | Zero-based index that will be used for the next page. |
| `outputBase`      | `string`  | No       | n/a     | Effective output base used by the PDF renderer.       |
| `timeoutMillisec` | `number`  | No       | n/a     | Timeout per page in milliseconds (`0` = unlimited).   |
| `textonly`        | `boolean` | No       | n/a     | Whether text-only PDF mode is enabled.                |

#### `DetectOrientationScriptResult`

| Field                   | Type     | Optional | Default | Description                                        |
| ----------------------- | -------- | -------- | ------- | -------------------------------------------------- |
| `orientationDegrees`    | `number` | No       | n/a     | Orientation of the source image (0, 90, 180, 270). |
| `orientationConfidence` | `number` | No       | n/a     | Confidence for the orientation.                    |
| `scriptName`            | `string` | No       | n/a     | Detected script name.                              |
| `scriptConfidence`      | `number` | No       | n/a     | Confidence for the script.                         |

### Tesseract API

#### Constructor

```ts
new Tesseract();
```

Creates a new Tesseract instance.

#### Initialization Requirements

Call `init(...)` once before using OCR/engine-dependent methods.

Methods that do **not** require `init(...)`:

- `version()`
- `isInitialized()`
- `setInputName(...)`
- `getInputName()`
- `abortProcessPages()`
- `getProcessPagesStatus()`
- `document.abort()`
- `document.status()`
- `init(...)`
- `end()`

Methods that **require** `init(...)`:

- `setInputImage(...)`
- `getInputImage()`
- `getSourceYResolution()`
- `getDataPath()`
- `setOutputName(...)`
- `clearPersistentCache()`
- `clearAdaptiveClassifier()`
- `setImage(...)`
- `getThresholdedImage()`
- `getThresholdedImageScaleFactor()`
- `setPageMode(...)`
- `setRectangle(...)`
- `setSourceResolution(...)`
- `recognize(...)`
- `detectOrientationScript()`
- `meanTextConf()`
- `allWordConfidences()`
- `getPAGEText(...)`
- `getLSTMBoxText(...)`
- `getBoxText(...)`
- `getWordStrBoxText(...)`
- `getOSDText(...)`
- `getUTF8Text()`
- `getHOCRText(...)`
- `getTSVText(...)`
- `getUNLVText()`
- `getALTOText(...)`
- `getInitLanguages()`
- `getLoadedLanguages()`
- `getAvailableLanguages()`
- `setDebugVariable(...)`
- `setVariable(...)`
- `getIntVariable(...)`
- `getBoolVariable(...)`
- `getDoubleVariable(...)`
- `getStringVariable(...)`
- `clear()`
- `beginProcessPages(...)`
- `addProcessPage(...)`
- `finishProcessPages()`
- `document.begin(...)`
- `document.addPage(...)`
- `document.finish()`

#### version

Returns the currently loaded libtesseract version string.

```ts
version(): Promise<string>
```

#### isInitialized

Returns whether `init(...)` has already completed successfully and has not been reset via `end()`.

```ts
isInitialized(): Promise<boolean>
```

#### init

Initializes the OCR engine with language, OEM, configs, and variables.

| Name      | Type                                            | Optional | Default | Description             |
| --------- | ----------------------------------------------- | -------- | ------- | ----------------------- |
| `options` | [`TesseractInitOptions`](#tesseractinitoptions) | No       | n/a     | Initialization options. |

```ts
init(options: TesseractInitOptions): Promise<void>
```

#### initForAnalysePage

Initializes the engine in analysis-only mode.

```ts
initForAnalysePage(): Promise<void>
```

#### analyseLayout

Runs page layout analysis on the current image.

| Name                | Type      | Optional | Default | Description                                 |
| ------------------- | --------- | -------- | ------- | ------------------------------------------- |
| `mergeSimilarWords` | `boolean` | No       | n/a     | Merge similar words during layout analysis. |

```ts
analyseLayout(mergeSimilarWords: boolean): Promise<void>
```

#### setInputName

Sets the source/input name used by renderer/training APIs.

| Name        | Type     | Optional | Default | Description                                |
| ----------- | -------- | -------- | ------- | ------------------------------------------ |
| `inputName` | `string` | No       | n/a     | Input name used by renderer/training APIs. |

```ts
setInputName(inputName: string): Promise<void>
```

#### getInputName

Returns the current input name from engine state.

```ts
getInputName(): Promise<string>
```

#### setInputImage

Sets the encoded source image buffer.

| Name     | Type     | Optional | Default | Description                  |
| -------- | -------- | -------- | ------- | ---------------------------- |
| `buffer` | `Buffer` | No       | n/a     | Encoded source image buffer. |

```ts
setInputImage(buffer: Buffer): Promise<void>
```

#### getInputImage

Returns the current input image bytes.

```ts
getInputImage(): Promise<Buffer>
```

#### getSourceYResolution

Returns source image Y resolution (DPI).

```ts
getSourceYResolution(): Promise<number>
```

#### getDataPath

Returns the active tessdata path from the engine.

```ts
getDataPath(): Promise<string>
```

#### setOutputName

Sets the output base name for renderer-based outputs.

| Name         | Type     | Optional | Default | Description                            |
| ------------ | -------- | -------- | ------- | -------------------------------------- |
| `outputName` | `string` | No       | n/a     | Output base name for renderer outputs. |

```ts
setOutputName(outputName: string): Promise<void>
```

#### clearPersistentCache

Clears global library-level caches (for example dictionaries).

```ts
clearPersistentCache(): Promise<void>
```

#### clearAdaptiveClassifier

Cleans adaptive classifier state between pages/documents.

```ts
clearAdaptiveClassifier(): Promise<void>
```

#### setImage

Sets the image used by OCR recognition.

| Name     | Type     | Optional | Default | Description              |
| -------- | -------- | -------- | ------- | ------------------------ |
| `buffer` | `Buffer` | No       | n/a     | Image data used for OCR. |

```ts
setImage(buffer: Buffer): Promise<void>
```

#### getThresholdedImage

Returns thresholded image bytes from Tesseract internals.

```ts
getThresholdedImage(): Promise<Buffer>
```

#### getThresholdedImageScaleFactor

Returns scale factor for thresholded/component images.

```ts
getThresholdedImageScaleFactor(): Promise<number>
```

#### setPageMode

Sets the page segmentation mode (PSM).

| Name  | Type                                            | Optional | Default | Description             |
| ----- | ----------------------------------------------- | -------- | ------- | ----------------------- |
| `psm` | [`PageSegmentationMode`](#pagesegmentationmode) | No       | n/a     | Page segmentation mode. |

```ts
setPageMode(psm: PageSegmentationMode): Promise<void>
```

#### setRectangle

Restricts recognition to the given rectangle.

| Name      | Type                                                            | Optional | Default | Description        |
| --------- | --------------------------------------------------------------- | -------- | ------- | ------------------ |
| `options` | [`TesseractSetRectangleOptions`](#tesseractsetrectangleoptions) | No       | n/a     | Region definition. |

```ts
setRectangle(options: TesseractSetRectangleOptions): Promise<void>
```

#### setSourceResolution

Sets the source resolution in PPI.

| Name  | Type     | Optional | Default | Description               |
| ----- | -------- | -------- | ------- | ------------------------- |
| `ppi` | `number` | No       | n/a     | Source resolution in PPI. |

```ts
setSourceResolution(ppi: number): Promise<void>
```

#### document

Facade for multipage PDF/document processing lifecycle.

```ts
document: {
  begin(options: TesseractBeginProcessPagesOptions): Promise<void>;
  addPage(buffer: Buffer, filename?: string): Promise<void>;
  finish(): Promise<string>;
  abort(): Promise<void>;
  status(): Promise<TesseractProcessPagesStatus>;
}
```

#### document.begin

Starts a multipage processing session.

| Name      | Type                                | Optional | Default | Description                 |
| --------- | ----------------------------------- | -------- | ------- | --------------------------- |
| `options` | `TesseractBeginProcessPagesOptions` | No       | n/a     | Multipage renderer options. |

```ts
document.begin(options: TesseractBeginProcessPagesOptions): Promise<void>
```

#### document.addPage

Adds an encoded page to the active session.

| Name       | Type     | Optional | Default     | Description                                                                                                                                                                                                                                                                                                                                         |
| ---------- | -------- | -------- | ----------- | --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `buffer`   | `Buffer` | No       | n/a         | Encoded page image buffer.                                                                                                                                                                                                                                                                                                                          |
| `filename` | `string` | Yes      | `undefined` | Optional source filename/path passed to Tesseract `ProcessPage` for this page. Tesseract/Leptonica may open this file internally and use it as the source image for parts of PDF rendering. If output pages look wrong (for example inverted or visually corrupted), pass a real image path here to force a stable source image path for that page. |

```ts
document.addPage(buffer: Buffer, filename?: string): Promise<void>
```

#### document.finish

Finalizes the active session and returns output PDF path.

```ts
document.finish(): Promise<string>
```

#### document.abort

Aborts and resets the active multipage session.

```ts
document.abort(): Promise<void>
```

#### document.status

Returns the current multipage session status (active flag, page counters, and effective renderer settings).

```ts
document.status(): Promise<TesseractProcessPagesStatus>
```

#### getProcessPagesStatus

Returns the current multipage session status from the instance API.

```ts
getProcessPagesStatus(): Promise<TesseractProcessPagesStatus>
```

#### setDebugVariable

Sets a debug configuration variable.

| Name    | Type                                                           | Optional | Default | Description     |
| ------- | -------------------------------------------------------------- | -------- | ------- | --------------- |
| `name`  | `keyof SetVariableConfigVariables`                             | No       | n/a     | Variable name.  |
| `value` | `SetVariableConfigVariables[keyof SetVariableConfigVariables]` | No       | n/a     | Variable value. |

```ts
setDebugVariable(
  name: keyof SetVariableConfigVariables,
  value: SetVariableConfigVariables[keyof SetVariableConfigVariables],
): Promise<boolean>
```

#### setVariable

Sets a regular configuration variable.

| Name    | Type                                                           | Optional | Default | Description     |
| ------- | -------------------------------------------------------------- | -------- | ------- | --------------- |
| `name`  | `keyof SetVariableConfigVariables`                             | No       | n/a     | Variable name.  |
| `value` | `SetVariableConfigVariables[keyof SetVariableConfigVariables]` | No       | n/a     | Variable value. |

```ts
setVariable(
  name: keyof SetVariableConfigVariables,
  value: SetVariableConfigVariables[keyof SetVariableConfigVariables],
): Promise<boolean>
```

#### getIntVariable

Reads a configuration variable as integer.

| Name   | Type                               | Optional | Default | Description    |
| ------ | ---------------------------------- | -------- | ------- | -------------- |
| `name` | `keyof SetVariableConfigVariables` | No       | n/a     | Variable name. |

```ts
getIntVariable(name: keyof SetVariableConfigVariables): Promise<number>
```

#### getBoolVariable

Reads a configuration variable as boolean (`0`/`1`).

| Name   | Type                               | Optional | Default | Description    |
| ------ | ---------------------------------- | -------- | ------- | -------------- |
| `name` | `keyof SetVariableConfigVariables` | No       | n/a     | Variable name. |

```ts
getBoolVariable(name: keyof SetVariableConfigVariables): Promise<number>
```

#### getDoubleVariable

Reads a configuration variable as double.

| Name   | Type                               | Optional | Default | Description    |
| ------ | ---------------------------------- | -------- | ------- | -------------- |
| `name` | `keyof SetVariableConfigVariables` | No       | n/a     | Variable name. |

```ts
getDoubleVariable(name: keyof SetVariableConfigVariables): Promise<number>
```

#### getStringVariable

Reads a configuration variable as string.

| Name   | Type                               | Optional | Default | Description    |
| ------ | ---------------------------------- | -------- | ------- | -------------- |
| `name` | `keyof SetVariableConfigVariables` | No       | n/a     | Variable name. |

```ts
getStringVariable(name: keyof SetVariableConfigVariables): Promise<string>
```

#### recognize

Runs OCR recognition (optionally with progress callback).

| Name               | Type                                  | Optional | Default     | Description            |
| ------------------ | ------------------------------------- | -------- | ----------- | ---------------------- |
| `progressCallback` | `(info: ProgressChangedInfo) => void` | Yes      | `undefined` | OCR progress callback. |

```ts
recognize(progressCallback?: (info: ProgressChangedInfo) => void): Promise<void>
```

#### detectOrientationScript

Detects orientation and script with confidence values.

```ts
detectOrientationScript(): Promise<DetectOrientationScriptResult>
```

#### meanTextConf

Returns mean text confidence.

```ts
meanTextConf(): Promise<number>
```

#### allWordConfidences

Returns all word confidences for current recognition result.

```ts
allWordConfidences(): Promise<number[]>
```

#### getPAGEText

Returns PAGE XML output.

| Name               | Type                                  | Optional | Default     | Description                        |
| ------------------ | ------------------------------------- | -------- | ----------- | ---------------------------------- |
| `progressCallback` | `(info: ProgressChangedInfo) => void` | Yes      | `undefined` | PAGE generation progress callback. |
| `pageNumber`       | `number`                              | Yes      | `undefined` | 0-based page number.               |

```ts
getPAGEText(
  progressCallback?: (info: ProgressChangedInfo) => void,
  pageNumber?: number,
): Promise<string>
```

#### getLSTMBoxText

Returns LSTM box output.

| Name         | Type     | Optional | Default     | Description          |
| ------------ | -------- | -------- | ----------- | -------------------- |
| `pageNumber` | `number` | Yes      | `undefined` | 0-based page number. |

```ts
getLSTMBoxText(pageNumber?: number): Promise<string>
```

#### getBoxText

Returns classic box output.

| Name         | Type     | Optional | Default     | Description          |
| ------------ | -------- | -------- | ----------- | -------------------- |
| `pageNumber` | `number` | Yes      | `undefined` | 0-based page number. |

```ts
getBoxText(pageNumber?: number): Promise<string>
```

#### getWordStrBoxText

Returns WordStr box output.

| Name         | Type     | Optional | Default     | Description          |
| ------------ | -------- | -------- | ----------- | -------------------- |
| `pageNumber` | `number` | Yes      | `undefined` | 0-based page number. |

```ts
getWordStrBoxText(pageNumber?: number): Promise<string>
```

#### getOSDText

Returns OSD text output.

| Name         | Type     | Optional | Default     | Description          |
| ------------ | -------- | -------- | ----------- | -------------------- |
| `pageNumber` | `number` | Yes      | `undefined` | 0-based page number. |

```ts
getOSDText(pageNumber?: number): Promise<string>
```

#### getUTF8Text

Returns recognized UTF-8 text.

```ts
getUTF8Text(): Promise<string>
```

#### getHOCRText

Returns hOCR output.

| Name               | Type                                  | Optional | Default     | Description                        |
| ------------------ | ------------------------------------- | -------- | ----------- | ---------------------------------- |
| `progressCallback` | `(info: ProgressChangedInfo) => void` | Yes      | `undefined` | hOCR generation progress callback. |
| `pageNumber`       | `number`                              | Yes      | `undefined` | 0-based page number.               |

```ts
getHOCRText(
  progressCallback?: (info: ProgressChangedInfo) => void,
  pageNumber?: number,
): Promise<string>
```

#### getTSVText

Returns TSV output.

| Name         | Type     | Optional | Default     | Description          |
| ------------ | -------- | -------- | ----------- | -------------------- |
| `pageNumber` | `number` | Yes      | `undefined` | 0-based page number. |

```ts
getTSVText(pageNumber?: number): Promise<string>
```

#### getUNLVText

Returns UNLV output.

```ts
getUNLVText(): Promise<string>
```

#### getALTOText

Returns ALTO XML output.

| Name         | Type     | Optional | Default     | Description          |
| ------------ | -------- | -------- | ----------- | -------------------- |
| `pageNumber` | `number` | Yes      | `undefined` | 0-based page number. |

```ts
getALTOText(pageNumber?: number): Promise<string>
```

#### getInitLanguages

Returns languages used during initialization (for example `deu+eng`).

```ts
getInitLanguages(): Promise<string>
```

#### getLoadedLanguages

Returns languages currently loaded in the engine.

```ts
getLoadedLanguages(): Promise<Language[]>
```

#### getAvailableLanguages

Returns languages available from tessdata.

```ts
getAvailableLanguages(): Promise<Language[]>
```

#### clear

Clears internal recognition state/results.

```ts
clear(): Promise<void>
```

#### end

Releases native resources and ends the instance.

```ts
end(): Promise<void>
```

## License

Apache-2.0. See [`LICENSE.md`](/LICENSE.md) for full terms.
