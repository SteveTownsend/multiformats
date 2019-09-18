cmake_minimum_required(VERSION 3.10)
project(multiformats)

include(GenerateExportHeader)

add_library(${PROJECT_NAME} SHARED src/multibase.cpp)

generate_export_header(${PROJECT_NAME}
    EXPORT_FILE_NAME include/multiformats/export.hpp)

target_include_directories(${PROJECT_NAME} PUBLIC include)
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)
