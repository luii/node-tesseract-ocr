# Tesseract Nodejs Addon

C++ Addon für Node.js, das Tesseract OCR (über `libtesseract-dev`) in JavaScript/TypeScript bereitstellt.

Status: **WIP**

Lizenz: **AGPL-3.0**

---

## Public API

### Klasse: `Tesseract`

#### Konstruktor

```ts
new Tesseract({
  dataPath: string,
  lang: string,
  skipOcr: boolean
})
```

##### recognize

```ts
recognize(buffer: Buffer, RecognizeOptions) => Promise<{
  getText() => string,
  getHOCR() => string,
  getTSV() => string,
}>

```

###### Recognize Options

```ts
{
  progressChanged?: ({ 
    progress: number, 
    ocrAlive: number, 
    bbox: { 
    top: number, 
    right: number, 
    bottom: number, 
    left: number 
  }) => void,
   
}
```

---

## Vorraussetzungen

- Nodejs
- python3 (für `node-gyp`)
- node-addon-api
- c++ build-toolchain (bspw. build-essentials)
- libtesseract-dev
- libleptonica-dev
- Tesseract-Trainingsdaten (eng bzw. deu)

> Siehe [Installation](#installation)

---

## Installation

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

---

## Starten

```bash
npm run dev
```

---

## Beispiel

```ts
import { readFileSync } from "fs";
const { Tesseract } = require("../build/Release/<addon_name>.node");

async function main() {
  const img = readFileSync("test.png");

  const t = new Tesseract({
    dataPath: "/usr/share/tesseract-ocr/4.00/tessdata",
    lang: "eng",
    skipOcr: false
  });

  const result = await t.recognize(img, {
    progressChanged: (p) => {
      console.log("progress:", p);
    }
  });

  console.log("TEXT:", result.getText());
  console.log("HOCR:", result.getHOCR());
  console.log("TSV:", result.getTSV());
}

main();
```

## Special Thanks

- Stunt3000
