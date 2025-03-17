cmake_minimum_required(VERSION 3.29)
project(nav)

set(CMAKE_CXX_STANDARD 20)

add_executable(nav main.cpp)

set(SFML_STATIC_LIBRARIES TRUE)
set(SFML_DIR "C:/Users/LENOVO/Desktop/programozas/SFML2/lib/cmake/SFML")
find_package(SFML 2.6 COMPONENTS graphics window system REQUIRED)

target_link_libraries(nav PRIVATE sfml-graphics sfml-window sfml-system)
include_directories(${SFML_INCLUDE_DIRS})
