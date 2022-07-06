set(TARGET_DOCSRC docsrc)

include(../cmakecommon.txt)

file(GLOB_RECURSE SRC_DOCSRC "./*.cpp")

add_executable(
	${TARGET_DOCSRC}
	${SRC_DOCSRC}
	${ALL_FILES}
)

target_include_directories(
	${TARGET_DOCSRC}
	PUBLIC
	./
)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(${TARGET_DOCSRC} Threads::Threads ${COMMON_LIBS})

if(MSVC)
	target_link_options(${TARGET_DOCSRC} PRIVATE "/INCREMENTAL")
endif()

add_test(NAME ${TARGET_DOCSRC} COMMAND ${TARGET_DOCSRC})
