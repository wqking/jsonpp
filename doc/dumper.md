[//]: # (Auto generated file, don't modify this file.)

# Use jsonpp::Dumper to parse JSON document
<!--begintoc-->
- [Class Dumper](#mdtoc_44468686)
  - [Header](#mdtoc_6e72a8c1)
  - [Default constructor](#mdtoc_56b1be22)
  - [Construct with config](#mdtoc_e79a33a1)
  - [dump to string](#mdtoc_71d693d0)
  - [dump to customized output](#mdtoc_5f8dfe6a)
- [How Dumper dumps array and object types](#mdtoc_f49c0253)
- [Class DumperConfig](#mdtoc_eb29bf76)
  - [Header](#mdtoc_6e72a8c2)
  - [Default constructor](#mdtoc_56b1be23)
  - [Set/get beautify](#mdtoc_3bf56ba2)
  - [Set/get named enum](#mdtoc_f77a6648)
  - [Set/get indent](#mdtoc_af8f8f5b)
- [TextOutput](#mdtoc_1052425f)
  - [Header](#mdtoc_6e72a8c3)
  - [Constructor](#mdtoc_7dd91a39)
  - [No member functions](#mdtoc_1e77cf9)
- [Writer classes for TextOutput](#mdtoc_99724e83)
  - [Header](#mdtoc_6e72a8c4)
  - [StringWriter](#mdtoc_c9946f83)
  - [VectorWriter](#mdtoc_52e0ff9f)
  - [StreamWriter](#mdtoc_52f0d175)
  - [Implement writer](#mdtoc_a75ebdd5)
- [Example code](#mdtoc_3bb166c4)
<!--endtoc-->

<a id="mdtoc_44468686"></a>
## Class Dumper

<a id="mdtoc_6e72a8c1"></a>
#### Header

```c++
#include "jsonpp/dumper.h"
```

<a id="mdtoc_56b1be22"></a>
#### Default constructor

```c++
Dumper();
```

Construct Dumper with default configurations.

<a id="mdtoc_e79a33a1"></a>
#### Construct with config

```c++
explicit Dumper(const DumperConfig & config);
```

Construct Dumper using `config`.

<a id="mdtoc_71d693d0"></a>
#### dump to string

```c++
std::string dump(const metapp::Variant & value);
```

Dump `value` to `std::string`.  
`value` can be any data, it will be converted to `metapp::Variant` automatically.  
To avoid copying large object, we can either move the object, or use metapp::Variant::reference. For example,  
```c++
LargeObject object;
// move the object
jsonpp::Dumper()::dump(std::move(object));
// reference to the object
jsonpp::Dumper()::dump(metapp::Variant::reference(object));
// rvalue object, move to `dump`
jsonpp::Dumper()::dump(LargeObject());
```

<a id="mdtoc_5f8dfe6a"></a>
#### dump to customized output

```c++
template <typename Output>
void dump(const metapp::Variant & value, const Output & output);
```

Dump `value` to `output`.

<a id="mdtoc_f49c0253"></a>
## How Dumper dumps array and object types

Dumper dumps belows types as JSON object,
1. The meta type implements meta interface `MetaMappable`.  
2. Or the meta type implements meta interface `MetaClass`.  
Usually `std::map`, `std::unordered_map`, and customized classes are dumped as object.

Dumper dumps belows types as JSON array,
1. The meta type implements meta interface `MetaIndexable`.  
2. Or the meta type implements meta interface `MetaIterable`.  
Usually `std::vector`, `std::deque`, `std::list`, `std::array`, `std::pair`, and `std::tuple` are dumped as array.

<a id="mdtoc_eb29bf76"></a>
## Class DumperConfig

<a id="mdtoc_6e72a8c2"></a>
#### Header

```c++
#include "jsonpp/dumper.h"
```

<a id="mdtoc_56b1be23"></a>
#### Default constructor

```c++
DumperConfig();
```

Construct a `DumperConfig` with default settings with the default values of,  
`beautify` - false.  
`named enum` - false.  
`indent` - "    ", 4 white spaces.

<a id="mdtoc_3bf56ba2"></a>
#### Set/get beautify

```c++
bool allowBeautify() const;
DumperConfig & enableBeautify(const bool enable);
```

If `beautify` is true, proper white spaces, line breaks, and indents are inserted in the dumped text.  
If `beautify` is false, there is no any white spaces, line breaks, or indents. The dumped text is minified.  

<a id="mdtoc_f77a6648"></a>
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

<a id="mdtoc_af8f8f5b"></a>
#### Set/get indent

```c++
DumperConfig & setIndent(const std::string & indent);
const std::string & getIndent() const;
```

The default indent is 4 white spaces.

<a id="mdtoc_1052425f"></a>
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

<a id="mdtoc_6e72a8c3"></a>
#### Header

```c++
#include "jsonpp/textoutput.h"
```

<a id="mdtoc_7dd91a39"></a>
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

<a id="mdtoc_1e77cf9"></a>
#### No member functions

There is no member functions for end users. The user only needs to construct a `TextOutput` and passes it to `Dumper`.  
All `TextOutput` member functions are to implement the `Output` protocol, which is not need by end users.  
If you are going to implement new `Output` class, your can either read the `TextOutput` source code, or leave me
a message so I can add document for it.

<a id="mdtoc_99724e83"></a>
## Writer classes for TextOutput

There are several built-in writer classes that can be used with `TextOutput`.  

<a id="mdtoc_6e72a8c4"></a>
### Header

```c++
#include "jsonpp/textoutput.h"
```

<a id="mdtoc_c9946f83"></a>
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

<a id="mdtoc_52e0ff9f"></a>
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

<a id="mdtoc_52f0d175"></a>
### StreamWriter

```c++
struct StreamWriter
{
  explicit StreamWriter(std::ostream & stream);

  std::ostream & getStream() const;
};

```

`StreamWriter` writes all output to a stream. The stream must be passed to `StreamWriter` constructor.

<a id="mdtoc_a75ebdd5"></a>
### Implement writer

A writer must implement two `operator()`.  

```c++
void operator() (const char c) const;
```

Write a single charactor `c`.

```c++
void operator() (const char * s, const std::size_t length) const
```

Write a series of characters `s` of `length`.  
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

<a id="mdtoc_3bb166c4"></a>
## Example code


Dump integer.

```c++
jsonpp::Dumper dumper;
const std::string jsonText = dumper.dump(5);
// jsonText is 5
```

Dump array.

```c++
jsonpp::Dumper dumper;
const std::string jsonText = dumper.dump(std::vector<std::string> { "Hello", "world" });
// jsonText is ["Hello","world"]
```

Dump beautified object.

```c++
jsonpp::Dumper dumper(jsonpp::DumperConfig().enableBeautify(true));
const std::string jsonText = dumper.dump(std::map<std::string, int> { { "one", 1}, { "two", 2 } });
//std::cout << jsonText << std::endl;
```

jsonText is
```
{
  "one": 1,
  "two": 2
}
```

Dump to stream.

```c++
jsonpp::Dumper dumper;
jsonpp::StreamWriter writer(std::cout);
jsonpp::TextOutput<jsonpp::StreamWriter> streamOutput(writer);
dumper.dump(5, streamOutput);
// output 5
```
