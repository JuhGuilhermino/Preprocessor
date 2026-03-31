cmake_minimum_required(VERSION 3.10)
project(Preprocessor)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)

add_executable(preprocessor
    src/main.cpp
    src/Preprocessor.cpp
    src/CommentRemover.cpp
    src/WhitespaceMinifier.cpp
    src/FileHandler.cpp
)