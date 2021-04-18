cmake_minimum_required(VERSION 3.10)
project(optic)
project(optic-viewer)

set(THREADS_PREFER_PTHREAD_FLAG ON)

SET(CMAKE_FIND_LIBRARY_PREFIXES lib)
SET(CMAKE_FIND_LIBRARY_SUFFIXES a)
set(CMAKE_CXX_FLAGS "-O3")

find_package(Threads REQUIRED)
find_package(OpenCV REQUIRED)

add_executable(optic server.cpp)
add_executable(optic-viewer client.cpp)

target_include_directories(optic PUBLIC
                          "${PROJECT_BINARY_DIR}"
                          )
target_include_directories(optic-viewer PUBLIC
                          "${PROJECT_BINARY_DIR}"
                          )

target_link_libraries(optic ${OpenCV_LIBS} libzmq.a libzmqpp.a libsodium.a Threads::Threads)
target_link_libraries(optic-viewer ${OpenCV_LIBS} libzmq.a libzmqpp.a libsodium.a Threads::Threads)
