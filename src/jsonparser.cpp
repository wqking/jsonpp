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

#include "jsonpp/jsonparser.h"
#include "jsonpp/parserbackend.h"

#include "metapp/allmetatypes.h"
#include "metapp/interfaces/metaclass.h"
#include "metapp/interfaces/metaindexable.h"
#include "metapp/interfaces/metamappable.h"
#include "metapp/compiler.h"

#include <array>
#include <limits>

namespace jsonpp {

namespace internal_ {

std::unique_ptr<ParserBackend> createBackend_cparser(const ParserConfig & config);
std::unique_ptr<ParserBackend> createBackend_simdjsonDom(const ParserConfig & config);

std::unique_ptr<ParserBackend> createBackend(const ParserConfig & config, const ParserType parserType)
{
#if JSONPP_BACKEND_CPARSER
	if(parserType == ParserType::cparser) {
		return createBackend_cparser(config);
	}
#endif

#if JSONPP_BACKEND_SIMDJSON
	if(parserType == ParserType::simdjson) {
		return createBackend_simdjsonDom(config);
	}
#endif

	return std::unique_ptr<ParserBackend>();
}

} // namespace internal_


std::string getParserTypeName(const ParserType type)
{
	switch(type) {
	case ParserType::cparser:
		return "json-parser";

	case ParserType::simdjson:
		return "simdjson";
	}

	return "Unknown";
}

JsonParserSource::JsonParserSource()
	:
		prepared(false),
		storageType(StorageType::string),
		cstr(nullptr),
		cstrLength(0),
		ref(nullptr),
		str()
{
}

JsonParserSource::JsonParserSource(const char * cstr, const std::size_t cstrLength)
	:
		prepared(false),
		storageType(StorageType::cstr),
		cstr(cstr),
		cstrLength(cstrLength),
		ref(nullptr),
		str()
{
}

JsonParserSource::JsonParserSource(const std::string & str)
	:
		prepared(false),
		storageType(StorageType::ref),
		cstr(nullptr),
		cstrLength(0),
		ref(&str),
		str()
{
}

JsonParserSource::JsonParserSource(std::string && str)
	:
		prepared(false),
		storageType(StorageType::string),
		cstr(nullptr),
		cstrLength(0),
		ref(nullptr),
		str(std::move(str))
{
}

const char * JsonParserSource::getText() const
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

std::size_t JsonParserSource::getTextLength() const
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

std::size_t JsonParserSource::getCapacity() const
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

void JsonParserSource::pad(const std::size_t size) const
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

JsonParser::JsonParser()
	: JsonParser(ParserConfig(), ParserType::simdjson)
{
}

JsonParser::JsonParser(const ParserType parserType)
	: JsonParser(ParserConfig(), parserType)
{
}

JsonParser::JsonParser(const ParserConfig & config, const ParserType parserType)
	: backend(internal_::createBackend(config, parserType))
{
}

JsonParser::~JsonParser()
{
}

bool JsonParser::hasError() const
{
	return backend->hasError();
}

std::string JsonParser::getError() const
{
	return backend->getError();
}

metapp::Variant JsonParser::parse(const char * jsonText, const std::size_t length, const metapp::MetaType * proto)
{
	return parse(JsonParserSource(jsonText, length), proto);
}

metapp::Variant JsonParser::parse(const std::string & jsonText, const metapp::MetaType * proto)
{
	return parse(JsonParserSource(jsonText), proto);
}

metapp::Variant JsonParser::parse(const JsonParserSource & source, const metapp::MetaType * proto)
{
	if(! source.hasPrepared()) {
		source.setAsPrepared();
		backend->prepareSource(source);
	}

	return backend->parse(source, proto);
}


} // namespace jsonpp

