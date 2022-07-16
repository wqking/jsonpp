set(TARGET_TEST unittest)

if(MSVC)
	# MSVC pre-C++20 can't compile some code due to bad conformance to C++ standard
	set(CMAKE_CXX_STANDARD 20)
endif()

include(../cmakecommon.txt)

file(GLOB_RECURSE SRC_TEST "./*.cpp")

add_executable(
	${TARGET_TEST}
	${SRC_TEST}
	${ALL_FILES}
)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(${TARGET_TEST} Threads::Threads ${COMMON_LIBS})

set_target_properties(${TARGET_TEST} PROPERTIES CXX_STANDARD 11)

if(CMAKE_COMPILER_IS_GNUCXX)
if(coverage)
	set(CMAKE_CXX_FLAGS "-g -O0 -Wall -fprofile-arcs -ftest-coverage")
endif()
endif()

if(MSVC)
	target_link_options(${TARGET_TEST} PRIVATE "/INCREMENTAL")
endif()

add_test(NAME ${TARGET_TEST} COMMAND ${TARGET_TEST})
