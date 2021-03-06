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

#ifndef TEST_PARSER_H
#define TEST_PARSER_H

#include "test.h"

#include "jsonpp/parser.h"

#define PARSER_TYPES() GENERATE(jsonpp::ParserBackendType::cparser, jsonpp::ParserBackendType::simdjson)

template <jsonpp::ParserBackendType type>
struct TestBackendType
{
	static constexpr jsonpp::ParserBackendType backendType = type;
};

using BackendTypes = std::tuple<
	TestBackendType<jsonpp::ParserBackendType::simdjson>,
	TestBackendType<jsonpp::ParserBackendType::cparser>
>;

#define DUMPER_CONFIGS() GENERATE( \
	jsonpp::DumperConfig(jsonpp::DumperConfig().enableBeautify(false).enableNamedEnum(false)), \
	jsonpp::DumperConfig(jsonpp::DumperConfig().enableBeautify(true).enableNamedEnum(false)), \
	jsonpp::DumperConfig(jsonpp::DumperConfig().enableBeautify(false).enableNamedEnum(true)), \
	jsonpp::DumperConfig(jsonpp::DumperConfig().enableBeautify(true).enableNamedEnum(true)) \
)

#endif
