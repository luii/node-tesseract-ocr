/*
 * Copyright 2026 Philipp Czarnetzki
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
import path from "node:path";
import { Language, OcrEngineModes, Tesseract } from "@luii/node-tesseract-ocr";

const drawProgressBar = (progress: number) => {
  const barWidth = 30;
  const filledWidth = Math.floor((progress / 100) * barWidth);
  const emptyWidth = barWidth - filledWidth;
  const progressBar = "█".repeat(filledWidth) + "▒".repeat(emptyWidth);
  return `[${progressBar}] ${progress.toFixed(2)}%`;
};

function toAbsoluteImagePath(inputPath: string): string {
  return path.isAbsolute(inputPath)
    ? inputPath
    : path.resolve(process.cwd(), inputPath);
}

async function main() {
  const imagePaths = process.argv
    .slice(2)
    .filter((arg) => !arg.startsWith("dotenv_config_"));

  if (imagePaths.length === 0) {
    console.error(
      "Usage: npm run example:multi-page -- <image-1> <image-2> [image-3 ...]",
    );
    process.exitCode = 1;
    return;
  }

  const tesseract = new Tesseract();
  const outputBase = path.resolve(process.cwd(), "dist", "multi-page-result");
  const absoluteImagePaths = imagePaths.map(toAbsoluteImagePath);

  try {
    await tesseract.init({
      dataPath: "./traineddata-local",
      langs: [Language.eng, Language.deu],
      oem: OcrEngineModes.OEM_DEFAULT,
      ensureTraineddata: true,
    });

    await tesseract.document.begin({
      outputBase,
      title: "Multi Page OCR Demo",
      timeout: 0,
      textonly: false,
    });

    for (const absolutePath of absoluteImagePaths) {
      const imageBuffer = readFileSync(absolutePath);
      await tesseract.document.addPage({
        buffer: imageBuffer,
        filename: absolutePath,
        progressCallback: (info) => {
          process.stdout.clearLine?.(0);
          process.stdout.cursorTo?.(0);
          process.stdout.write(
            `${path.basename(absolutePath)}: ${drawProgressBar(info.percent)}`,
          );
        },
      });

      process.stdout.clearLine?.(0);
      process.stdout.cursorTo?.(0);
      process.stdout.write(
        `${path.basename(absolutePath)}: ${drawProgressBar(100)}`,
      );
      process.stdout.write("\n");

      const status = await tesseract.document.status();
      console.log(
        `added page ${status.processedPages}: ${absolutePath} (next=${status.nextPageIndex})`,
      );
    }

    const statusBeforeFinish = await tesseract.getProcessPagesStatus();
    console.log("status before finish:", statusBeforeFinish);

    const pdfPath = await tesseract.document.finish();
    console.log(`multi-page pdf written to: ${pdfPath}`);
  } catch (error) {
    await tesseract.document.abort().catch(() => {});
    throw error;
  } finally {
    await tesseract.end();
  }
}

main().catch((error) => {
  console.error(error);
  process.exit(1);
});
