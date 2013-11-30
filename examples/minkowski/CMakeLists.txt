cmake_minimum_required(VERSION 2.6 FATAL_ERROR)

project(minkowski-example)

add_executable(minkowski minkowski.cpp)
target_link_libraries(minkowski visualization ${GMP_LIBRARIES})
