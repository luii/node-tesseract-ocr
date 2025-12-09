# node-tesseract-ocr

C++ Addon for Node.js, that uses Tesseract OCR (`libtesseract-dev`) in JavaScript/TypeScript.

Status: **WIP**

Lizenz: **AGPL-3.0**

## Public API

### Class: `Tesseract`

#### TesseractOptions

```ts
 {
    /**
     * Its generally safer to use as few languages as possible.
     * The more languages Tesseract needs to load the longer 
     * it takes to recognize a image.
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
```

#### constructor(options: TesseractOptions)

```ts
new Tesseract({
  lang: string,
  skipOcr: boolean
})
```

##### recognize

```ts
recognize(buffer: Buffer, RecognizeOptions) => Promise<{
  getText() => string;
  getHOCR() => string;
  getTSV() => string;
  getALTO() => string;
}>

```

###### Recognize Options

```ts
{
  progressChanged?: ({ 
    progress: number; 
    ocrAlive: number;
    top: number; 
    right: number; 
    bottom: number; 
    left: number;
  }) => void,
   
}
```

## Prerequisities

- nodejs
- python3 (for `node-gyp`)
- node-addon-api
- c++ build-toolchain (e.g. build-essentials)
- libtesseract-dev
- libleptonica-dev
- Tesseract Training-data (eng, deu, ...)

> See [Install](#install)

## Install

```bash
sudo apt update
sudo apt install -y nodejs npm build-essential python3 pkg-config libtesseract-dev libleptonica-dev tesseract-ocr-eng
```

```bash
git clone git@github.com:luii/node-tesseract-ocr.git
cd node-tesseract-ocr
npm install
```

## Build

```bash
npm run build
```

## Start

Either set the `NODE_TESSERACT_DATAPATH` beforehand or do it in one go, it needs to point to where the training data is located.
On a standard install this is usually `/usr/share/tesseract-ocr/5/tessdata`

```bash
NODE_TESSERACT_DATAPATH=/usr/share/tesseract-ocr/5/tessdata npm run dev
```

## Examples

```bash
NODE_TESSERACT_DATAPATH=/usr/share/tesseract-ocr/5/tessdata npm run examples:recognize
```

## Special Thanks

- **Stunt3000**
