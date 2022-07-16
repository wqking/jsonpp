// metapp library
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

#include "jsonpp/parser.h"
#include "jsonpp/parserbackend.h"

#include "metapp/allmetatypes.h"
#include "metapp/interfaces/metaclass.h"
#include "metapp/interfaces/metaindexable.h"
#include "metapp/interfaces/metamappable.h"
#include "metapp/compiler.h"

#include <array>
#include <limits>

#ifndef JSONPP_DEFAULT_PARSER_BACKEND
#define JSONPP_DEFAULT_PARSER_BACKEND simdjson
#endif

namespace jsonpp {

namespace internal_ {

std::unique_ptr<ParserBackend> createBackend_cparser(const ParserConfig & config);
std::unique_ptr<ParserBackend> createBackend_simdjsonDom(const ParserConfig & config);

} // namespace internal_

JsonType getJsonType(const metapp::Variant & var)
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

std::string getParserBackendName(const ParserBackendType type)
{
	switch(type) {
	case ParserBackendType::cparser:
		return "json-parser";

	case ParserBackendType::simdjson:
		return "simdjson";
	}

	return "Unknown";
}

ParserConfig::ParserConfig()
	:
		backendType(),
		backendCreator(nullptr),
		comment(false),
		arrayType(),
		objectType()
{
	setBackendType<ParserBackendType::JSONPP_DEFAULT_PARSER_BACKEND>();
}

ParserConfig::~ParserConfig()
{
}

ParserSource::ParserSource()
	:
		prepared(false),
		storageType(StorageType::string),
		cstr(nullptr),
		cstrLength(0),
		ref(nullptr),
		str()
{
}

ParserSource::ParserSource(const char * cstr, const std::size_t cstrLength)
	:
		prepared(false),
		storageType(StorageType::cstr),
		cstr(cstr),
		cstrLength(cstrLength),
		ref(nullptr),
		str()
{
}

ParserSource::ParserSource(const std::string & str)
	:
		prepared(false),
		storageType(StorageType::ref),
		cstr(nullptr),
		cstrLength(0),
		ref(&str),
		str()
{
}

ParserSource::ParserSource(std::string && str)
	:
		prepared(false),
		storageType(StorageType::string),
		cstr(nullptr),
		cstrLength(0),
		ref(nullptr),
		str(std::move(str))
{
}

const char * ParserSource::getText() const
{
	switch(storageType) {
	case StorageType::cstr:
		return cstr;

	case StorageType::ref:
		return ref->c_str();

	default:
		return str.c_str();
	}
}

std::size_t ParserSource::getTextLength() const
{
	switch(storageType) {
	case StorageType::cstr:
		return cstrLength;

	case StorageType::ref:
		return ref->size();

	default:
		return str.size();
	}
}

std::size_t ParserSource::getCapacity() const
{
	switch(storageType) {
	case StorageType::cstr:
		return cstrLength;

	case StorageType::ref:
		return ref->capacity();

	default:
		return str.capacity();
	}
}

void ParserSource::pad(const std::size_t size) const
{
	switch(storageType) {
	case StorageType::cstr: {
		str.resize(cstrLength);
		str.reserve(cstrLength + size);
		memmove(&str[0], cstr, cstrLength);
		str[cstrLength] = 0;
		cstr = nullptr;
		storageType = StorageType::string;
		break;
	}

	case StorageType::ref: {
		if(ref->capacity() < ref->size() + size) {
			str.resize(ref->size());
			str.reserve(ref->size() + size);
			memmove(&str[0], &ref->at(0), ref->size());
			str[ref->size()] = 0;
			ref = nullptr;
			storageType = StorageType::string;
		}
		break;
	}

	default: {
		if(str.capacity() < str.size() + size) {
			str.reserve(str.size() + size);
		}
		break;
	}
	}
}

Parser::Parser()
	: Parser(ParserConfig())
{
}

Parser::Parser(const ParserConfig & config)
	: backend(config.getBackendCreator()(config)), errorMessage()
{
}

Parser::~Parser()
{
}

bool Parser::hasError() const
{
	return ! errorMessage.empty();
}

const std::string & Parser::getError() const
{
	return errorMessage;
}

metapp::Variant Parser::parse(const char * jsonText, const std::size_t length, const metapp::MetaType * proto)
{
	return parse(ParserSource(jsonText, length), proto);
}

metapp::Variant Parser::parse(const std::string & jsonText, const metapp::MetaType * proto)
{
	return parse(ParserSource(jsonText), proto);
}

metapp::Variant Parser::parse(const ParserSource & source, const metapp::MetaType * proto)
{
	errorMessage.clear();

	if(! source.hasPrepared()) {
		source.setAsPrepared();
		backend->prepareSource(source);
	}

	try {
		ParserBackendResult result = backend->parse(source, proto);
		errorMessage = std::move(result.errorMessage);
		return result.value;
	}
	catch(const metapp::MetaException & e) {
		errorMessage = e.what();
	}
	catch(const std::exception & e) {
		errorMessage = e.what();
	}
	return metapp::Variant();
}


} // namespace jsonpp

