set(TARGET_BENCHMARK benchmark)

set(CMAKE_CXX_STANDARD 17)

include(../cmakecommon.txt)

file(GLOB_RECURSE SRC_BENCHMARK "./*.cpp" "./*.h")

add_executable(
	${TARGET_BENCHMARK}
	${SRC_BENCHMARK}
	${ALL_FILES}
)

if(CMAKE_COMPILER_IS_GNUCXX)
	set(CMAKE_CXX_FLAGS "-O3")
endif()

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(${TARGET_BENCHMARK} Threads::Threads ${COMMON_LIBS})

