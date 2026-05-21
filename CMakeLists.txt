cmake_minimum_required(VERSION 3.23)
project(expresion_parser VERSION 0.0.1)

add_library(ExpresionParser OBJECT
  src/NanBoxing.cpp
  src/ExpresionParser.cpp
)
target_include_directories(ExpresionParser PUBLIC src)

if (CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
  add_executable(main src/main.cpp)
  target_include_directories(main PRIVATE src)
  target_link_libraries(main)
endif()
