[//]: # (Auto generated file, don't modify this file.)

# jsonpp -- easy to use C++ JSON parser and stringify library
<!--begintoc-->
- [Features](#mdtoc_bfc0dc13)
- [Basic information](#mdtoc_abc52c05)
  - [License](#mdtoc_5768f419)
  - [Version 0.1.0](#mdtoc_c3ac4dae)
  - [Source code](#mdtoc_6b8a2c23)
  - [Supported compilers](#mdtoc_215a5bea)
- [Quick start](#mdtoc_ea7b0a9)
  - [Namespace](#mdtoc_33e16b56)
  - [Build and install, use jsonpp in your project](#mdtoc_3155c9b5)
- [Example code](#mdtoc_3bb166c4)
  - [Parse JSON text](#mdtoc_f277719e)
  - [Dump JSON object to text (stringify)](#mdtoc_a4e21f76)
- [Documentations](#mdtoc_aa76f386)
- [Build the test code](#mdtoc_460948ff)
- [Known compiler related quirks in MSVC](#mdtoc_2f938cf5)
- [Motivations](#mdtoc_c55531bd)
<!--endtoc-->

jsonpp is a cross platform C++ json library, focusing on easy to use.  

![C++](https://img.shields.io/badge/C%2B%2B-11-blue)
![Compilers](https://img.shields.io/badge/Compilers-GCC%2FMSVC%2FClang-blue)
![License](https://img.shields.io/badge/License-Apache--2.0-blue)
![CI](https://github.com/wqking/jsonpp/workflows/CI/badge.svg)

<a id="mdtoc_bfc0dc13"></a>
## Features

- **Easy to use, very easy to use, very very ... easy to use**. That means,
  - You can parse and stringify any C++ data structures, include STL containers, classes, etc.
  - For C++ native data types and STL containers, you don't need to provide any meta data to use them, because metapp already provides them.
  - Reflecting meta data for class is very easy. 

- **Reusable meta data**. jsonpp uses meta data from [C++ reflection library metapp](https://github.com/wqking/metapp) that's developed
by the same developer of jsonpp, which is not only not special to jsonpp, but also general enough to use for other purpose
such as serialization, script binding, etc.

- **Multiple parser backends**. jsonpp uses existing matured and well tested JSON parser libraries as the parser backend, such as
[json-parser](https://github.com/json-parser/json-parser) and [simdjson 2.2.0](https://github.com/simdjson/simdjson).
That means you can choose the best backend to achieve your goals.  

- **Decent performance**. Performance is not jsonpp strength and it's not close to high performance JSON libraries such as
simdjson. However, the performance is better than some existing popular JSON libraries which also focus on usability, thanks to
the high performance simdjson and well optimized metapp.

- Cross platforms, cross compilers.

- Written in standard and portable C++, only require C++11, and support later C++ standard

<a id="mdtoc_abc52c05"></a>
## Basic information

<a id="mdtoc_5768f419"></a>
### License

Apache License, Version 2.0  

<a id="mdtoc_c3ac4dae"></a>
### Version 0.1.0

The project is under working in progress.  
The first stable release will be v1.0.0. 

To put the library to first release, we need to,   
1. Add more tests.
2. Complete the documentations.

You are welcome to try the project and give feedback. Your participation will help to make the development faster and better.

<a id="mdtoc_6b8a2c23"></a>
### Source code

[https://github.com/wqking/jsonpp](https://github.com/wqking/jsonpp)

<a id="mdtoc_215a5bea"></a>
### Supported compilers

jsonpp requires C++ compiler that supports C++11 standard.  
The library is tested with MSVC 2022, 2019, MinGW (Msys) GCC 8.3 and 11.3.0, Clang (carried by MSVC).  
In brief, MSVC, GCC, Clang that has well support for C++11, or released after 2019, should be able to compile the library.

<a id="mdtoc_ea7b0a9"></a>
## Quick start

<a id="mdtoc_33e16b56"></a>
### Namespace

`jsonpp`

<a id="mdtoc_3155c9b5"></a>
### Build and install, use jsonpp in your project

There are various methods to use jsonpp

1, Install using CMake and use it in CMake

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

<a id="mdtoc_3bb166c4"></a>
## Example code


<a id="mdtoc_f277719e"></a>
### Parse JSON text
Header for JsonParser

```c++
#include "jsonpp/jsonparser.h"
```

Create a parser with default configuration and default backend which is simdjson.

```c++
jsonpp::JsonParser parser;
```

This is the JSON we are going to parse.

```c++
const std::string jsonText = R"(
  [ 5, "abc", true, null, 3.14, [ 1, 2, 3 ], { "one": 1, "two": 2 } ]
)";
```

Parse the JSON, the result is a `metapp::Variant`.

```c++
metapp::Variant var = parser.parse(jsonText);
```

The result is an array.

```c++
ASSERT(jsonpp::getJsonType(var) == jsonpp::JsonType::jtArray);
```

Get the underlying array. jsonpp::JsonArray is alias of `std::vector<metapp::Variant>`

```c++
const jsonpp::JsonArray & array = var.get<const jsonpp::JsonArray &>();
```

Now verify the elements.

```c++
ASSERT(array[0].get<jsonpp::JsonInt>() == 5);
ASSERT(array[1].get<const jsonpp::JsonString &>() == "abc");
ASSERT(array[2].get<jsonpp::JsonBool>());
ASSERT(array[3].get<jsonpp::JsonNull>() == nullptr);
ASSERT(array[4].get<jsonpp::JsonReal>() == 3.14);
const jsonpp::JsonArray & nestedArray = array[5].get<const jsonpp::JsonArray &>();
ASSERT(nestedArray[0].get<jsonpp::JsonInt>() == 1);
ASSERT(nestedArray[1].get<jsonpp::JsonInt>() == 2);
ASSERT(nestedArray[2].get<jsonpp::JsonInt>() == 3);
jsonpp::JsonObject & nestedObject = array[6].get<jsonpp::JsonObject &>();
ASSERT(nestedObject["one"].get<jsonpp::JsonInt>() == 1);
ASSERT(nestedObject["two"].get<jsonpp::JsonInt>() == 2);
```

<a id="mdtoc_a4e21f76"></a>
### Dump JSON object to text (stringify)
Header for JsonDumper

```c++
#include "jsonpp/jsondumper.h"
```

```c++
std::string text;
```

Create a dumper with default configuration.

```c++
jsonpp::JsonDumper dumper;
```

Dump a simple integer.

```c++
text = dumper.dump(5);
ASSERT(text == "5");
```

Dump a boolean.

```c++
text = dumper.dump(true);
ASSERT(text == "true");
```

Dump complicated data struct.

```c++
text = dumper.dump(jsonpp::JsonObject {
  { "first", "hello" },
  { "second", nullptr },
  { "third", std::vector<int> { 5, 6, 7 } },
  { "fourth", jsonpp::JsonArray { "abc", 9.1 } },
});
// jsonpp::JsonObject is alias of std::map<std::string, metapp::Variant>, so the keys are sorted alphabetically.
ASSERT(text == R"({"first":"hello","fourth":["abc",9.1],"second":null,"third":[5,6,7]})");
```

<a id="mdtoc_aa76f386"></a>
## Documentations
Below are tutorials and documents. Don't feel upset if you find issues or missing stuff in the documents, I'm not
native English speaker and it's not that exciting to write document. Any way, the code quality is always much better
than the document, for ever.  
If you want to contribute to the documents, be sure to read [How to generate documentations](doc/about_document.md).  

- Tutorials
  - [Use Variant](doc/tutorial/tutorial_variant.md)
  - [Use MetaType](doc/tutorial/tutorial_metatype.md)
  - [Use MetaClass and meta data for class members](doc/tutorial/tutorial_metaclass.md)
  - [Use MetaCallable, function, member function, etc](doc/tutorial/tutorial_callable.md)
  - [Use MetaRepo to retrieve meta data at running time](doc/tutorial/tutorial_metarepo.md)
  - [Use classes inheritance](doc/tutorial/tutorial_metaclass_inheritance.md)

- Core concepts and classes
  - [Core concepts - type kind, meta type, up type, meta interface](doc/core_concepts.md)
  - [Class Variant reference](doc/variant.md)
  - [Class MetaType reference](doc/metatype.md)

- Build and use meta data
  - [Reflect meta type at compile time using DeclareMetaType](doc/declaremetatype.md)
  - [Register and use meta type at running time using MetaRepo](doc/metarepo.md)
  - [MetaItem](doc/metaitem.md)
  - [MetaItemView and BaseView](doc/views.md)

- Meta interfaces
  - [Overview and implement meta interface](doc/meta_interface_overview.md)
  - [MetaClass](doc/interfaces/metaclass.md)
  - [MetaCallable](doc/interfaces/metacallable.md)
  - [MetaAccessible](doc/interfaces/metaaccessible.md)
  - [MetaEnum](doc/interfaces/metaenum.md)
  - [MetaIndexable](doc/interfaces/metaindexable.md)
  - [MetaIterable](doc/interfaces/metaiterable.md)
  - [MetaStreamable](doc/interfaces/metastreamable.md)
  - [MetaMappable](doc/interfaces/metamappable.md)
  - [MetaPointerWrapper](doc/interfaces/metapointerwrapper.md)
  - [User defined meta interface](doc/interfaces/metauser.md)

- Built-in meta types
  - [Overview](doc/metatypes/overview_metatypes.md)
  - [List of all built-in meta types](doc/metatypes/list_all.md)
  - [Constructor](doc/metatypes/constructor.md)
  - [Overloaded function](doc/metatypes/overloaded_function.md)
  - [Default arguments function](doc/metatypes/default_args_function.md)
  - [Variadic function](doc/metatypes/variadic_function.md)
  - [Accessor](doc/metatypes/accessor.md)

- Utilities
  - [utility.h](doc/utilities/utility.md)
  - [TypeList reference](doc/utilities/typelist.md)

- Miscellaneous
  - [Use metapp in dynamic library](doc/dynamic_library.md)
  - [Performance and benchmark](doc/benchmark.md)
  - [Exception and thread safety](doc/exception_thread_safety.md)
  - [Infrequently Asked Questions](doc/faq.md)
  - [About documentations](doc/about_document.md)


<a id="mdtoc_460948ff"></a>
## Build the test code

There are several parts of code to test the library,

- unittests: tests the library.
- docsrc: documentation source code, and sample code to demonstrate how to use the library. 
- benchmark: measure the performance.

All parts are in the `tests` folder.

All parts require CMake to build, and there is a makefile to ease the building.  
Go to folder `tests/build`, then run `make` with different target.
- `make vc19` #generate solution files for Microsoft Visual Studio 2019, then open metapptest.sln in folder project_vc19
- `make vc17` #generate solution files for Microsoft Visual Studio 2017, then open metapptest.sln in folder project_vc17
- `make vc15` #generate solution files for Microsoft Visual Studio 2015, then open metapptest.sln in folder project_vc15
- `make mingw` #build using MinGW
- `make linux` #build on Linux
- `make mingw_coverage` #build using MinGW and generate code coverage report

<a id="mdtoc_2f938cf5"></a>
## Known compiler related quirks in MSVC

MSVC 2022 and 2019, can build the CMake generated test projects and the tests run correctly in Debug and RelWithDebugInfo
configurations. But some tests fail in Release mode when incremental linking is disabled.  
Those failed tests should not fail, because they work correct in MSVC debug mode and in GCC/Clang.
Adding /Debug option to linking which generates debug information makes the tests success.  
Without /Debug option, but enabling incremental linking, will cause the tests success too.  
So if metapp shows weird behavior in MSVC, try to enable incremental linking.

<a id="mdtoc_c55531bd"></a>
## Motivations

I (wqking) developed `cpgf` library since more than 12 years ago. `cpgf` works, but it has several serious problems.
The first problem is it was written in C++03, the code is verbose and difficult to write. The second problem is that
it includes too many features in a single library -- reflection, serialization, script binding (Lua, Python, JavaScript).
`cpgf` became unmanageable and can't be developed in an elegant way.  
`metapp` is a fresh library that only focuses on reflection. Other features, such as serialization,
script binding, will be in separated projects, if they are developed.

