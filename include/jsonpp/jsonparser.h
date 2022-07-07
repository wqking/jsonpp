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

#ifndef JSONPP_JSONPARSER_H_821598293712
#define JSONPP_JSONPARSER_H_821598293712

#include "metapp/variant.h"
#include "metapp/allmetatypes.h"

#include <memory>
#include <string>
#include <vector>
#include <map>

#ifndef JSONPP_BACKEND_CPARSER
#define JSONPP_BACKEND_CPARSER 1
#endif

#ifndef JSONPP_BACKEND_SIMDJSON
#define JSONPP_BACKEND_SIMDJSON 1
#endif

namespace jsonpp {

namespace internal_ {
class ParserBackend;
} // namespace internal_

using JsonNull = void *;
using JsonBool = bool;
using JsonInt = long long;
using JsonUnsignedInt = unsigned long long;
using JsonReal = double;
using JsonString = std::string;
using JsonArray = std::vector<metapp::Variant>;
using JsonObject = std::map<std::string, metapp::Variant>;

class ParserConfig
{
public:
	ParserConfig()
		:
			arrayType(),
			objectType()
	{
	}

	ParserConfig & setArrayType(const metapp::MetaType * arrayType_) {
		arrayType = arrayType_;
		return *this;
	}

	ParserConfig & setObjectType(const metapp::MetaType * objectType_) {
		objectType = objectType_;
		return *this;
	}

	const metapp::MetaType * getArrayType() const {
		return arrayType;
	}

	const metapp::MetaType * getObjectType() const {
		return objectType;
	}

private:
	const metapp::MetaType * arrayType;
	const metapp::MetaType * objectType;
};

enum class ParserType
{
	cparser,
	simdjsonDom,
	simdjsonOnDemand
};

class JsonParserSource
{
public:
	JsonParserSource();
	JsonParserSource(const char * cstr, const std::size_t cstrLength);
	explicit JsonParserSource(const std::string & str);
	explicit JsonParserSource(std::string && str);

	const char * getText() const;
	std::size_t getTextLength() const;
	std::size_t getCapacity() const;

	void pad(const std::size_t size) const;

private:
	bool hasPrepared() const {
		return prepared;
	}

	void setAsPrepared() const {
		prepared = true;
	}

private:
	mutable bool prepared;
	mutable const char * cstr;
	mutable std::size_t cstrLength;
	mutable std::string str;

	friend class JsonParser;
};

class JsonParser
{
public:
	JsonParser();
	explicit JsonParser(const ParserType parserType);
	explicit JsonParser(const ParserConfig & config, const ParserType parserType);
	~JsonParser();

	bool hasError() const;
	std::string getError() const;

	metapp::Variant parse(const char * jsonText, const std::size_t length, const metapp::MetaType * proto = nullptr);
	metapp::Variant parse(const std::string & jsonText, const metapp::MetaType * proto = nullptr);
	metapp::Variant parse(const JsonParserSource & source, const metapp::MetaType * proto = nullptr);

private:
	std::unique_ptr<internal_::ParserBackend> backend;
};

std::string getParserTypeName(const ParserType type);

} // namespace jsonpp

#endif
