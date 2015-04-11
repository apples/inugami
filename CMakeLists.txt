cmake_minimum_required (VERSION 2.6)
project(Inugami)

find_package(PNG REQUIRED)
find_package(GLEW REQUIRED)
find_package(OpenGL REQUIRED)

#find_package(glfw3 REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_search_module(GLFW REQUIRED glfw3)

file(GLOB_RECURSE inugami_SOURCES src/inugami/*.cpp src/inugami/*.hpp)
add_library(inugami ${inugami_SOURCES})
set_property(TARGET inugami PROPERTY CXX_STANDARD 14)
target_include_directories(inugami INTERFACE src)
target_link_libraries(inugami ${PNG_LIBRARIES})
target_link_libraries(inugami ${GLEW_LIBRARIES})
target_link_libraries(inugami ${GLFW_LIBRARIES})
target_link_libraries(inugami ${OPENGL_LIBRARIES})

add_executable(demo src/main.cpp)
set_property(TARGET demo PROPERTY CXX_STANDARD 14)
target_link_libraries(demo inugami)
