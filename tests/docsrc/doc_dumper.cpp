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
#include "jsonpp/dumper.h"
#include "jsonpp/textoutput.h"

#include "metapp/allmetatypes.h"
#include "metapp/interfaces/metaclass.h"
#include "metapp/interfaces/metaenum.h"

#include <cstring>
#include <sstream>
#include <iostream>

/*desc
# Use jsonpp::Dumper to parse JSON document

## Class Dumper

#### Header

```c++
#include "jsonpp/dumper.h"
```

#### Default constructor

```c++
Dumper();
```

Construct Dumper with default configurations.

#### Construct with config

```c++
explicit Dumper(const DumperConfig & config);
```

Construct Dumper using `config`.

#### dump to string

```c++
std::string dump(const metapp::Variant & value);
```

Dump `value` to `std::string`.

#### dump to customized output

```c++
template <typename Output>
void dump(const metapp::Variant & value, const Output & output);
```

Dump `value` to `output`.

## Class DumperConfig

#### Header

```c++
#include "jsonpp/dumper.h"
```

#### Default constructor

```c++
DumperConfig();
```

Construct a `DumperConfig` with default settings with the default values of,  
`beautify` - false.  
`named enum` - false.  
`indent` - "    ", 4 white spaces.

#### Set/get beautify

```c++
bool allowBeautify() const;
DumperConfig & enableBeautify(const bool enable);
```

If `beautify` is true, proper white spaces, line breaks, and indents are inserted in the dumped text.  
If `beautify` is false, there is no any white spaces, line breaks, or indents. The dumped text is minified.  

#### Set/get named enum

```c++
bool allowNamedEnum() const;
DumperConfig & enableNamedEnum(const bool enable);
```

If `named enum` is true, for enumerator values that have names registed via metapp, the names are dumped. If the values
don't have registered names, the integral values are dumped.  
If `named enum` is false, the integral values are dumped.  
Using enumerator names in the dumped JSON has advantage that, if the enumerator values changed (such as reordered), the
correct values (after changed) can be read back by names.  
Note: When parsing JSON document with named enumerators, `prototype` must be specified and passed to `jsonpp::Parser`.

#### Set/get indent

```c++
DumperConfig & setIndent(const std::string & indent);
const std::string & getIndent() const;
```

The default indent is 4 white spaces.

## TextOutput

`Dumper` supports to dump to customized `output`. `Dumper` doesn't assume anything on the output. The output
can be text format, or binary format. The output can write to `std::string`, write to stream, or even write to socket.  
`jsonpp` has one built-in output class, `TextOutput`, which can output standard text based JSON document.

`TextOutput` is a template class that has one argument of `writer`. A `writer` writes the text to specified storage,
such as `std::string`, `std::vector`, or stream.  

```c++
template <typename Writer>
struct TextOutput;
```

#### Header

```c++
#include "jsonpp/textoutput.h"
```

#### Constructor

```c++
explicit TextOutput(const Writer & writer);
```

Construct a `TextOutput` with `writer`.  
`TextOutput` keeps reference to `writer`. That's to say, `writer` must be
living until `TextOutput` is destroyed.

```c++
TextOutput(const DumperConfig & config, const Writer & writer);
```

Construct a `TextOutput` with `config` and `writer`.  
`TextOutput` copies `config` to internal data, but keeps reference to `writer`. That's to say, `writer` must be
living until `TextOutput` is destroyed.

#### No member functions

There is no member functions for end users. The user only needs to construct a `TextOutput` and passes it to `Dumper`.  
All `TextOutput` member functions are to implement the `Output` protocol, which is not need by end users.  
If you are going to implement new `Output` class, your can either read the `TextOutput` source code, or leave me
a message so I can add document for it.

## Writer classes for TextOutput

There are several built-in writer classes that can be used with `TextOutput`.  

### Header

```c++
#include "jsonpp/textoutput.h"
```

### StringWriter

```c++
struct StringWriter
{
	StringWriter();

	std::string && takeString() const;
	const std::string & getString() const;
};
```

`StringWriter` writes all output to a `std::string`. The string can be obtained by `takeString` or `getString`.

### VectorWriter

```c++
struct VectorWriter
{
	VectorWriter();

	std::vector<char> && takeVector() const;
	const std::vector<char> & getVector() const;
};
```

`VectorWriter` writes all output to a `std::vector<char>`. The vector can be obtained by `takeVector` or `getVector`.

### StreamWriter

```c++
struct StreamWriter
{
	explicit StreamWriter(std::ostream & stream);

	std::ostream & getStream() const;
};

```

`StreamWriter` writes all output to a stream. The stream must be passed to `StreamWriter` constructor.

### Implement writer

A writer must implement two `operator()`.  

```c++
void operator() (const char c) const;
```

Write a single charactor `c`.

```c++
void operator() (const char * s, const std::size_t length) const
```

Write a serias of charactors `s` of `length`.  
Note: `s` is not null terminated.  

Here is the source code of `StringWriter`, for example,  
```c++
struct StringWriter
{
	StringWriter()
		: str()
	{
		str.reserve(1024 * 16);
	}

	void operator() (const char c) const {
		str.push_back(c);
	}

	void operator() (const char * s, const std::size_t length) const {
		str.append(s, length);
	}

	std::string && takeString() const {
		return std::move(str);
	}

	const std::string & getString() const {
		return str;
	}

private:
	mutable std::string str;
};
```

## Example code

desc*/

ExampleFunc
{
	//code
	//desc Dump integer.
	jsonpp::Dumper dumper;
	const std::string jsonText = dumper.dump(5);
	// jsonText is 5
	//code
	(void)jsonText;
}

ExampleFunc
{
	//code
	//desc Dump array.
	jsonpp::Dumper dumper;
	const std::string jsonText = dumper.dump(std::vector<std::string> { "Hello", "world" });
	// jsonText is ["Hello","world"]
	//code
	(void)jsonText;
}

ExampleFunc
{
	//code
	//desc Dump beautified object.
	jsonpp::Dumper dumper(jsonpp::DumperConfig().enableBeautify(true));
	const std::string jsonText = dumper.dump(std::map<std::string, int> { { "one", 1}, { "two", 2 } });
	//std::cout << jsonText << std::endl;
	/*desc
	jsonText is
```
{
	"one": 1,
	"two": 2
}
```
	desc*/
	//code
	(void)jsonText;
}

ExampleFunc
{
	//code
	//desc Dump to stream.
	jsonpp::Dumper dumper;
	jsonpp::StreamWriter writer(std::cout);
	jsonpp::TextOutput<jsonpp::StreamWriter> streamOutput(writer);
	dumper.dump(5, streamOutput);
	// output 5
	//code
}

/*desc

desc*/
