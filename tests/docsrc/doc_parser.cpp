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

Parse JSON document at `jsonText` of `length` characters. `jsonText` doesn't need to be null terminated.  
If `prototype` is nullptr, the document is parsed as default data types. Please check [this document](common_types.md) for the
default data types.  
If `prototype` is not nullptr, the document is parsed as `prototype`.  

For the first form,  
If there is no any error, a `metapp::Variant` is returned that represents the document.  
If there is any error occurred, empty Variant is returned (Variant::isEmpty() is true).  

For the second templated form,  
If there is no any error, the parsed value of `T` is returned.  
If there is any error occurred, the default constructed `T()` is returned.  

Some parser backends have special requirements on the data alignment and padding, `jsonpp` will handle such requirements
internally, so the caller of `parse` don't need to care about the requirements.  

The second template form is same as,  
```
jsonpp::Parser parser;

// template form, `object` must be value, it can't be reference.
T object = parser.parse<T>(jsonText, length);

// equals to, `object` can be reference since `var` holds the object.
metapp::Variant var = parser.parse(jsonText, length, metapp::getMetaType<T>());
const T & object = var.get<const T &>();
```

#### parse a std::string

```c++
metapp::Variant parse(const std::string & jsonText, const metapp::MetaType * prototype = nullptr);

template <typename T>
T parse(const std::string & jsonText);
```

Parse JSON document in `jsonText`.

#### parse ParserSource

```c++
metapp::Variant parse(const ParserSource & source, const metapp::MetaType * prototype = nullptr);

template <typename T>
T parse(const ParserSource & source);
```

Parse JSON document in `source`.  
If you needs to repeat parsing the same document, using `ParserSource` may increase performance slightly,
depending on the parser backend.

#### Error handling

```c++
bool hasError() const;
std::string getError() const;
```

`hasError()` returns true if there is any error occurred during previous `parse`, false if it succeeds.  
`getError()` returns the error message if there is any error.  
Both `metapp` and the parser backend may throw exceptions, all exceptions are captured and converted to error message.
So `parse` functions don't throw any exceptions.  
`metapp` works if exceptions are disabled in compiler, for the parser backend, please check their document to see
if exceptions can be disabled.

#### The input data

The input data must be UTF-8 text or plain ASCII text which is a subset of UTF-8. If the input is other unicode encoding,
you need to convert it to UTF-8 beforehand. The input can't contain '\0'.

#### Use the parsed result

In this section we will use the overloaded function `parse(const std::string & jsonText)` as example, we can apply the same
knowledge to the other overloaded functions.

The function `parse` can be called in 3 forms,

```c++
// #1 prototype is nullptr
metapp::Variant parse(const std::string & jsonText);

// #2 prototype is not nullptr
metapp::Variant parse(const std::string & jsonText, const metapp::MetaType * prototype);

// #3 templated version
template <typename T>
T parse(const std::string & jsonText);
```

**#1 prototype is nullptr**  
The document is parsed as default data types. Please check [this document](common_types.md) for the default data types.  
If the `arrayType` in ParserConfig is not nullptr, JSON array is parsed as `arrayType`.  
If the `objectType` in ParserConfig is not nullptr, JSON object is parsed as `objectType`.  

