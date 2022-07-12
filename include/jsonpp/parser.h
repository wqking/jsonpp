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

#ifndef JSONPP_PARSER_H_821598293712
#define JSONPP_PARSER_H_821598293712

#include "jsonpp/common.h"

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

#if ! (JSONPP_BACKEND_CPARSER || JSONPP_BACKEND_SIMDJSON)
#error "jsonpp - no parser backend is specified"
#endif

namespace jsonpp {

enum class ParserBackendType;

namespace internal_ {

ParserBackendType getDefaultBackend();

} // namespace internal_

enum class ParserBackendType
{
	cparser,
	simdjson,
};

class ParserBackend;

class ParserConfig
{
public:
	ParserConfig()
		:
			backendType(internal_::getDefaultBackend()),
			comment(false),
			arrayType(),
			objectType()
	{
	}

	ParserBackendType getBackendType() const {
		return backendType;
	}

	ParserConfig & setBackendType(const ParserBackendType type) {
		backendType = type;
		return *this;
	}

	bool allowComment() const {
		return comment;
	}

	ParserConfig & enableComment(const bool enable) {
		comment = enable;
		return *this;
	}

	const metapp::MetaType * getArrayType() const {
		return arrayType;
	}

	ParserConfig & setArrayType(const metapp::MetaType * arrayType_) {
		arrayType = arrayType_;
		return *this;
	}

	const metapp::MetaType * getObjectType() const {
		return objectType;
	}

	ParserConfig & setObjectType(const metapp::MetaType * objectType_) {
		objectType = objectType_;
		return *this;
	}

private:
	ParserBackendType backendType;
	bool comment;
	const metapp::MetaType * arrayType;
	const metapp::MetaType * objectType;
};

class ParserSource
{
private:
	enum class StorageType {
		cstr,
		ref,
		string
	};

public:
	ParserSource();
	ParserSource(const char * cstr, const std::size_t cstrLength);
	explicit ParserSource(const std::string & str);
	explicit ParserSource(std::string && str);

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
	mutable StorageType storageType;
	mutable const char * cstr;
	mutable std::size_t cstrLength;
	mutable const std::string * ref;
	mutable std::string str;

	friend class Parser;
};

class Parser
{
public:
	Parser();
	explicit Parser(const ParserConfig & config);
	~Parser();

	bool hasError() const;
	std::string getError() const;

	metapp::Variant parse(const char * jsonText, const std::size_t length, const metapp::MetaType * prototype = nullptr);
	metapp::Variant parse(const std::string & jsonText, const metapp::MetaType * prototype = nullptr);
	metapp::Variant parse(const ParserSource & source, const metapp::MetaType * prototype = nullptr);

	template <typename T>
	T parse(const char * jsonText, const std::size_t length) {
		const metapp::Variant result = parse(jsonText, length, metapp::getMetaType<T>());
		if(hasError()) {
			return T();
		}
		return result.get<const T &>();
	}

	template <typename T>
	T parse(const std::string & jsonText) {
		const metapp::Variant result = parse(jsonText, metapp::getMetaType<T>());
		if(hasError()) {
			return T();
		}
		return result.get<const T &>();
	}

	template <typename T>
	T parse(const ParserSource & source) {
		const metapp::Variant result = parse(source, metapp::getMetaType<T>());
		if(hasError()) {
			return T();
		}
		return result.get<const T &>();
	}

private:
	std::unique_ptr<ParserBackend> backend;
};

std::string getParserBackendName(const ParserBackendType type);

} // namespace jsonpp

#endif
