cmake_minimum_required(VERSION 3.23)
project(expresion_parser VERSION 0.0.1)

if (CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
  add_executable(main src/main.cpp)
  target_include_directories(main PRIVATE src)
endif()
