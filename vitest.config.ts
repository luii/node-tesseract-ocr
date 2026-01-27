import { defineConfig } from "vitest/config";

export default defineConfig({
  test: {
    environment: "node",
    include: ["tests/js/**/*.{test,spec}.{js,ts}"],
    setupFiles: ["tests/js/setup.ts"],
  },
});
