cmake_minimum_required(VERSION 3.23)
project(IndexationImage)

set(CMAKE_CXX_STANDARD 14)

add_executable(IndexationImage main.cpp imageProcessingFunction.cpp header/imageProcessingFunction.h header/mask.h nrc/def.h nrc/nralloc.cpp nrc/nralloc.h nrc/nrarith.cpp nrc/nrio.cpp)
