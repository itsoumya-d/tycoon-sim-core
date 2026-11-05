cmake_minimum_required(VERSION 3.16)
project(tycoon_sim_core LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(tycoon_sim src/sim.cpp)
target_include_directories(tycoon_sim PUBLIC include)

add_executable(tycoon_demo src/main.cpp)
target_link_libraries(tycoon_demo PRIVATE tycoon_sim)

enable_testing()
add_executable(tycoon_tests tests/test_sim.cpp)
target_link_libraries(tycoon_tests PRIVATE tycoon_sim)
add_test(NAME tycoon_tests COMMAND tycoon_tests)
