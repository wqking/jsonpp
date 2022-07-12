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

desc*/
