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


desc*/
