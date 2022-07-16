[//]: # (Auto generated file, don't modify this file.)

# Build, install, integrate

## Build and install

If you are going to use jsonpp in CMake managed project, you can install jsonpp then use it in CMake.  
In jsonpp root folder, run the commands,  
```
mkdir build
cd build
cmake ..
sudo make install
```

Then in the project CMakeLists.txt,   
```
# the project target is mytest, just for example
add_executable(mytest test.cpp)

find_package(jsonpp)
if(jsonpp)
target_link_libraries(mytest jsonpp::jsonpp)
else(jsonpp)
message(FATAL_ERROR "jsonpp library is not found")
endif()
```

Note: when using this method with MinGW on Windows, by default CMake will install jsonpp in system folder which is not writable.
You should specify another folder to install.
To do so, replace `cmake ..` with `cmake .. -DCMAKE_INSTALL_PREFIX="YOUR_NEW_LIB_FOLDER"`.

## Configuration

jsonpp supports one CMake configuration, `DEFAULT_PARSER_BACKEND`, the available values are the enum name of `ParserBackendType`,
which is `simdjson` and `cparser`, the default value is `simdjson`.  
`DEFAULT_PARSER_BACKEND` defines the default parser backend. The code of the default backend is always linked to the executable,
no matter it's used or not. For example, if the default is `simdjson`, and your project only uses `simdjson`, then only code
of `simdjson` is linked, and `cparser` is eliminated from the executable. But if you project only uses `cparser`, then both
`simdjson` and `cparser` are linked to the executable, and `simdjson` wastes binary size, in such case, you should set
`DEFAULT_PARSER_BACKEND` to `cparser` to reduce binary size.

## Example

Assume the current directory is `jsonpp/build`.

Build with default tool chain and default configuration.
```
cmake ..
sudo make install
```

Build with default tool chain, set default parser backend as `cparser`.
```
cmake .. -DDEFAULT_PARSER_BACKEND=cparser
sudo make install
```

This is my test command on Windows using MinGW.
```
cmake .. -DCMAKE_INSTALL_PREFIX="/temp/lib" -G"MinGW Makefiles" -DDEFAULT_PARSER_BACKEND=cparser
mingw32-make.exe install
```

## Integrate to your project

Below is the content of CMakeLists.txt in `jsonpp/tests/cmake-pkg-demo`. It uses the installed jsonpp library.

```
project(jsonpppkgdemo)

cmake_minimum_required(VERSION 3.2)

set(CMAKE_CXX_STANDARD 11)

set(TARGET pkgdemo)

set(SRC
  main.cpp
)

add_executable(
  ${TARGET}
  ${SRC}
)

find_package(jsonpp CONFIG REQUIRED)
target_link_libraries(${TARGET} PRIVATE jsonpp::jsonpp)
```
