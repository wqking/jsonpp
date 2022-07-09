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

#include <memory>
#include <string>
#include <vector>
#include <map>

namespace jsonpp {

using JsonNull = void *;
using JsonBool = bool;
using JsonInt = long long;
using JsonUnsignedInt = unsigned long long;
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

inline JsonType getJsonType(const metapp::Variant & var)
{
	auto metaType = metapp::getNonReferenceMetaType(var);
	const auto typeKind = metaType->getTypeKind();

	switch(typeKind) {
	case metapp::getTypeKind<JsonNull>():
		if(metaType->getUpType()->getTypeKind() == metapp::tkVoid
			&& var.get<void *>() == nullptr) {
			return JsonType::jtNull;
		}
		break;
	
	case metapp::getTypeKind<JsonBool>():
		return JsonType::jtBool;
	
	case metapp::getTypeKind<JsonInt>():
		return JsonType::jtInt;
	
	case metapp::getTypeKind<JsonUnsignedInt>():
		return JsonType::jtUnsignedInt;
	
	case metapp::getTypeKind<JsonReal>():
		return JsonType::jtReal;
	
	case metapp::getTypeKind<JsonString>():
		return JsonType::jtString;
	
	case metapp::getTypeKind<JsonArray>():
		if(metaType->getUpType()->getTypeKind() == metapp::tkVariant) {
			return JsonType::jtArray;
		}
		break;
	
	default:
		if(metaType->equal(metapp::getMetaType<JsonObject>())) {
			return JsonType::jtObject;
		}
		break;

	}

	return JsonType::jtNone;
}

} // namespace jsonpp

#endif
