[//]: # (Auto generated file, don't modify this file.)

# Common data types

## Overview

`jsonpp` doesn't enforce the data types when parsing or dumping JSON document. For example, `short int`, `unsigned int`, or other
integral types can be used as JSON integers, `std::vector<std::string>`, `std::list<int>`, or other sequence containers,
can be used as JSON array.  

When parsing JSON without specifying the `prototype` argument, the parser will use default data types to store the JSON data.
This document discuss the default data types.

#### Header

```c++
#include "jsonpp/common.h"
```

#### Default data types

```c++
using JsonNull = void *;
using JsonBool = bool;
using JsonInt = int64_t;
using JsonUnsignedInt = uint64_t;
using JsonReal = double;
using JsonString = std::string;
using JsonArray = std::vector<metapp::Variant>;
using JsonObject = std::map<std::string, metapp::Variant>;
```

`JsonNull` represents `null`.  
`JsonBool` represents JSON boolean type, `true` and `false`.  
`JsonInt` represents JSON integral type. The integer can be negative, zero, or positive.  
`JsonUnsignedInt` represents unsigned integer. Not all parser backends support it. Currently only simdjson supports it. Usually an integer
is parsed as unsigned if it's positive and larger than the maximum value of integer.  
`JsonReal` represents JSON float point number.  
`JsonString` represents JSON string.  
`JsonArray` represents JSON array. It's heterogeneous data structure, the element type is `metapp::Variant`,
so any data types can be stored.  
`JsonObject` represents JSON object. It's heterogeneous data structure, the mapped value type is `metapp::Variant`,
so any data types can be stored.  

#### Type constants

There are enum values to identify the types. `jtNone` means it's not any known default data type.

```c++
enum class JsonType {
  jtNone,
  jtNull,
  jtBool,
  jtInt,
  jtUnsignedInt,
  jtReal,
  jtString,
  jtArray,
  jtObject
};
```

#### Function getJsonType

```c++
JsonType getJsonType(const metapp::Variant & var);
```

Returns the data type of `var` if it holds any default data type, otherwise returns `JsonType::jtNone`.


