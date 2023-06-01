cmake_minimum_required(VERSION 3.10.0)

project (AudioSampleLibrary)

add_library(asl INTERFACE)

target_include_directories(asl
    INTERFACE include
)

add_executable(asl_test
    test/asl_test.cpp
)

target_link_libraries(asl_test
    PRIVATE
        asl
)