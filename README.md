## Functions

[x] **recognize(buffer, options?)**
  Gleich wie oben, aber direkt aus einem Buffer.
[x] **recognizeBatch(jobs, options?)**
  Führt mehrere OCR-Jobs in einem Lauf aus, liefert Ergebnisse pro Job.
[x] **detectOrientationAndScript(pathOrBuffer)**
  Erkennt Drehwinkel und dominante Schrift/Sprache.
[x] **getText(pathOrBuffer, options?)**
  Gibt nur den reinen UTF-8-Text zurück.
[x] **getHOCR(pathOrBuffer, options?)**
  Gibt hOCR-HTML‐Struktur (Text + BBox + Confidences) zurück.
[x] **getTSV(pathOrBuffer, options?)**
  Liefert TSV-Format mit Zeilen-/Wort-Infos.
[x] **getBoxData(pathOrBuffer, options?)**
  Extrahiert Bounding-Boxes aller Tokens.
[x] **getLayout(buffer, options?)**
  Erkennt Blöcke, Zeilen, Wörter inkl. BBox + Struktur.
[x] **getWords(pathOrBuffer, options?)**
  Gibt nur erkannte Wörter inkl. Box + Confidence zurück.
[x] **getLines(pathOrBuffer, options?)**
  Gibt erkannte Textzeilen inkl. Box + Confidence zurück.
[x] **getBlocks(pathOrBuffer, options?)**
  Gibt größere Textblöcke (Abschnitte) zurück.
[x] **getConfidences(pathOrBuffer, options?)**
  Liefert Array aller Wort-Confidence-Werte.
[x] **processToPDF(pages, options?)**
  Rendert Bilder/Seiten zu einem OCR-PDF.
[x] **processToHOCR(pages, options?)**
  Erzeugt hOCR-Datei aus mehreren Seiten.
[x] **processToALTO(pages, options?)**
  Erzeugt ALTO-XML für Dokumente.
[x] **setVariable(name, value)**
  Setzt interne Tesseract-Variablen (Whitelist, PSM-Overrides etc.).
[x] **clearCache()**
  Leert Tesseract-Caches (z.B. LSTM-Cache) für definierte Re-Runs.
