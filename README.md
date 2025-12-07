# Tesseract Nodejs Addon

C++ Addon für Node.js, das Tesseract OCR (über `libtesseract-dev`) in JavaScript/TypeScript bereitstellt.

Status: **WIP**
Lizenz: **GPL**

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
recognize(buffer: Buffer, {
  progressChanged?: ({ progress: number, ocrAlive: number, bbox: { top: number, right: number, bottom: number, left: number } }) => Promise<{
    getText() => string,
    getHOCR() => string,
    getTSV() => string,
  }>
})
```

---

## Vorraussetzungen

- Nodejs
- node-gyp + c(++) build-toolchain (bspw. build-essentials)
- libtesseract-dev (5.x.x)
- Tesseract-Trainingsdaten (eng bzw. deu)

---

## Installation

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
