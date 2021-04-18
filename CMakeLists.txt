cmake_minimum_required(VERSION 3.10)
project(optic)

find_package( OpenCV REQUIRED )

add_executable(optic main.cpp)
target_include_directories(optic PUBLIC
                          "${PROJECT_BINARY_DIR}"
                          )
target_link_libraries(optic ${OpenCV_LIBS})