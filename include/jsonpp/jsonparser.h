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

#include <memory>

namespace jsonpp {

namespace jsonparser_internal_ {
class Implement;
} // namespace jsonparser_internal_

class ParserConfig
{
public:
	ParserConfig()
		:
			nullType(),
			booleanType(),
			integerType(),
			doubleType(),
			stringType(),
			arrayType(),
			objectType()
	{
	}

	ParserConfig & setNullType(const metapp::MetaType * nullType_) {
		nullType = nullType_;
		return *this;
	}

	ParserConfig & setBooleanType(const metapp::MetaType * booleanType_) {
		booleanType = booleanType_;
		return *this;
	}

	ParserConfig & setIntegerType(const metapp::MetaType * integerType_) {
		integerType = integerType_;
		return *this;
	}

	ParserConfig & setDoubleType(const metapp::MetaType * doubleType_) {
		doubleType = doubleType_;
		return *this;
	}

	ParserConfig & setStringType(const metapp::MetaType * stringType_) {
		stringType = stringType_;
		return *this;
	}

	ParserConfig & setArrayType(const metapp::MetaType * arrayType_) {
		arrayType = arrayType_;
		return *this;
	}

	ParserConfig & setObjectType(const metapp::MetaType * objectType_) {
		objectType = objectType_;
		return *this;
	}

	const metapp::MetaType * getNullType() const {
		return nullType;
	}

	const metapp::MetaType * getBooleanType() const {
		return booleanType;
	}

	const metapp::MetaType * getIntegerType() const {
		return integerType;
	}

	const metapp::MetaType * getDoubleType() const {
		return doubleType;
	}

	const metapp::MetaType * getStringType() const {
		return stringType;
	}

	const metapp::MetaType * getArrayType() const {
		return arrayType;
	}

	const metapp::MetaType * getObjectType() const {
		return objectType;
	}

private:
	const metapp::MetaType * nullType;
	const metapp::MetaType * booleanType;
	const metapp::MetaType * integerType;
	const metapp::MetaType * doubleType;
	const metapp::MetaType * stringType;
	const metapp::MetaType * arrayType;
	const metapp::MetaType * objectType;
};

class JsonParser
{
public:
	JsonParser();
	explicit JsonParser(const ParserConfig & config);
	~JsonParser();

	bool hasError() const;
	std::string getError() const;

	metapp::Variant parse(const char * jsonText, const size_t length, const metapp::MetaType * proto = nullptr);
	metapp::Variant parse(const std::string & jsonText, const metapp::MetaType * proto = nullptr);

private:
	std::unique_ptr<jsonparser_internal_::Implement> implement;
};

} // namespace jsonpp

#endif
