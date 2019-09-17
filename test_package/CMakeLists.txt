cmake_minimum_required(VERSION 2.8.12)
project(MultiformatsTests CXX)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

find_package(multiformats REQUIRED)

add_executable(${PROJECT_NAME}
    src/multibase.cpp)

message(STATUS ${CONAN_LIBS_GTEST})
target_link_libraries(${PROJECT_NAME} ${CONAN_LIBS_GTEST} multiformats::multiformats)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)
