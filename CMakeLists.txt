cmake_minimum_required(VERSION 3.4)
project(multiformats)

find_package(OpenSSL REQUIRED)

add_library(${PROJECT_NAME} SHARED
    src/cid.cpp
    src/multibase.cpp
    src/multihash.cpp
    src/multiaddr.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE OpenSSL::OpenSSL)
target_include_directories(${PROJECT_NAME} PUBLIC include)
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)
