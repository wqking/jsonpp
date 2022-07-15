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

#ifndef JSONPP_COMMON_H_821598293712
#define JSONPP_COMMON_H_821598293712

#include "metapp/variant.h"
#include "metapp/allmetatypes.h"
#include "metapp/interfaces/metaclass.h"
#include "metapp/interfaces/metaindexable.h"
#include "metapp/interfaces/metamappable.h"
#include "metapp/interfaces/metaenum.h"
#include "metapp/utilities/utility.h"
#include "metapp/compiler.h"

#include <memory>
#include <string>
#include <vector>
#include <map>
#include <cstdint>

namespace jsonpp {

using JsonNull = void *;
using JsonBool = bool;
using JsonInt = int64_t;
using JsonUnsignedInt = uint64_t;
using JsonReal = double;
using JsonString = std::string;
using JsonArray = std::vector<metapp::Variant>;
using JsonObject = std::map<std::string, metapp::Variant>;

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

JsonType getJsonType(const metapp::Variant & var);


} // namespace jsonpp

#endif