If both the `arrayType` and `objectType` in ParserConfig are nullptr, then we can use below pseudo code to retrieve
the native C++ types from the parsed result.  
```c++
metapp::Variant doc = myParser.parse(document);
if(myParser.hasError()) {
	// handle error here, and return
}
const auto type = jsonpp::getJsonType(doc);
if(type == jsonpp::JsonType::jtNull) {
	// process null
}
else if(type == jsonpp::JsonType::jtBool) {
	jsonpp::JsonBool value = doc.get<jsonpp::JsonBool>();
	// process value
}
else if(type == jsonpp::JsonType::jtInt) {
	jsonpp::JsonInt value = doc.get<jsonpp::JsonInt>();
	// process value
}
else if(type == jsonpp::JsonType::jtUnsignedInt) {
	jsonpp::JsonUnsignedInt value = doc.get<jsonpp::JsonUnsignedInt>();
	// process value
}
else if(type == jsonpp::JsonType::jtReal) {
	jsonpp::JsonReal value = doc.get<jsonpp::JsonReal>();
	// process value
}
else if(type == jsonpp::JsonType::jtString) {
	// Get as reference to avoid copying
	const jsonpp::JsonString & value = doc.get<const jsonpp::JsonString &>();
	// process value
}
else if(type == jsonpp::JsonType::jtArray) {
	// Get as reference to avoid copying
	const jsonpp::JsonArray & value = doc.get<const jsonpp::JsonArray &>();
	// process value
}
else if(type == jsonpp::JsonType::jtObject) {
	// Get as reference to avoid copying
	const jsonpp::JsonObject & value = doc.get<const jsonpp::JsonObject &>();
	// process value
}
```

If `arrayType` or `objectType` in ParserConfig is not nullptr, then you need to check if any Variant is the specified type
of `arrayType` or `objectType`, or any other type. To check if a Variant is any type,  
```c++
// Assume the type we want to check is std::vector<int>
metapp::Variant var = // get the var from either parser result, or a JSON node inside the result
if(var.canGet<std::vector<int> >()) { // use the raw type, don't use reference such as `std::vector<int> &`
	const std::vector<int> & myArray = var.get<std::vector<int> >();
	// process myArray here
}
```

**#2 prototype is not nullptr**  
The document is parsed as the type of `prototype`, the result Variant can be converted to the type of `prototype`.  
For example, assume we have a type `MyStruct`, then,  
```c++
metapp::Variant doc = myJsonppParser.parse("What ever json text", metapp::getMetaType<MyStruct>());

// You may want to check error before calling below line.
const MyStruct & myStruct = doc.get<>(const MyStruct &);
```

**#3 templated version**  
This form is the simplest way to use.  
```c++
MyStruct myStruct = myJsonppParser.parse<MyStruct>("What ever json text");
```
Note: unlike #2, here `myStruct` must be value and not reference, because #2 has the Variant to hold the object, but in this form,
there is no variable to hold the object.  
Note: you should prefer this form to #2. You should only use #2 for advanced usage, such as the prototype is obtained at runtime
and you don't know the compile time type.

## Class ParserConfig

#### Header

```c++
#include "jsonpp/parser.h"
```

#### Default constructor

```c++
ParserConfig();
```

Construct a `ParserConfig` with default settings with the default values of,  
`backendType` - ParserBackendType::simdjson.  
`comment` - false.  
`arrayType` - nullptr.  
`objectType` - nullptr.  

#### Set/get backendType

```c++
ParserBackendType getBackendType() const;

template <ParserBackendType type>
ParserConfig & setBackendType();
```

jsonpp doesn't parse JSON document directly, it uses third party JSON parser as the backend.  
Different backend may have different features and performance.  
`backendType` determines which backend is used, the default is ParserBackendType::simdjson.  

`ParserBackendType` is an enum that defines the backend types,  
```c++
enum class ParserBackendType
{
	simdjson,
	cparser,
};
```

Please see section "Parser backend comparison" for more details.

Note `setBackendType` accepts the backend type as template argument because then the linker can eliminate the unused
backend from the executable.

#### Set/get comment

```c++
bool allowComment() const;
ParserConfig & enableComment(const bool enable);
```

Set whether C style comment should be parsed in the JSON document. Default is false.  
Note not all backends support comment. Currently only `ParserBackendType::cparser` supports comment.  

#### Set/get array type

```c++
const metapp::MetaType * getArrayType() const;

template <typename T>
ParserConfig & setArrayType();

ParserConfig & setArrayType(const metapp::MetaType * arrayType);
```

