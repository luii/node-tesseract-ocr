{
  "targets": [
    {
      "target_name": "node-tesseract-ocr",
      "sources": [
        "src/addon.cpp",
        "src/ocr_result.cpp",
        "src/ocr_worker.cpp",
        "src/handle.cpp"
      ],
      "include_dirs": [
        "<!(node -p \"require('node-addon-api').include\")",
        "/usr/include"
      ],
      "dependencies": [
        "<!(node -p \"require('node-addon-api').targets\"):node_addon_api",
      ],
      "cflags_cc": [
        "-std=c++23",
        "-Wall",
        "-Wextra",
        "-Wpedantic",
        "-Wconversion",
        "-Werror",
        "-fexceptions"
      ],
      "libraries": [
        "-ltesseract",
        "-llept"
      ],
      "defines": [
        "NODE_ADDON_API_DISABLE_DEPRECATED",
      ]
    }
  ]
}
