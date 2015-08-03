project(hawk-test)
cmake_minimum_required(VERSION 2.8)
aux_source_directory(. SRC_LIST)

set(CMAKE_CXX_FLAGS "-std=c++1y -Wall -pedantic -g -O0")

include_directories("include" "/usr/local/include/hawk")

add_executable(${PROJECT_NAME} ${SRC_LIST})
target_link_libraries(${PROJECT_NAME} hawk magic pthread)
