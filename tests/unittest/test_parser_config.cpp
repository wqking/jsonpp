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

TEST_CASE("ParserConfig, comment")
{
	constexpr auto backendType = jsonpp::ParserBackendType::cparser;
	auto dumperConfig = DUMPER_CONFIGS();
	jsonpp::ParserConfig parserConfig;
	parserConfig.setBackendType<backendType>();
	parserConfig.enableComment(true);
	const std::string jsonText = R"( [
		5 /* comment */, //nothing
		6 // what
	]
	)";
	const auto array = jsonpp::Parser(parserConfig).parse<std::vector<int> >(jsonText);
	REQUIRE(array[0] == 5);
	REQUIRE(array[1] == 6);
}

TEMPLATE_LIST_TEST_CASE("ParserConfig, setArrayType", "", BackendTypes)
{
	constexpr auto backendType = TestType::backendType;
	auto dumperConfig = DUMPER_CONFIGS();
	jsonpp::ParserConfig parserConfig;
	parserConfig.setBackendType<backendType>();

	SECTION("top level array") {
		using T = std::list<int>;
		parserConfig.setArrayType<T>();
		const std::string jsonText = R"([ 5, -7, 6 ])";
		const metapp::Variant var = jsonpp::Parser(parserConfig).parse(jsonText);
		REQUIRE(var.getMetaType()->equal(metapp::getMetaType<T>()));
		const auto & array = var.get<const T &>();
		auto it = array.begin();
		REQUIRE(*it == 5);
		++it;
		REQUIRE(*it == -7);
		++it;
		REQUIRE(*it == 6);
	}
	SECTION("array in object") {
		using T = std::deque<long>;
		parserConfig.setArrayType<T>();
		const std::string jsonText = R"({ "a" : 1, "b" : [ 5, -7, 6 ] })";
		const metapp::Variant var = jsonpp::Parser(parserConfig).parse(jsonText);
		const metapp::Variant & nestedArray = var.get<const jsonpp::JsonObject &>().at("b");
		REQUIRE(nestedArray.getMetaType()->equal(metapp::getMetaType<T>()));
		const auto & array = nestedArray.get<const T &>();
		REQUIRE(array[0] == 5);
		REQUIRE(array[1] == -7);
		REQUIRE(array[2] == 6);
	}
	SECTION("prototype suppresses array type") {
		parserConfig.setArrayType<std::list<int> >();
		const std::string jsonText = R"([ 5, -7, 6 ])";
		const auto array = jsonpp::Parser(parserConfig).parse<std::vector<int> >(jsonText);
		REQUIRE(array[0] == 5);
		REQUIRE(array[1] == -7);
		REQUIRE(array[2] == 6);
	}
}

TEMPLATE_LIST_TEST_CASE("ParserConfig, setObjectType", "", BackendTypes)
{
	constexpr auto backendType = TestType::backendType;
	auto dumperConfig = DUMPER_CONFIGS();
	jsonpp::ParserConfig parserConfig;
	parserConfig.setBackendType<backendType>();

	SECTION("top level object") {
		using T = std::map<std::string, long>;
		parserConfig.setObjectType<T>();
		const std::string jsonText = R"({ "a" : 7, "b" : -6, "c" : 5 })";
		const metapp::Variant var = jsonpp::Parser(parserConfig).parse(jsonText);
		REQUIRE(var.getMetaType()->equal(metapp::getMetaType<T>()));
		const auto & object = var.get<const T &>();
		REQUIRE(object.at("a") == 7);
		REQUIRE(object.at("b") == -6);
		REQUIRE(object.at("c") == 5);
	}
	SECTION("object in array") {
		using T = std::unordered_map<std::string, int>;
		parserConfig.setObjectType<T>();
		const std::string jsonText = R"([ 1, { "a" : 7, "b" : -6, "c" : 5 } ])";
		const metapp::Variant var = jsonpp::Parser(parserConfig).parse(jsonText);
		const metapp::Variant & nestedObject = var.get<const jsonpp::JsonArray &>()[1];
		REQUIRE(nestedObject.getMetaType()->equal(metapp::getMetaType<T>()));
		const auto & object = nestedObject.get<const T &>();
		REQUIRE(object.at("a") == 7);
		REQUIRE(object.at("b") == -6);
		REQUIRE(object.at("c") == 5);
	}
	SECTION("std::vector<std::pair<std::string, int> >") {
		using T = std::vector<std::pair<std::string, int> >;
		parserConfig.setObjectType<T>();
		const std::string jsonText = R"({ "a" : 7, "b" : -6, "c" : 5 })";
		const metapp::Variant var = jsonpp::Parser(parserConfig).parse(jsonText);
		REQUIRE(var.getMetaType()->equal(metapp::getMetaType<T>()));
		const auto & object = var.get<const T &>();
		REQUIRE(object[0].first == "a");
		REQUIRE(object[0].second == 7);
		REQUIRE(object[1].first == "b");
		REQUIRE(object[1].second == -6);
		REQUIRE(object[2].first == "c");
		REQUIRE(object[2].second == 5);
	}
	SECTION("std::vector<std::array<std::string, 5> >") {
		using T = std::vector<std::array<std::string, 5> >;
		parserConfig.setObjectType<T>();
		const std::string jsonText = R"({ "a" : "7", "b" : "-6", "c" : "5" })";
		const metapp::Variant var = jsonpp::Parser(parserConfig).parse(jsonText);
		REQUIRE(var.getMetaType()->equal(metapp::getMetaType<T>()));
		const auto & object = var.get<const T &>();
		REQUIRE(object[0][0] == "a");
		REQUIRE(object[0][1] == "7");
		REQUIRE(object[1][0] == "b");
		REQUIRE(object[1][1] == "-6");
		REQUIRE(object[2][0] == "c");
		REQUIRE(object[2][1] == "5");
	}
	SECTION("prototype suppresses object type") {
		parserConfig.setObjectType<std::map<std::string, long> >();
		const std::string jsonText = R"({ "a" : 7, "b" : -6, "c" : 5 })";
		const auto object = jsonpp::Parser(parserConfig).parse<std::unordered_map<std::string, int> >(jsonText);
		REQUIRE(object.at("a") == 7);
		REQUIRE(object.at("b") == -6);
		REQUIRE(object.at("c") == 5);
	}
}

