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

import { readFileSync } from "node:fs";
import { Tesseract } from 'node-tesseract-ocr';


async function main() {
  const buf = readFileSync("./eng_bw.png");
  const t = new Tesseract({
    skipOcr: false,
    lang: ['eng']
  });

  t.recognize(buf, {
    progressChanged: (info) => console.log(info)
  })
    .then((result) => result.getText())
    .then(console.log);
}

main();
