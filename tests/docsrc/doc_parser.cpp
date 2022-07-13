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
#include "jsonpp/parser.h"
#include "jsonpp/dumper.h"

#include "metapp/allmetatypes.h"
#include "metapp/interfaces/metaclass.h"
#include "metapp/interfaces/metaenum.h"

#include <cstring>
#include <sstream>

/*desc
# Use jsonpp::Parser to parse JSON document

## Class Parser

#### Header

```c++
#include "jsonpp/parser.h"
```

#### Default constructor

```c++
Parser();
```

Construct Parser with default configurations.

#### Construct with config

```c++
explicit Parser(const ParserConfig & config);
```

Construct Parser using `config`.

#### parse a series of characters

```c++
metapp::Variant parse(const char * jsonText, const std::size_t length, const metapp::MetaType * prototype = nullptr);

template <typename T>
T parse(const char * jsonText, const std::size_t length);
```

Parse JSON document at `jsonText` of `length` characters.  
If `prototype` is nullptr, the document is parsed as default data types. Please check [this document](common_types.md) for the
default data types.  
If `prototype` is not nullptr, the document is parsed as `prototype`.  

If there is no any error, a `metapp::Variant` is returned that represents the document.  
If there is any error occurred, empty Variant is returned (Variant::isEmpty() is true).  

Some parser backends have special requirement on the data alignment and padding, `jsonpp` will handle such requirements
internally, so the caller of `parse` don't need to care about the requirements.  

The second template form is same as,  
```
json::Parser parser;

// template form
T result = parser.parse<T>(jsonText, length);

// equals to
metapp::Variant result = parser.parse(jsonText, length, metapp::getMetaType<T>());
return result.get<const T &>();
```

#### parse a std::string

```c++
metapp::Variant parse(const std::string & jsonText, const metapp::MetaType * prototype = nullptr);

template <typename T>
T parse(const std::string & jsonText);
```

Parse the document in `jsonText`.

#### parse ParserSource

```c++
metapp::Variant parse(const ParserSource & source, const metapp::MetaType * prototype = nullptr);

template <typename T>
T parse(const ParserSource & source);
```

Parse the document in `source`.  
If you needs to repeat parsing the same document, using `ParserSource` may increase performance slightly,
depending on the parse backend.

#### Error handling

```c++
bool hasError() const;
std::string getError() const;
```

`hasError()` returns true if there is any error occurred during previous `parse`, false if it succeeds.  
`getError()` returns the error message if there is any error.  
Both `metapp` and the parser backend may throw exceptions, all exceptions are captured and converted to error message.  
`metapp` works if exceptions are disabled in compiler, for the parser backend, please check their document to see
if exceptions can be disabled.

## ParserSource

#### Header

```c++
#include "jsonpp/dumper.h"
```

```c++
class ParserSource
{
public:
	ParserSource();
	ParserSource(const char * cstr, const std::size_t cstrLength);
	explicit ParserSource(const std::string & str);
	explicit ParserSource(std::string && str);
};
```

`ParserSource` has constructors that accept `std::string` or C string which is the JSON document.  
Note `ParserSource` refers to the string, so the content must be available untile the `ParserSource` is destroyed.

## ParserConfig

#### Header

```c++
#include "jsonpp/dumper.h"
```

#### Default constructor

```c++
ParserConfig();
```

Construct a `ParserConfig` with default settings with the default values of,  
`backendType` - ParserBackendType::simdjson.  
`comment` - false.  

#### Set/get backendType

```c++
ParserBackendType getBackendType() const;
ParserConfig & setBackendType(const ParserBackendType type);
```

jsonpp doesn't parse JSON document directly, it uses third party JSON parser as the backend.  
Different backend may have different features and performance.  
`backendType` determines which backend is used, the default is ParserBackendType::simdjson.  

`ParserBackendType` is an enum that defines the backend types,  
```c++
enum class ParserBackendType
{
	cparser,
	simdjson,
};
```

`cparser` is [json-parser project](https://github.com/json-parser/json-parser). It's said it has very low footprint.  
`simdjson` is [simdjson project](https://github.com/simdjson/simdjson). It's very high performance.  

#### Set/get comment

```c++
bool allowComment() const;
ParserConfig & enableComment(const bool enable);
```

Set whether C style comment should be parsed in the JSON document. Default is false.  
Note not all backends support comment. Currently only ParserBackendType::cparser supports comment.  

## Example code

desc*/

//desc ### Parse JSON document

ExampleFunc
{
	//code
	//desc Create a parser with default configuration and default backend which is simdjson.
	jsonpp::Parser parser;
	//desc This is the JSON we are going to parse.
	const std::string jsonText = R"(
		[ 5, "abc", true, null, 3.14, [ 1, 2, 3 ], { "one": 1, "two": 2 } ]
	)";
	//desc Parse the JSON, the result is a `metapp::Variant`.
	const metapp::Variant var = parser.parse(jsonText);
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

namespace {

//code

//desc ### Dump/parse class object
//desc Now let's dump and parse customized class objects. First let's define the enum and classes we will use later.

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

} // namespace

//code

//desc Now make the enum and class information availabe to metapp. jsonpp uses the reflection information from metapp.
//desc The information is not special to jsonpp, it's general reflection and can be used for other purposes such
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

ExampleFunc
{
	//code
	//desc Now let's dump `person` to text, then parse the text back to `Person` object.
	//desc `enableNamedEnum(true)` will use the name such as "female" for the Gender enum, instead of numbers such as 0.
	//desc This allows the enum value change without breaking the dumped object.
	Person person { "Mary", Gender::female, 26, { { "Writing", 8 }, { "Cooking", 6 } } };
	jsonpp::Dumper dumper(jsonpp::DumperConfig().enableBeautify(true).enableNamedEnum(true));
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
	jsonpp::Parser parser;
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

	jsonpp::Dumper dumper(jsonpp::DumperConfig().enableBeautify(true).enableNamedEnum(true));
	const std::string jsonText = dumper.dump(std::vector<Person> { personAlice, personTom });
	
	jsonpp::Parser parser;
	const std::vector<Person> parsedPersons = parser.parse<std::vector<Person> >(jsonText);
	ASSERT(parsedPersons[0] == personAlice);
	ASSERT(parsedPersons[1] == personTom);
	//code
}

