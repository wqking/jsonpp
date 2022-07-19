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

#include "test_parser.h"
#include "classes.h"

#include "jsonpp/dumper.h"
#include "jsonpp/parser.h"
#include "metapp/interfaces/metaindexable.h"
#include "metapp/allmetatypes.h"

#include <deque>
#include <unordered_map>
#include <iostream>

TEMPLATE_LIST_TEST_CASE("DumpAndParse, null", "", BackendTypes)
{
	constexpr auto backendType = TestType::backendType;
	auto dumperConfig = DUMPER_CONFIGS();
	const std::string jsonText = jsonpp::Dumper(dumperConfig).dump(nullptr);
	metapp::Variant var = jsonpp::Parser(jsonpp::ParserConfig().setBackendType<backendType>()).parse(jsonText);
	REQUIRE(var.get<jsonpp::JsonNull>() == nullptr);
}

TEMPLATE_LIST_TEST_CASE("DumpAndParse, bool", "", BackendTypes)
{
	constexpr auto backendType = TestType::backendType;
	auto dumperConfig = DUMPER_CONFIGS();
	SECTION("true") {
		const std::string jsonText = jsonpp::Dumper(dumperConfig).dump(true);
		metapp::Variant var = jsonpp::Parser(jsonpp::ParserConfig().setBackendType<backendType>()).parse(jsonText);
		REQUIRE(var.get<jsonpp::JsonBool>());
	}

	SECTION("false") {
		const std::string jsonText = jsonpp::Dumper(dumperConfig).dump(false);
		metapp::Variant var = jsonpp::Parser(jsonpp::ParserConfig().setBackendType<backendType>()).parse(jsonText);
		REQUIRE(! var.get<jsonpp::JsonBool>());
	}
}

TEMPLATE_LIST_TEST_CASE("DumpAndParse, int", "", BackendTypes)
{
	constexpr auto backendType = TestType::backendType;
	auto dumperConfig = DUMPER_CONFIGS();
	SECTION("38") {
		const std::string jsonText = jsonpp::Dumper(dumperConfig).dump(38);
		metapp::Variant var = jsonpp::Parser(jsonpp::ParserConfig().setBackendType<backendType>()).parse(jsonText);
		REQUIRE(var.get<jsonpp::JsonInt>() == 38);
	}
	SECTION("-98765") {
		const std::string jsonText = jsonpp::Dumper(dumperConfig).dump(-98765);
		metapp::Variant var = jsonpp::Parser(jsonpp::ParserConfig().setBackendType<backendType>()).parse(jsonText);
		REQUIRE(var.get<jsonpp::JsonInt>() == -98765);
	}
}

TEST_CASE("DumpAndParse, unsigned int")
{
	constexpr auto backendType = jsonpp::ParserBackendType::simdjson;
	auto dumperConfig = DUMPER_CONFIGS();
	// 18446744073709551615 is max of uint64_t
	SECTION("18446744073709551615") {
		const std::string jsonText = jsonpp::Dumper(dumperConfig).dump(18446744073709551615ULL);
		metapp::Variant var = jsonpp::Parser(jsonpp::ParserConfig().setBackendType<backendType>()).parse(jsonText);
		REQUIRE(jsonpp::getJsonType(var) == jsonpp::JsonType::jtUnsignedInt);
		REQUIRE(var.get<jsonpp::JsonUnsignedInt>() == 18446744073709551615ULL);
	}
}

TEMPLATE_LIST_TEST_CASE("DumpAndParse, double", "", BackendTypes)
{
	constexpr auto backendType = TestType::backendType;
	auto dumperConfig = DUMPER_CONFIGS();
	SECTION("9.1") {
		const std::string jsonText = jsonpp::Dumper(dumperConfig).dump(9.1);
		metapp::Variant var = jsonpp::Parser(jsonpp::ParserConfig().setBackendType<backendType>()).parse(jsonText);
		REQUIRE(var.get<jsonpp::JsonReal>() == 9.1);
	}
	SECTION("-3.14") {
		const std::string jsonText = jsonpp::Dumper(dumperConfig).dump(-3.14);
		metapp::Variant var = jsonpp::Parser(jsonpp::ParserConfig().setBackendType<backendType>()).parse(jsonText);
		REQUIRE(var.get<jsonpp::JsonReal>() == -3.14);
	}
}

TEMPLATE_LIST_TEST_CASE("DumpAndParse, string", "", BackendTypes)
{
	constexpr auto backendType = TestType::backendType;
	auto dumperConfig = DUMPER_CONFIGS();
	const std::string jsonText = jsonpp::Dumper(dumperConfig).dump("Hello world");
	metapp::Variant var = jsonpp::Parser(jsonpp::ParserConfig().setBackendType<backendType>()).parse(jsonText);
	REQUIRE(var.get<const std::string &>() == "Hello world");
}

TEMPLATE_LIST_TEST_CASE("DumpAndParse, string, escape/unescape", "", BackendTypes)
{
	constexpr auto backendType = TestType::backendType;
	auto dumperConfig = DUMPER_CONFIGS();
	const std::string text = ""
		"one"
		"\x01\x02\x03\x04\x05\x06\x07\x08"
		"two"
		"\x09\x0a\x0b\x0c\x0d\x0e\x0f\x10"
		"three"
		"\x11\x12\x13\x14\x15\x16\x17\x18"
		"four"
		"\x19\x1a\x1b\x1c\x1d\x1e\x1f\"\\"
		"five"
	;
	const std::string jsonText = jsonpp::Dumper(dumperConfig).dump(text);
	metapp::Variant var = jsonpp::Parser(jsonpp::ParserConfig().setBackendType<backendType>()).parse(jsonText);
	REQUIRE(var.get<const std::string &>() == text);
}

