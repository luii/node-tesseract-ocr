
declare module 'Tesseract' {
  export interface Tesseract {
    recognize: (imageBuffer: Buffer<any[]>) => Promise<any>;
  }
}
