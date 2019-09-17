cmake_minimum_required(VERSION 3.10)
project(multiformats)

add_library(${PROJECT_NAME}
    src/multibase.cpp)

target_include_directories(${PROJECT_NAME} PUBLIC include)
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)
