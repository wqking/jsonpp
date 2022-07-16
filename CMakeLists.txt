cmake_minimum_required(VERSION 3.11)
project(jsonpp VERSION 0.1.0)

add_library(${PROJECT_NAME} STATIC)
set(CMAKE_CXX_STANDARD 11)

set(DEFAULT_PARSER_BACKEND "simdjson" CACHE STRING "Specify default ParserBackendType, the default value is simdjson.")
set_property(CACHE DEFAULT_PARSER_BACKEND PROPERTY STRINGS simdjson cparser)

add_definitions(-DJSONPP_DEFAULT_PARSER_BACKEND=${DEFAULT_PARSER_BACKEND})

include(FetchContent)
FetchContent_Declare(
	metapp
	GIT_REPOSITORY https://github.com/wqking/metapp.git
	#GIT_TAG        703bd9caab50b139428cea1aaff9974ebee5742e # release-1.10.0
)
FetchContent_MakeAvailable(metapp)

target_link_libraries(${PROJECT_NAME} metapp::metapp)
target_include_directories(
	${PROJECT_NAME}
	PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

file(GLOB_RECURSE SRC_LIB "src/*.c*")
target_sources(${PROJECT_NAME}
    PRIVATE
        ${SRC_LIB}
)

add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

# Installation
# ------------
include(GNUInstallDirs)

# Install the library
install(
	TARGETS ${PROJECT_NAME}
	EXPORT ${PROJECT_NAME}Targets
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

# Install the headers
install(
	DIRECTORY include/
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# (Generate and) install the target import file
install(
	EXPORT ${PROJECT_NAME}Targets
	NAMESPACE jsonpp::
	DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/jsonpp
)

# Generate the package version file
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
	${CMAKE_CURRENT_BINARY_DIR}/jsonppConfigVersion.cmake
	VERSION ${PROJECT_VERSION}
	COMPATIBILITY AnyNewerVersion
)

# Generate the package configuration file, that allows other
# CMake projects to find the library with find_package()
configure_package_config_file(
	cmake/jsonppConfig.cmake.in
	${CMAKE_CURRENT_BINARY_DIR}/jsonppConfig.cmake
	INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/jsonpp
)

# Install the package version and configuration files
install(
	FILES
	${CMAKE_CURRENT_BINARY_DIR}/jsonppConfig.cmake
	${CMAKE_CURRENT_BINARY_DIR}/jsonppConfigVersion.cmake
	DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/jsonpp
)

# Install readme and license
install(
	FILES
	readme.md
	license
	DESTINATION ${CMAKE_INSTALL_DATADIR}/jsonpp
)
