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
#include "metapp/interfaces/metaindexable.h"
#include "metapp/allmetatypes.h"

#include <deque>
#include <unordered_map>
#include <iostream>

TEMPLATE_LIST_TEST_CASE("Parse, array", "", BackendTypes)
{
	constexpr auto backendType = TestType::backendType;
	jsonpp::Parser parser(jsonpp::ParserConfig().setBackendType<backendType>());
	const std::string jsonText = R"(
		[ 5, "abc" ]
	)";
	metapp::Variant var = parser.parse(jsonText);
	REQUIRE(jsonpp::getJsonType(var) == jsonpp::JsonType::jtArray);
	const jsonpp::JsonArray & array = var.get<const jsonpp::JsonArray &>();
	REQUIRE(array[0].get<jsonpp::JsonInt>() == 5);
	REQUIRE(array[1].get<std::string &>() == "abc");
}

TEMPLATE_LIST_TEST_CASE("Parse, array, proto", "", BackendTypes)
{
	constexpr auto backendType = TestType::backendType;
	jsonpp::Parser parser(jsonpp::ParserConfig().setBackendType<backendType>());

	SECTION("std::deque<long>") {
		const std::string jsonText = R"(
			[ 5, 6.1 ]
		)";
		metapp::Variant var = parser.parse(jsonText, metapp::getMetaType<std::deque<long> >());
		REQUIRE(jsonpp::getJsonType(var) == jsonpp::JsonType::jtNone);
		const auto & array = var.get<const std::deque<long> &>();
		REQUIRE(var.getMetaType()->getTypeKind() == metapp::tkStdDeque);
		REQUIRE(array[0] == 5);
		REQUIRE(array[1] == 6);
	}

	SECTION("std::list<metapp::Variant>") {
		const std::string jsonText = R"(
			[ 5, "abc" ]
		)";
		metapp::Variant var = parser.parse(jsonText, metapp::getMetaType<std::list<metapp::Variant> >());
		REQUIRE(jsonpp::getJsonType(var) == jsonpp::JsonType::jtNone);
		const auto & array = var.get<const std::list<metapp::Variant> &>();
		auto it = array.begin();
		REQUIRE(it->get<jsonpp::JsonInt>() == 5);
		++it;
		REQUIRE(it->get<const jsonpp::JsonString &>() == "abc");
	}
}

TEMPLATE_LIST_TEST_CASE("Parse, object", "", BackendTypes)
{
	constexpr auto backendType = TestType::backendType;
	jsonpp::Parser parser(jsonpp::ParserConfig().setBackendType<backendType>());
	const std::string jsonText = R"(
		{ "b" : 5, "a" : "hello" }
	)";
	metapp::Variant var = parser.parse(jsonText);
	REQUIRE(jsonpp::getJsonType(var) == jsonpp::JsonType::jtObject);
	const jsonpp::JsonObject & object = var.get<const jsonpp::JsonObject &>();
	REQUIRE(object.at("a").get<std::string &>() == "hello");
	REQUIRE(object.at("b").get<jsonpp::JsonInt>() == 5);
}

TEMPLATE_LIST_TEST_CASE("Parse, object, proto", "", BackendTypes)
{
	constexpr auto backendType = TestType::backendType;
	jsonpp::Parser parser(jsonpp::ParserConfig().setBackendType<backendType>());
	using Proto = std::unordered_map<std::string, metapp::Variant>;
	const std::string jsonText = R"(
		{ "b" : 5, "a" : "hello" }
	)";
	metapp::Variant var = parser.parse(jsonText, metapp::getMetaType<Proto>());
	REQUIRE(jsonpp::getJsonType(var) == jsonpp::JsonType::jtNone);
	const auto & object = var.get<const Proto &>();
	REQUIRE(object.at("a").get<std::string &>() == "hello");
	REQUIRE(object.at("b").get<jsonpp::JsonInt>() == 5);
}

TEMPLATE_LIST_TEST_CASE("Parse, array in object", "", BackendTypes)
{
	constexpr auto backendType = TestType::backendType;
	jsonpp::Parser parser(jsonpp::ParserConfig().setBackendType<backendType>());
	const std::string jsonText = R"(
		{ "b" : 5, "a" : [ "hello", 38 ] }
	)";
	metapp::Variant var = parser.parse(jsonText);
	const jsonpp::JsonObject & object = var.get<const jsonpp::JsonObject &>();
	REQUIRE(object.at("b").get<jsonpp::JsonInt>() == 5);
	const jsonpp::JsonArray & array = object.at("a").get<const jsonpp::JsonArray &>();
	REQUIRE(array[0].get<std::string &>() == "hello");
	REQUIRE(array[1].cast<int>().template get<int>() == 38);
}

TEMPLATE_LIST_TEST_CASE("Parse, object in array", "", BackendTypes)
{
	constexpr auto backendType = TestType::backendType;
	jsonpp::Parser parser(jsonpp::ParserConfig().setBackendType<backendType>());
	const std::string jsonText = R"(
		[ 5, { "b" : 38, "a" : "hello" } ]
	)";
	metapp::Variant var = parser.parse(jsonText);
	const jsonpp::JsonArray & array = var.get<const jsonpp::JsonArray &>();
	REQUIRE(array[0].get<jsonpp::JsonInt>() == 5);
	const jsonpp::JsonObject & object = array[1].get<const jsonpp::JsonObject &>();
	REQUIRE(object.at("a").get<std::string &>() == "hello");
	REQUIRE(object.at("b").get<jsonpp::JsonInt>() == 38);
}

TEMPLATE_LIST_TEST_CASE("Parse, object, parse as array", "", BackendTypes)
{
	constexpr auto backendType = TestType::backendType;
	jsonpp::Parser parser(jsonpp::ParserConfig().setBackendType<backendType>());
	const std::string jsonText = R"(
		{ "one" : 1, "two" : 2.1 }
	)";
	using Proto = std::vector<std::pair<std::string, int> >;
	metapp::Variant var = parser.parse(jsonText, metapp::getMetaType<Proto>());
	const Proto & array = var.get<const Proto &>();
	REQUIRE(array[0].first == "one");
	REQUIRE(array[0].second == 1);
	REQUIRE(array[1].first == "two");
	REQUIRE(array[1].second == 2);
}

