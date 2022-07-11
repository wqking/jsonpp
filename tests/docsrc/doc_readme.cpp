// jsonpp library
// 
// Copyright (C) 2022 Wang Qi (wqking)
// 
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// 
//   http://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "testutil.h"

#include "jsonpp/macros.h"

#include "metapp/allmetatypes.h"
#include "metapp/interfaces/metaclass.h"
#include "metapp/interfaces/metaenum.h"

#include <cstring>
#include <sstream>

/*desc
# jsonpp -- easy to use C++ JSON parser and stringify library

jsonpp is a cross platform C++ json library, focusing on easy to use.  

![C++](https://img.shields.io/badge/C%2B%2B-11-blue)
![Compilers](https://img.shields.io/badge/Compilers-GCC%2FMSVC%2FClang-blue)
![License](https://img.shields.io/badge/License-Apache--2.0-blue)
![CI](https://github.com/wqking/jsonpp/workflows/CI/badge.svg)

## Features

- **Easy to use, very easy to use, very very ... easy to use**. That means,
  - You can parse and stringify any C++ data structures, include STL containers, classes, etc.
  - For C++ native data types and STL containers, you don't need to provide any meta data to use them, because metapp already provides them.
  - Reflecting meta data for class is very easy. 

- **Reusable meta data**. jsonpp uses meta data from [C++ reflection library metapp](https://github.com/wqking/metapp) that's developed
by the same developer (wqking) of jsonpp, which is not only not special to jsonpp, but also general enough to use for other purpose
such as serialization, script binding, etc. You don't build data or spend time for jsonpp, you invest for a potential ecosystem
with bright future.

- **Multiple parser backends**. jsonpp uses existing matured and well tested JSON parser libraries as the parser backend, such as
[json-parser](https://github.com/json-parser/json-parser) and [simdjson 2.2.0](https://github.com/simdjson/simdjson).
That means you can choose the best viable backend to achieve your goals and avoid the backends that make you trouble. Also adding
new backend is very easy.

- **Decent performance**. Performance is not jsonpp strength and it's not close to high performance JSON libraries such as
simdjson. However, the performance is better than some existing popular JSON libraries which also focus on usability, thanks to
the high performance simdjson and well optimized metapp.

- Cross platforms, cross compilers.

- Written in standard and portable C++, only require C++11, and support later C++ standard

## Basic information

### License

Apache License, Version 2.0  

### Version 0.1.0

The project is under working in progress.  
The first stable release will be v1.0.0. 

To put the library to first release, we need to,   
1. Add more tests.
2. Complete the documentations.

You are welcome to try the project and give feedback. Your participation will help to make the development faster and better.

### Source code

[https://github.com/wqking/jsonpp](https://github.com/wqking/jsonpp)

### Supported compilers

jsonpp requires C++ compiler that supports C++11 standard.  
The library is tested with MSVC 2022, 2019, MinGW (Msys) GCC 8.3 and 11.3.0, Clang (carried by MSVC).  
In brief, MSVC, GCC, Clang that has well support for C++11, or released after 2019, should be able to compile the library.

## Quick start

### Namespace

`jsonpp`

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

## Example code

desc*/
//desc ### Parse JSON text

//code
//desc Header for JsonParser
#include "jsonpp/jsonparser.h"
//code

ExampleFunc
{
	//code
	//desc Create a parser with default configuration and default backend which is simdjson.
	jsonpp::JsonParser parser;
	//desc This is the JSON we are going to parse.
	const std::string jsonText = R"(
		[ 5, "abc", true, null, 3.14, [ 1, 2, 3 ], { "one": 1, "two": 2 } ]
	)";
	//desc Parse the JSON, the result is a `metapp::Variant`.
	metapp::Variant var = parser.parse(jsonText);
	//desc The result is an array.
	ASSERT(jsonpp::getJsonType(var) == jsonpp::JsonType::jtArray);
	//desc Get the underlying array. jsonpp::JsonArray is alias of `std::vector<metapp::Variant>`
	const jsonpp::JsonArray & array = var.get<const jsonpp::JsonArray &>();
	//desc Now verify the elements.
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
	//code
}

//desc ### Dump JSON object to text (stringify)

//code
//desc Header for JsonDumper
#include "jsonpp/jsondumper.h"
//code

ExampleFunc
{
	//code
	std::string text;
	//desc Create a dumper with default configuration.
	jsonpp::JsonDumper dumper;
	//desc Dump a simple integer.
	text = dumper.dump(5);
	ASSERT(text == "5");
	//desc Dump a boolean.
	text = dumper.dump(true);
	ASSERT(text == "true");
	//desc Dump complicated data struct.
	text = dumper.dump(jsonpp::JsonObject {
		{ "first", "hello" },
		{ "second", nullptr },
		{ "third", std::vector<int> { 5, 6, 7 } },
		{ "fourth", jsonpp::JsonArray { "abc", 9.1 } },
	});
	// jsonpp::JsonObject is alias of std::map<std::string, metapp::Variant>, so the keys are sorted alphabetically.
	ASSERT(text == R"({"first":"hello","fourth":["abc",9.1],"second":null,"third":[5,6,7]})");
	//code
}

