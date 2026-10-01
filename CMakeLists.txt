cmake_minimum_required(VERSION 3.10...4.11)
project(src)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_COMPILER g++)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/build/bin")

add_executable(opg3 src/opg3.cpp)

add_executable(step1_single_car src/step1_single_car.cpp)

add_executable(step2_multiple_cars src/step2_multiple_cars.cpp)