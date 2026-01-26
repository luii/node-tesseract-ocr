cmake_minimum_required(VERSION 3.10...4.2.1)
project(node-tesseract-ocr)
add_compile_definitions(NAPI_VERSION=10)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB SOURCE_FILES "src/*.cpp" "src/*.hpp")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(APPLE)
  set(CMAKE_OSX_DEPLOYMENT_TARGET "12.0" CACHE STRING "" FORCE)
  add_compile_options(-stdlib=libc++)
  add_link_options(-stdlib=libc++)
endif()

add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES} ${CMAKE_JS_SRC})
set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "" SUFFIX ".node")

find_package(PkgConfig REQUIRED)
pkg_check_modules(TESS REQUIRED IMPORTED_TARGET "tesseract>=5")
pkg_check_modules(LEPT REQUIRED IMPORTED_TARGET "lept>=1.74")

target_include_directories(${PROJECT_NAME} PRIVATE 
  ${CMAKE_JS_INC}
  ${TESS_INCLUDE_DIRS}
  ${LEPT_INCLUDE_DIRS}
)

if(APPLE)
  foreach(dir IN LISTS LEPT_INCLUDE_DIRS)
    target_include_directories(${PROJECT_NAME} PRIVATE "${dir}/..")
  endforeach()
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE 
  ${CMAKE_JS_LIB}
  PkgConfig::TESS
  PkgConfig::LEPT
)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)

if(MSVC AND CMAKE_JS_NODELIB_DEF AND CMAKE_JS_NODELIB_TARGET)
  # Generate node.lib
  execute_process(COMMAND ${CMAKE_AR} /def:${CMAKE_JS_NODELIB_DEF} /out:${CMAKE_JS_NODELIB_TARGET} ${CMAKE_STATIC_LINKER_FLAGS})
endif()
