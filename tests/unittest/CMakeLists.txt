set(TARGET_TEST unittest)

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

add_test(NAME ${TARGET_TEST} COMMAND ${TARGET_TEST})
