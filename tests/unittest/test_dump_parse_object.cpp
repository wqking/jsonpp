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

TEMPLATE_LIST_TEST_CASE("DumpAndParse, object, std::map<std::string, metapp::Variant>", "", BackendTypes)
{
	constexpr auto backendType = TestType::backendType;
	auto dumperConfig = DUMPER_CONFIGS();
	const std::string jsonText = jsonpp::Dumper(dumperConfig).dump(std::map<std::string, metapp::Variant> {
		{ "one", 1 },
		{ "two", std::vector<int> { 2, 3 } }
	});
	jsonpp::ParserConfig parserConfig;
	parserConfig.setBackendType<backendType>();

	SECTION("parse as default") {
		const metapp::Variant var = jsonpp::Parser(parserConfig).parse(jsonText);
		const jsonpp::JsonObject & object = var.get<const jsonpp::JsonObject &>();
		REQUIRE(object.at("one").get<jsonpp::JsonInt>() == 1);
		const jsonpp::JsonArray & two = object.at("two").get<const jsonpp::JsonArray &>();
		REQUIRE(two[0].get<jsonpp::JsonInt>() == 2);
		REQUIRE(two[1].get<jsonpp::JsonInt>() == 3);
	}
	SECTION("parse as prototype") {
		using T = std::map<std::string, metapp::Variant>;
		const metapp::Variant var = jsonpp::Parser(parserConfig).parse(jsonText, metapp::getMetaType<T>());
		const T & object = var.get<const T &>();
		REQUIRE(object.at("one").get<jsonpp::JsonInt>() == 1);
		const jsonpp::JsonArray & two = object.at("two").get<const jsonpp::JsonArray &>();
		REQUIRE(two[0].get<jsonpp::JsonInt>() == 2);
		REQUIRE(two[1].get<jsonpp::JsonInt>() == 3);
	}
	SECTION("parse as template") {
		using T = std::map<std::string, metapp::Variant>;
		const T object = jsonpp::Parser(parserConfig).parse<T>(jsonText);
		REQUIRE(object.at("one").get<jsonpp::JsonInt>() == 1);
		const jsonpp::JsonArray & two = object.at("two").get<const jsonpp::JsonArray &>();
		REQUIRE(two[0].get<jsonpp::JsonInt>() == 2);
		REQUIRE(two[1].get<jsonpp::JsonInt>() == 3);
	}
}

TEMPLATE_LIST_TEST_CASE("DumpAndParse, object, std::map<std::string, int>", "", BackendTypes)
{
	constexpr auto backendType = TestType::backendType;
	auto dumperConfig = DUMPER_CONFIGS();
	const std::string jsonText = jsonpp::Dumper(dumperConfig).dump(std::map<std::string, int> {
		{ "one", 1 },
		{ "two", 2 }
	});
	jsonpp::ParserConfig parserConfig;
	parserConfig.setBackendType<backendType>();

	SECTION("parse as default") {
		const metapp::Variant var = jsonpp::Parser(parserConfig).parse(jsonText);
		const jsonpp::JsonObject & object = var.get<const jsonpp::JsonObject &>();
		REQUIRE(object.at("one").get<jsonpp::JsonInt>() == 1);
		REQUIRE(object.at("two").get<jsonpp::JsonInt>() == 2);
	}
	SECTION("parse as prototype") {
		using T = std::map<std::string, long>;
		const metapp::Variant var = jsonpp::Parser(parserConfig).parse(jsonText, metapp::getMetaType<T>());
		const T & object = var.get<const T &>();
		REQUIRE(object.at("one") == 1);
		REQUIRE(object.at("two") == 2);
	}
	SECTION("parse as template") {
		using T = std::map<std::string, char>;
		const T object = jsonpp::Parser(parserConfig).parse<T>(jsonText);
		REQUIRE(object.at("one") == 1);
		REQUIRE(object.at("two") == 2);
	}
}

