project(jsonpptest)

cmake_minimum_required(VERSION 3.2)

set(CMAKE_CXX_STANDARD 11)

include_directories(../include)
include_directories(../thirdparty)
include_directories(/projects/metapp/include)

if(MSVC)
	add_definitions(/W4)
else()
	add_definitions(-Wall -Wextra -Wpedantic)
endif()

enable_testing()
add_subdirectory(unittest)
add_subdirectory(docsrc)
add_subdirectory(benchmark)
