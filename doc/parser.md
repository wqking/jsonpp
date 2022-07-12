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
If `prototype` is nullptr, the document is parsed as default data types.  
If `prototype` is not nullptr, the document is parsed as `prototype`.  

If there is no any error, a `metapp::Variant` is returned that represents the document.  
If there is any error occurred, empty Variant is returned (Variant::isEmpty() is true).  

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

<a id="mdtoc_628c8e14"></a>
#### Set/get comment

```c++
bool allowComment() const;
ParserConfig & enableComment(const bool enable);
```

Set whether C style comment should be parsed in the JSON document. Default is false.  
Note not all backends support comment. Currently only ParserBackendType::cparser supports comment.  


