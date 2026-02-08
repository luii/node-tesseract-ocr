import { stat } from "node:fs/promises";

export const isValidTraineddata = async (filePath: string) => {
  try {
    const info = await stat(filePath);
    return info.isFile() && info.size > 0;
  } catch {
    return false;
  }
};
