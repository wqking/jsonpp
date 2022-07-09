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

TEST_CASE("JsonParser, error")
{
	auto parserType = PARSER_TYPES();
	std::string jsonText = R"(
		5, 6
	)";
	jsonpp::JsonParser parser(parserType);
	metapp::Variant var = parser.parse(jsonText);
	REQUIRE(var.isEmpty());
	REQUIRE(parser.hasError());
}

TEST_CASE("JsonParser, null")
{
	auto parserType = PARSER_TYPES();
	jsonpp::JsonParser parser(parserType);
	std::string jsonText = R"(
		null
	)";
	metapp::Variant var = parser.parse(jsonText);
	REQUIRE(jsonpp::getJsonType(var) == jsonpp::JsonType::jtNull);
	REQUIRE(var.get<jsonpp::JsonNull>() == nullptr);
}

TEST_CASE("JsonParser, bool")
{
	auto parserType = PARSER_TYPES();
	jsonpp::JsonParser parser(parserType);
	SECTION("true") {
		std::string jsonText = R"(
			true
		)";
		metapp::Variant var = parser.parse(jsonText);
		REQUIRE(jsonpp::getJsonType(var) == jsonpp::JsonType::jtBool);
		REQUIRE(var.get<jsonpp::JsonBool>() == true);
	}
	SECTION("false") {
		std::string jsonText = R"(
			false
		)";
		metapp::Variant var = parser.parse(jsonText);
		REQUIRE(jsonpp::getJsonType(var) == jsonpp::JsonType::jtBool);
		REQUIRE(var.get<jsonpp::JsonBool>() == false);
	}
}

TEST_CASE("JsonParser, int")
{
	auto parserType = PARSER_TYPES();
	jsonpp::JsonParser parser(parserType);
	std::string jsonText = R"(
		5
	)";
	metapp::Variant var = parser.parse(jsonText);
	REQUIRE(jsonpp::getJsonType(var) == jsonpp::JsonType::jtInt);
	REQUIRE(var.get<jsonpp::JsonInt>() == 5);
}

TEST_CASE("JsonParser, string")
{
	auto parserType = PARSER_TYPES();
	jsonpp::JsonParser parser(parserType);
	std::string jsonText = R"(
		"abc"
	)";
	metapp::Variant var = parser.parse(jsonText);
	REQUIRE(var.get<std::string &>() == "abc");
}

TEST_CASE("JsonParser, array")
{
	auto parserType = PARSER_TYPES();
	jsonpp::JsonParser parser(parserType);
	std::string jsonText = R"(
		[ 5, "abc" ]
	)";
	metapp::Variant var = parser.parse(jsonText);
	const jsonpp::JsonArray & array = var.get<const jsonpp::JsonArray &>();
	REQUIRE(array[0].get<jsonpp::JsonInt>() == 5);
	REQUIRE(array[1].get<std::string &>() == "abc");
}

TEST_CASE("JsonParser, array, proto")
{
	auto parserType = PARSER_TYPES();
	jsonpp::JsonParser parser(parserType);
	std::string jsonText = R"(
		[ 5, 6 ]
	)";
	metapp::Variant var = parser.parse(jsonText, metapp::getMetaType<std::deque<long> >());
	const std::deque<long> & array = var.get<const std::deque<long> &>();
	REQUIRE(array[0] == 5);
	REQUIRE(array[1] == 6);
	REQUIRE(var.getMetaType()->getTypeKind() == metapp::tkStdDeque);
}

TEST_CASE("JsonParser, object")
{
	auto parserType = PARSER_TYPES();
	jsonpp::JsonParser parser(parserType);
	std::string jsonText = R"(
		{ "b" : 5, "a" : "hello" }
	)";
	metapp::Variant var = parser.parse(jsonText);
	const jsonpp::JsonObject & object = var.get<const jsonpp::JsonObject &>();
	REQUIRE(object.at("a").get<std::string &>() == "hello");
	REQUIRE(object.at("b").get<jsonpp::JsonInt>() == 5);
}

TEST_CASE("JsonParser, array in object")
{
	auto parserType = PARSER_TYPES();
	jsonpp::JsonParser parser(parserType);
	std::string jsonText = R"(
		{ "b" : 5, "a" : [ "hello", 38 ] }
	)";
	metapp::Variant var = parser.parse(jsonText);
	const jsonpp::JsonObject & object = var.get<const jsonpp::JsonObject &>();
	REQUIRE(object.at("b").get<jsonpp::JsonInt>() == 5);
	const jsonpp::JsonArray & array = object.at("a").get<const jsonpp::JsonArray &>();
	REQUIRE(array[0].get<std::string &>() == "hello");
	REQUIRE(array[1].cast<int>().template get<int>() == 38);
}

TEST_CASE("JsonParser, object in array")
{
	auto parserType = PARSER_TYPES();
	jsonpp::JsonParser parser(parserType);
	std::string jsonText = R"(
		[ 5, { "b" : 38, "a" : "hello" } ]
	)";
	metapp::Variant var = parser.parse(jsonText);
	const jsonpp::JsonArray & array = var.get<const jsonpp::JsonArray &>();
	REQUIRE(array[0].get<jsonpp::JsonInt>() == 5);
	const jsonpp::JsonObject & object = array[1].get<const jsonpp::JsonObject &>();
	REQUIRE(object.at("a").get<std::string &>() == "hello");
	REQUIRE(object.at("b").get<jsonpp::JsonInt>() == 38);
}

TEST_CASE("JsonParser, object, parse as array")
{
	auto parserType = PARSER_TYPES();
	jsonpp::JsonParser parser(parserType);
	std::string jsonText = R"(
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

