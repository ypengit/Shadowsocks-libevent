cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF) # -std=c++11 instead of -std=gnu++11
set(MY_CXX_COMPILE_FEATURES cxx_generic_lambdas cxx_range_for cxx_strong_enums)



set(SRCS
    other.cpp
    server.cpp
    ss-server.cpp
)


add_executable(ss-server ${SRCS})

target_link_libraries(ss-server pthread event)