//desc ### Dump/parse class object
//desc Now let's dump and parse customized class objects. First let's define the enum and classes we will use later.

//code
enum class Gender
{
	female,
	male
};

struct Skill
{
	std::string name;
	int level;
};

struct Person
{
	std::string name;
	Gender gender;
	int age;
	std::vector<Skill> skills;
};

//desc Now make the enum and class information availabe to metapp. jsonpp uses the reflection information from metapp.
//desc The information is not special to jsonpp, it's general purpose reflection and can be used for other purposes such
//desc as serialization, script binding, etc.
template <>
struct metapp::DeclareMetaType <Gender> : metapp::DeclareMetaTypeBase <Gender>
{
	static const metapp::MetaEnum * getMetaEnum() {
		static const metapp::MetaEnum metaEnum([](metapp::MetaEnum & me) {
				me.registerValue("female", Gender::female);
				me.registerValue("male", Gender::male);
			}
		);
		return &metaEnum;
	}
};

template <>
struct metapp::DeclareMetaType <Skill> : metapp::DeclareMetaTypeBase <Skill>
{
	static const metapp::MetaClass * getMetaClass() {
		static const metapp::MetaClass metaClass(
			metapp::getMetaType<Skill>(),
			[](metapp::MetaClass & mc) {
				mc.registerAccessible("name", &Skill::name);
				mc.registerAccessible("level", &Skill::level);
			}
		);
		return &metaClass;
	}
};

// I don't encourage to use macros and I don't provide macros in metapp library.
// But for jsonpp users that don't want to dig into metapp and only want to the jsonpp features,
// jsonpp provides macros to ease the meta type declaration.
// Note: the macros are not required by jsonpp. The code can be rewritten without macros, as how Skill is declared above.
JSONPP_BEGIN_DECLARE_CLASS(Person)
	JSONPP_REGISTER_CLASS_FIELD(name)
	JSONPP_REGISTER_CLASS_FIELD(gender)
	JSONPP_REGISTER_CLASS_FIELD(age)
	JSONPP_REGISTER_CLASS_FIELD(skills)
JSONPP_END_DECLARE_CLASS()

//code

bool operator == (const Skill & a, const Skill & b)
{
	return a.name == b.name && a.level == b.level;
}

bool operator == (const Person & a, const Person & b)
{
	return a.name == b.name
		&& a.gender == b.gender
		&& a.age == b.age
		&& a.skills == b.skills
	;
}

ExampleFunc
{
	//code
	//desc Now let's dump `person` to text, then parse the text back to `Person` object.
	//desc `enableNamedEnum(true)` will use the name such as "female" for the Gender enum, instead of numbers such as 0.
	//desc This allows the enum value change without breaking the dumped object.
	Person person { "Mary", Gender::female, 26, { { "Writing", 8 }, { "Cooking", 6 } } };
	jsonpp::JsonDumper dumper(jsonpp::DumperConfig().enableBeautify(true).enableNamedEnum(true));
	// We don't user `person` any more, so we can move it to `dump` to avoid copying.
	const std::string jsonText = dumper.dump(std::move(person));
	/*desc
	The jsonText looks like,
```
{
    "name": "Mary",
    "gender": "female",
    "age": 26,
    "skills": [
        {
            "name": "Writing",
            "level": 8
        },
        {
            "name": "Cooking",
            "level": 6
        }
    ]
}
```
	desc*/
	//desc Now let's parse the JSON text back to Person object, and verify the values.
	jsonpp::JsonParser parser;
	const Person parsedPerson = parser.parse<Person>(jsonText);
	ASSERT(parsedPerson.name == "Mary");
	ASSERT(parsedPerson.gender == Gender::female);
	ASSERT(parsedPerson.age == 26);
	ASSERT(parsedPerson.skills[0].name == "Writing");
	ASSERT(parsedPerson.skills[0].level == 8);
	ASSERT(parsedPerson.skills[1].name == "Cooking");
	ASSERT(parsedPerson.skills[1].level == 6);
	//code
}

ExampleFunc
{
	//code
	//desc We can not only dump/parse a single object, but also any STL containers with the objects.
	Person personAlice { "Alice", Gender::female, 28, { { "Excel", 7 }, { "Word", 8 } } };
	Person personTom { "Tom", Gender::male, 29, { { "C++", 9 }, { "Python", 10 }, { "PHP", 7 } } };
	jsonpp::JsonDumper dumper(jsonpp::DumperConfig().enableBeautify(true).enableNamedEnum(true));
	const std::string jsonText = dumper.dump(std::vector<Person>{ personAlice, personTom });
	jsonpp::JsonParser parser;
	const std::vector<Person> parsedPersons = parser.parse<std::vector<Person> >(jsonText);
	ASSERT(parsedPersons[0] == personAlice);
	ASSERT(parsedPersons[1] == personTom);
	//code
}

/*desc
## Documentations
Below are tutorials and documents. Don't feel upset if you find issues or missing stuff in the documents, I'm not
native English speaker and it's not that exciting to write document. Any way, the code quality is always much better
than the document, for ever.  
If you want to contribute to the documents, be sure to read [How to generate documentations](doc/about_document.md).  

//@include inc/inc_doc_index.md

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

desc*/