Set the array type to represent JSON array. Default is nullptr. If it's nullptr, `jsonpp::JsonArray` is used.  
If the argument `prototype` in `Parser::parse` is not nullptr, the array type is ignored. Otherwise, all arrays are parsed
as the array type.  
The array type can be `std::vector`, `std::deque`, `std::list`, `std::array`, `std::tuple`, `std::pair`, or any containers that
implements meta interface `metapp::MetaIndexable`. For fixed size containers such as `std::array`, `std::tuple`, or `std::pair`,
the capacity must be large enough to hold the parsed elements.  
The element type must be able to casted from the value in the JSON document.  

#### Set/get object type

```c++
const metapp::MetaType * getObjectType() const;

template <typename T>
ParserConfig & setObjectType();

ParserConfig & setObjectType(const metapp::MetaType * objectType);
```

Set the object type to represent JSON object. Default is nullptr. If it's nullptr, `jsonpp::JsonObject` is used.  
If the argument `prototype` in `Parser::parse` is not nullptr, the object type is ignored. Otherwise, all objects are parsed
as the object type.  
The object type can be `std::map<std::string, T>`, `std::unordered_map<std::string, T>`, or any containers that
implements meta interface `metapp::MetaMappable`. The type `T` must be able to casted from the value in the JSON document.  
The object type can also be sequence containers such as `std::vector`, `std::deque`, `std::list`, `std::array` with enough elements,
or any containers that implements meta interface `metapp::MetaIndexable`. The element type can be `std::pair<std::string, T>`, or
any sequence containers which size can grow to at least 2.

#### Difference between array/object type in ParserConfig and argument `prototype` in function `Parser::parse`

When the argument `prototype` is not nullptr in function `Parser::parse` (or the templated `parse` function), the whole
JSON document is parsed as `prototype`. In such case, the array and object type in ParserConfig are ignored.  

If array type in ParserConfig is not nullptr (set by `setArrayType`), all other data are parsed as the default data types,
and all arrays are parsed as `getArrayType`. It's same for object type.

## Class ParserSource

#### Header

