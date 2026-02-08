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

import { readFileSync } from "node:fs";
import {
  OcrEngineModes,
  Language,
  PageSegmentationModes,
  LogLevels,
  Tesseract,
} from "@luii/node-tesseract-ocr";

const drawProgressBar = (progress: any) => {
  const barWidth = 30;
  const filledWidth = Math.floor((progress / 100) * barWidth);
  const emptyWidth = barWidth - filledWidth;
  const progressBar = "█".repeat(filledWidth) + "▒".repeat(emptyWidth);
  return `[${progressBar}] ${progress}%`;
};

async function main() {
  const buf = readFileSync("./example8.jpg");
  const tesseract = new Tesseract();

  try {
    await tesseract.init({
      dataPath: "./traineddata-local",
      langs: [Language.eng],
      oem: OcrEngineModes.OEM_DEFAULT,
      vars: {
        log_level: LogLevels.ALL,
      },
      traineddataProgressCallback: (info) => {
        process.stdout.clearLine(1);
        process.stdout.cursorTo(0);
        process.stdout.write(
          `${info.lang} download: ${drawProgressBar(info.percent?.toFixed(2))}`,
        );
      },
    });
    await tesseract.setPageMode(PageSegmentationModes.PSM_OSD_ONLY);
    await tesseract.setImage(buf);
    await tesseract.setSourceResolution(176);
    await tesseract.getHOCRText((info) => {
      console.log(info);
    });
    const textconf = await tesseract.detectOrientationScript();
    console.log(textconf);
  } finally {
    await tesseract.end();
  }
}

main();
