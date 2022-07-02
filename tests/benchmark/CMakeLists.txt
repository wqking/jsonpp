set(TARGET_BENCHMARK benchmark)

set(CMAKE_CXX_STANDARD 17)

file(GLOB_RECURSE SRC_BENCHMARK "./*.cpp" "./*.h")
file(GLOB_RECURSE HEADERS "../../include/jsonpp/*.h")
file(GLOB_RECURSE SRC_LIB "../../src/*.cpp")
file(GLOB_RECURSE SRC_INC_LIB "../../src/*.h")
file(GLOB_RECURSE SRC_THIRDPARTY "../../thirdparty/*.c*")
file(GLOB_RECURSE METAPP_HEADERS "/projects/metapp/include/*.h")
file(GLOB_RECURSE METAPP_SRC "/projects/metapp/src/*.cpp")

set(SRC_BENCHMARK
	${SRC_LIB}
	${SRC_INC_LIB}
	${SRC_BENCHMARK}
	${HEADERS}
	${METAPP_HEADERS}
	${METAPP_SRC}
	${SRC_THIRDPARTY}
)

add_executable(
	${TARGET_BENCHMARK}
	${SRC_BENCHMARK}
)

if(CMAKE_COMPILER_IS_GNUCXX)
	set(CMAKE_CXX_FLAGS "-O3")
endif()

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(${TARGET_BENCHMARK} Threads::Threads)

