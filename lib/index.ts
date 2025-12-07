import { readFileSync } from "node:fs";
const { Tesseract } = require('../build/Debug/tesseract_ocr_addon.node');

// const buf = readFileSync("./eng_bw.png");
const buf = readFileSync("./example5.png");

const t = new Tesseract({
  dataPath: "/usr/share/tesseract-ocr/5/tessdata",
  lang: 'deu',
  skipOcr: false,
});

const abortController = new AbortController();

t.recognize(buf, {
  abortSignal: abortController.signal,
  progressChanged: (info: any) => {
    console.count('called');
    console.log(info);
  }
}).then((res: any) => res.getText()).then(console.log);

