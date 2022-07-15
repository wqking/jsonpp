[//]: # (Auto generated file, don't modify this file.)

# Use jsonpp::Parser to parse JSON document
<!--begintoc-->
- [Class Parser](#mdtoc_435d0ed9)
  - [Header](#mdtoc_6e72a8c1)
  - [Default constructor](#mdtoc_56b1be22)
  - [Construct with config](#mdtoc_e79a33a1)
  - [parse a series of characters](#mdtoc_4ad2ff01)
  - [parse a std::string](#mdtoc_c02f04a)
  - [parse ParserSource](#mdtoc_924c6482)
  - [Error handling](#mdtoc_e2f32606)
- [ParserSource](#mdtoc_f8e315db)
  - [Header](#mdtoc_6e72a8c2)
- [ParserConfig](#mdtoc_73e345d4)
  - [Header](#mdtoc_6e72a8c3)
  - [Default constructor](#mdtoc_56b1be23)
  - [Set/get backendType](#mdtoc_3d8edcbc)
  - [Set/get comment](#mdtoc_628c8e14)
- [Example code](#mdtoc_3bb166c4)
  - [Parse JSON document](#mdtoc_bdd95779)
  - [Dump/parse class object](#mdtoc_9eda3b58)
<!--endtoc-->

<a id="mdtoc_435d0ed9"></a>
## Class Parser

<a id="mdtoc_6e72a8c1"></a>
#### Header

```c++
#include "jsonpp/parser.h"
```

<a id="mdtoc_56b1be22"></a>
#### Default constructor

```c++
Parser();
```

Construct Parser with default configurations.

<a id="mdtoc_e79a33a1"></a>
#### Construct with config

```c++
explicit Parser(const ParserConfig & config);
```

Construct Parser using `config`.

<a id="mdtoc_4ad2ff01"></a>
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

<a id="mdtoc_c02f04a"></a>
#### parse a std::string

```c++
metapp::Variant parse(const std::string & jsonText, const metapp::MetaType * prototype = nullptr);

template <typename T>
T parse(const std::string & jsonText);
```

Parse the document in `jsonText`.

<a id="mdtoc_924c6482"></a>
#### parse ParserSource

```c++
metapp::Variant parse(const ParserSource & source, const metapp::MetaType * prototype = nullptr);

template <typename T>
T parse(const ParserSource & source);
```

Parse the document in `source`.  
If you needs to repeat parsing the same document, using `ParserSource` may increase performance slightly,
depending on the parse backend.

<a id="mdtoc_e2f32606"></a>
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

<a id="mdtoc_f8e315db"></a>
## ParserSource

<a id="mdtoc_6e72a8c2"></a>
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

<a id="mdtoc_73e345d4"></a>
## ParserConfig

<a id="mdtoc_6e72a8c3"></a>
#### Header

```c++
#include "jsonpp/dumper.h"
```

<a id="mdtoc_56b1be23"></a>
#### Default constructor

```c++
ParserConfig();
```

Construct a `ParserConfig` with default settings with the default values of,  
`backendType` - ParserBackendType::simdjson.  
`comment` - false.  

<a id="mdtoc_3d8edcbc"></a>
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
  cparser,
  simdjson,
};
```

`cparser` is [json-parser project](https://github.com/json-parser/json-parser). It's said it has very low footprint.  
`simdjson` is [simdjson project](https://github.com/simdjson/simdjson). It's very high performance.  

Note `setBackendType` accepts the backend type as template argument because then the linker can eliminate the unused
backend from the executable.

<a id="mdtoc_628c8e14"></a>
#### Set/get comment

```c++
bool allowComment() const;
ParserConfig & enableComment(const bool enable);
```

Set whether C style comment should be parsed in the JSON document. Default is false.  
Note not all backends support comment. Currently only ParserBackendType::cparser supports comment.  

<a id="mdtoc_3bb166c4"></a>
## Example code


<a id="mdtoc_bdd95779"></a>
### Parse JSON document
Create a parser with default configuration and default backend which is simdjson.

```c++
jsonpp::Parser parser;
```

This is the JSON we are going to parse.

```c++
const std::string jsonText = R"(
  [ 5, "abc", true, null, 3.14, [ 1, 2, 3 ], { "one": 1, "two": 2 } ]
)";
```

Parse the JSON, the result is a `metapp::Variant`.

```c++
const metapp::Variant var = parser.parse(jsonText);
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

<a id="mdtoc_9eda3b58"></a>
### Dump/parse class object
Now let's dump and parse customized class objects. First let's define the enum and classes we will use later.

```c++
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
```

Now make the enum and class information availabe to metapp. jsonpp uses the reflection information from metapp.
The information is not special to jsonpp, it's general reflection and can be used for other purposes such
as serialization, script binding, etc.

```c++
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
```

Now let's dump `person` to text, then parse the text back to `Person` object.
`enableNamedEnum(true)` will use the name such as "female" for the Gender enum, instead of numbers such as 0.
This allows the enum value change without breaking the dumped object.

```c++
Person person { "Mary", Gender::female, 26, { { "Writing", 8 }, { "Cooking", 6 } } };
jsonpp::Dumper dumper(jsonpp::DumperConfig().enableBeautify(true).enableNamedEnum(true));
// We don't user `person` any more, so we can move it to `dump` to avoid copying.
const std::string jsonText = dumper.dump(std::move(person));
```

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

Now let's parse the JSON text back to Person object, and verify the values.

```c++
jsonpp::Parser parser;
const Person parsedPerson = parser.parse<Person>(jsonText);
ASSERT(parsedPerson.name == "Mary");
ASSERT(parsedPerson.gender == Gender::female);
ASSERT(parsedPerson.age == 26);
ASSERT(parsedPerson.skills[0].name == "Writing");
ASSERT(parsedPerson.skills[0].level == 8);
ASSERT(parsedPerson.skills[1].name == "Cooking");
ASSERT(parsedPerson.skills[1].level == 6);
```

We can not only dump/parse a single object, but also any STL containers with the objects.

```c++
Person personAlice { "Alice", Gender::female, 28, { { "Excel", 7 }, { "Word", 8 } } };
Person personTom { "Tom", Gender::male, 29, { { "C++", 9 }, { "Python", 10 }, { "PHP", 7 } } };

jsonpp::Dumper dumper(jsonpp::DumperConfig().enableBeautify(true).enableNamedEnum(true));
const std::string jsonText = dumper.dump(std::vector<Person> { personAlice, personTom });

jsonpp::Parser parser;
const std::vector<Person> parsedPersons = parser.parse<std::vector<Person> >(jsonText);
ASSERT(parsedPersons[0] == personAlice);
ASSERT(parsedPersons[1] == personTom);
```
