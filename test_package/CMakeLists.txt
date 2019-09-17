cmake_minimum_required(VERSION 2.8.12)
project(MultiformatsTests CXX)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

add_executable(${PROJECT_NAME}
    src/multibase-test.cpp)

target_link_libraries(${PROJECT_NAME} ${CONAN_LIBS})
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)