```c++
#include "jsonpp/parser.h"
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
Note `ParserSource` refers to the string, so the content must be available until the `ParserSource` is destroyed.

## Example code

desc*/

ExampleFunc
{
	//code
	//desc ### Parse JSON document

	// Create a parser with default configuration and default backend which is simdjson.
	jsonpp::Parser parser;

	// This is the JSON we are going to parse.
	const std::string jsonText = R"(
		[ 5, "abc", true, null, 3.14, [ 1, 2, 3 ], { "one": 1, "two": 2 } ]
	)";

	// Parse the JSON, the result is a `metapp::Variant`.
	const metapp::Variant var = parser.parse(jsonText);

	// The result is an array.
	ASSERT(jsonpp::getJsonType(var) == jsonpp::JsonType::jtArray);

	// Get the underlying array. jsonpp::JsonArray is alias of `std::vector<metapp::Variant>`
	const jsonpp::JsonArray & array = var.get<const jsonpp::JsonArray &>();

	// Now verify the elements.
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

ExampleFunc
{
	//code
	//desc ### Parse as prototype
	
	// Create a parser.
	jsonpp::Parser parser;

	// This is the JSON we are going to parse.
	const std::string jsonText = R"(
		{ "a" : [ 3.1, 4, 15, 9 ], "b" : [ -1, -2, -3 ] }
	)";
	
	// Parse the JSON as `std::map<std::string, std::vector<int> >`
	const std::map<std::string, std::vector<int> > object = parser.parse<std::map<std::string, std::vector<int> > >(jsonText);
	// Above line equals to these two lines,
	//const metapp::Variant var = parser.parse(jsonText, metapp::getMetaType<std::map<std::string, std::vector<int> > >());
	//const std::map<std::string, std::vector<int> > & object = var.get<std::map<std::string, std::vector<int> > >();

	// Note: since the array is parsed as std::vector<int>, the float 3.1 is converted to 3
	ASSERT(object.at("a") == std::vector<int> { 3, 4, 15, 9 });
	ASSERT(object.at("b") == std::vector<int> { -1, -2, -3 });
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

//desc Now make the enum and class information available to metapp. jsonpp uses the reflection information from metapp.
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

ExampleFunc
{
	//code
	//desc ### Parse with specified array type
	
	// Create the config, we want all JSON arrays parsed as std::deque<int> instead of JsonArray.
	jsonpp::ParserConfig parserConfig;
	parserConfig.setArrayType<std::deque<int> >();
	// Create a parser with parserConfig
	jsonpp::Parser parser(parserConfig);

	// This is the JSON we are going to parse.
	const std::string jsonText = R"(
		{ "a" : [ 3.1, 4, 15, 9 ], "b" : [ -1, -2, -3 ] }
	)";
	
	// Parse the JSON
	const metapp::Variant var = parser.parse(jsonText);
	const jsonpp::JsonObject & object = var.get<const jsonpp::JsonObject &>();

	ASSERT(object.at("a").get<const std::deque<int> & >() == std::deque<int> { 3, 4, 15, 9 });
	ASSERT(object.at("b").get<const std::deque<int> & >() == std::deque<int> { -1, -2, -3 });
	//code
}

ExampleFunc
{
	//code
	//desc ### Parse with specified object type
	
	// Create the config, we want all JSON arrays parsed as std::deque<int> instead of JsonArray.
	jsonpp::ParserConfig parserConfig;
	parserConfig.setObjectType<std::unordered_map<std::string, int> >();
	// Create a parser with parserConfig
	jsonpp::Parser parser(parserConfig);

	// This is the JSON we are going to parse.
	const std::string jsonText = R"(
		[ { "a" : 1.2, "b" : 3 } ]
	)";
	
	// Parse the JSON
	const metapp::Variant var = parser.parse(jsonText);
	const jsonpp::JsonArray & array = var.get<const jsonpp::JsonArray &>();

	ASSERT(array[0].get<const std::unordered_map<std::string, int> & >().at("a") == 1);
	ASSERT(array[0].get<const std::unordered_map<std::string, int> & >().at("b") == 3);
	//code
}

ExampleFunc
{
	//code
	//desc ### Parse object as std::vector
	
	// Create the config, we want all JSON arrays parsed as std::deque<int> instead of JsonArray.
	jsonpp::ParserConfig parserConfig;
	parserConfig.setObjectType<std::vector<std::pair<std::string, int> > >();
	// Create a parser with parserConfig
	jsonpp::Parser parser(parserConfig);

	// This is the JSON we are going to parse.
	const std::string jsonText = R"(
		{ "a" : 1.2, "b" : 3 }
	)";
	
	// Parse the JSON
	const metapp::Variant var = parser.parse(jsonText);
	const std::vector<std::pair<std::string, int> > & array
		= var.get<const std::vector<std::pair<std::string, int> > &>();

	ASSERT(array[0].first == "a");
	ASSERT(array[0].second == 1);
	ASSERT(array[1].first == "b");
	ASSERT(array[1].second == 3);

	// Since the whole JSON document is an object, we can also parse it with `prototype`
	const std::vector<std::pair<std::string, int> > array2
		= parser.parse<std::vector<std::pair<std::string, int> > >(jsonText);
	ASSERT(array2 == array);
	//code
}

/*desc
## Parser backend comparison

`ParserBackendType::simdjson` - [simdjson project](https://github.com/simdjson/simdjson).  
`ParserBackendType::cparser` - [json-parser project](https://github.com/json-parser/json-parser).  

| Feature            | simdjson    | cparser       |
|--------------------|-------------|---------------|
| Performance        | Very high   | Not very slow |
| Input encoding     | UTF-8       | UTF-8         |
| \0' in JSON string | Support     | Not support   |
| Comment in JSON    | Not support | Support       |
| Trailing comma     | Reject      | Pass          |
| Memory usage       | High        | Low           |

Note: simdjson has very high performance on computers with SIMD instructions. For computers without SIMD support, the performance
is not that high.

desc*/