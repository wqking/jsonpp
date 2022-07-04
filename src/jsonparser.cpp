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
#include "parser.h"

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
std::unique_ptr<ParserBackend> createBackend_simdjson(const ParserConfig & config);

std::unique_ptr<ParserBackend> createBackend(const ParserConfig & config, const ParserType parserType)
{
#if JSONPP_BACKEND_CPARSER
	if(parserType == ParserType::cparser) {
		return createBackend_cparser(config);
	}
#endif

#if JSONPP_BACKEND_SIMDJSON
	if(parserType == ParserType::simdjson) {
		return createBackend_simdjson(config);
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

metapp::Variant JsonParser::parse(const char * jsonText, const size_t length, const metapp::MetaType * proto)
{
	return backend->parse(jsonText, length, proto);
}

metapp::Variant JsonParser::parse(const std::string & jsonText, const metapp::MetaType * proto)
{
	return parse(jsonText.c_str(), jsonText.size(), proto);
}


} // namespace jsonpp

