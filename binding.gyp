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
        "<!(node -p \"require('node-addon-api').include\")"
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
      "defines": [
        "NODE_ADDON_API_DISABLE_DEPRECATED",
      ],
      "conditions": [
        [
          "OS=='linux'",
          {
            "include_dirs": [
              "<!(node -p \"require('node-addon-api').include\")",
              "<!@(pkg-config --cflags-only-I tesseract lept | sed -e 's/-I//g')"
            ],
            "libraries": [
              "<!@(pkg-config --libs tesseract lept)"
            ]
          }
        ],
        [
          "OS=='mac'",
          {
            "include_dirs": [
              "<!(node -p \"require('node-addon-api').include\")",
              "/opt/homebrew/include",
              "/usr/local/include"
            ],
            "libraries": [
              "-L/opt/homebrew/lib",
              "-L/usr/local/lib",
              "-ltesseract",
              "-llept"
            ]
          }
        ],
      ]
    }
  ]
}
