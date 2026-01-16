# node-tesseract-ocr

Native C++ addon for Node.js that exposes Tesseract OCR (`libtesseract-dev`) to JavaScript/TypeScript.

## Features

- Native bindings to Tesseract (prebuilds via `pkg-prebuilds`)
- Access to Tesseract enums and configuration from TypeScript
- Progress callback and multiple output formats

## Prerequisites

- nodejs
- node-addon-api
- c++ build toolchain (e.g. build-essentials)
- libtesseract-dev
- libleptonica-dev
- Tesseract training data (eng, deu, ...)

> See [Install](#install)

## Install

```bash
sudo apt update
sudo apt install -y nodejs npm build-essential pkg-config libtesseract-dev libleptonica-dev tesseract-ocr-eng
```

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

If you install traineddata files manually, make sure `NODE_TESSERACT_DATAPATH` points to the directory that contains them (for example `/usr/share/tesseract-ocr/5/tessdata`).

## Build

```bash
# Debug build (native addon + TS outputs)
npm run build:debug

# Release build
npm run build:release
```

## Start

Set `NODE_TESSERACT_DATAPATH` to your traineddata directory (usually `/usr/share/tesseract-ocr/5/tessdata`).

```sh
env NODE_TESSERACT_DATAPATH=/usr/share/tesseract-ocr/5/tessdata node path/to/your/app.js
```

## Scripts

```bash
# Build native addon + TS outputs (debug / release)
npm run build:debug
npm run build:release

# Build precompiled binaries for distribution
npm run prebuild

# Run the JS example (builds debug first)
npm run example:recognize

# Tests
npm run test:cpp
npm run test:js
npm run test:js:watch
```

## Examples

```sh
env NODE_TESSERACT_DATAPATH=/usr/share/tesseract-ocr/5/tessdata npm run example:recognize
```

## Public API

### Enums

#### `AvailableLanguages`

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

| Field                   | Type                              | Optional | Default     | Description                                       |
| ----------------------- | --------------------------------- | -------- | ----------- | ------------------------------------------------- |
| `lang`                  | [`Language`](#availablelanguages) | Yes      | `undefined` | Language to load (single or `lang1+lang2`).       |
| `oem`                   | [`OcrEngineMode`](#ocrenginemode) | Yes      | `undefined` | OCR engine mode.                                  |
| `setOnlyNonDebugParams` | `boolean`                         | Yes      | `undefined` | If true, only non-debug params are set.           |
| `configs`               | `Array<string>`                   | Yes      | `undefined` | Tesseract config files to apply.                  |
| `varsVec`               | `Array<string>`                   | Yes      | `undefined` | Variable names to set.                            |
| `varsValues`            | `Array<string>`                   | Yes      | `undefined` | Variable values to set (same order as `varsVec`). |

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

#### init

Initializes Tesseract with language, engine mode, configs, and variables.

| Name    | Type                                            | Optional | Default | Description             |
| ------- | ----------------------------------------------- | -------- | ------- | ----------------------- |
| options | [`TesseractInitOptions`](#tesseractinitoptions) | No       | n/a     | Initialization options. |

```ts
init(options: TesseractInitOptions): Promise<void>
```

#### initForAnalysePage

Initializes for layout analysis only.

```ts
initForAnalysePage(): Promise<void>
```

#### analysePage

Runs the layout analysis.

| Name              | Type    | Optional | Default | Description                     |
| ----------------- | ------- | -------- | ------- | ------------------------------- |
| mergeSimilarWords | boolean | No       | n/a     | Whether to merge similar words. |

```ts
analysePage(mergeSimilarWords: boolean): Promise<void>
```

#### setPageMode

Sets the page segmentation mode.

| Name | Type                                             | Optional | Default | Description             |
| ---- | ------------------------------------------------ | -------- | ------- | ----------------------- |
| psm  | [`PageSegmentationMode`](#pagesegmentationmodes) | No       | n/a     | Page segmentation mode. |

```ts
setPageMode(psm: PageSegmentationMode): Promise<void>
```

#### setVariable

Sets a Tesseract variable. Returns the Tesseract status code.

| Name  | Type   | Optional | Default | Description     |
| ----- | ------ | -------- | ------- | --------------- |
| name  | string | No       | n/a     | Variable name.  |
| value | string | No       | n/a     | Variable value. |

```ts
setVariable(name: string, value: string): Promise<number>
```

#### getIntVariable

Reads an integer variable from Tesseract.

| Name | Type   | Optional | Default | Description    |
| ---- | ------ | -------- | ------- | -------------- |
| name | string | No       | n/a     | Variable name. |

```ts
getIntVariable(name: string): Promise<number>
```

#### getBoolVariable

Reads a boolean variable from Tesseract. Returns `0` or `1`.

| Name | Type   | Optional | Default | Description    |
| ---- | ------ | -------- | ------- | -------------- |
| name | string | No       | n/a     | Variable name. |

```ts
getBoolVariable(name: string): Promise<number>
```

#### getDoubleVariable

Reads a double variable from Tesseract.

| Name | Type   | Optional | Default | Description    |
| ---- | ------ | -------- | ------- | -------------- |
| name | string | No       | n/a     | Variable name. |

```ts
getDoubleVariable(name: string): Promise<number>
```

#### getStringVariable

Reads a string variable from Tesseract.

| Name | Type   | Optional | Default | Description    |
| ---- | ------ | -------- | ------- | -------------- |
| name | string | No       | n/a     | Variable name. |

```ts
getStringVariable(name: string): Promise<string>
```

#### setImage

Sets the image from a Buffer.

| Name   | Type   | Optional | Default | Description |
| ------ | ------ | -------- | ------- | ----------- |
| buffer | Buffer | No       | n/a     | Image data. |

```ts
setImage(buffer: Buffer): Promise<void>
```

#### setRectangle

Sets the image region using coordinates and size.

| Name    | Type                                                            | Optional | Default | Description        |
| ------- | --------------------------------------------------------------- | -------- | ------- | ------------------ |
| options | [`TesseractSetRectangleOptions`](#tesseractsetrectangleoptions) | No       | n/a     | Region definition. |

```ts
setRectangle(options: TesseractSetRectangleOptions): Promise<void>
```

#### setSourceResolution

Sets the source resolution in PPI.

| Name | Type   | Optional | Default | Description      |
| ---- | ------ | -------- | ------- | ---------------- |
| ppi  | number | No       | n/a     | Pixels per inch. |

```ts
setSourceResolution(ppi: number): Promise<void>
```

#### recognize

Starts OCR and calls the callback with progress info.

| Name             | Type                                                          | Optional | Default | Description        |
| ---------------- | ------------------------------------------------------------- | -------- | ------- | ------------------ |
| progressCallback | (info: [`ProgressChangedInfo`](#progresschangedinfo)) => void | No       | n/a     | Progress callback. |

```ts
recognize(progressCallback: (info: ProgressChangedInfo) => void): Promise<void>
```

#### getUTF8Text

Returns recognized text as UTF-8.

```ts
getUTF8Text(): Promise<string>
```

#### getHOCRText

Returns HOCR output. Optional progress callback and page number.

| Name             | Type                                                          | Optional | Default   | Description            |
| ---------------- | ------------------------------------------------------------- | -------- | --------- | ---------------------- |
| progressCallback | (info: [`ProgressChangedInfo`](#progresschangedinfo)) => void | Yes      | undefined | Progress callback.     |
| pageNumber       | number                                                        | Yes      | undefined | Page number (0-based). |

```ts
getHOCRText(
  progressCallback?: (info: ProgressChangedInfo) => void,
  pageNumber?: number,
): Promise<string>
```

#### getTSVText

Returns TSV output.

```ts
getTSVText(): Promise<string>
```

#### getUNLVText

Returns UNLV output.

```ts
getUNLVText(): Promise<string>
```

#### getALTOText

Returns ALTO output. Optional progress callback and page number.

| Name             | Type                                                          | Optional | Default   | Description            |
| ---------------- | ------------------------------------------------------------- | -------- | --------- | ---------------------- |
| progressCallback | (info: [`ProgressChangedInfo`](#progresschangedinfo)) => void | Yes      | undefined | Progress callback.     |
| pageNumber       | number                                                        | Yes      | undefined | Page number (0-based). |

```ts
getALTOText(
  progressCallback?: (info: ProgressChangedInfo) => void,
  pageNumber?: number,
): Promise<string>
```

#### detectOrientationScript

Detects orientation and script with confidences. Returns [`DetectOrientationScriptResult`](#detectorientationscriptresult).

```ts
detectOrientationScript(): Promise<DetectOrientationScriptResult>
```

#### meanTextConf

Mean text confidence (0-100).

```ts
meanTextConf(): Promise<number>
```

#### getInitLanguages

Returns [`Language`](#availablelanguages) in raw Tesseract format.

```ts
getInitLanguages(): Promise<Language>
```

#### getLoadedLanguages

Returns [`AvailableLanguage[]`](#availablelanguages) in raw Tesseract format.

```ts
getLoadedLanguages(): Promise<AvailableLanguage[]>
```

#### getAvailableLanguages

Returns [`AvailableLanguage[]`](#availablelanguages) in raw Tesseract format.

```ts
getAvailableLanguages(): Promise<AvailableLanguage[]>
```

#### clear

Clears internal state.

```ts
clear(): Promise<void>
```

#### end

Ends the instance.

```ts
end(): Promise<void>
```

## Example

```ts
import fs from "node:fs";
import Tesseract, { OcrEngineModes } from "node-tesseract-ocr";

async function main() {
  const tesseract = new Tesseract();
  await tesseract.init({
    lang: "eng",
    oem: OcrEngineModes.OEM_LSTM_ONLY,
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

## License

Apache-2.0. See [`LICENSE.md`](/LICENSE.md) for full terms.

## Special Thanks

- **Stunt3000**
