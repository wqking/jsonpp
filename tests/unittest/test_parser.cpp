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
	std::string jsonText = R"(
		5, 6
	)";
	auto parserType = PARSER_TYPES();
	jsonpp::JsonParser parser(parserType);
	metapp::Variant var = parser.parse(jsonText.c_str(), jsonText.size());
	REQUIRE(var.isEmpty());
	REQUIRE(parser.hasError());
}

TEST_CASE("JsonParser, int")
{
	std::string jsonText = R"(
		5
	)";
	auto parserType = PARSER_TYPES();
	jsonpp::JsonParser parser(parserType);
	metapp::Variant var = parser.parse(jsonText.c_str(), jsonText.size());
	REQUIRE(var.get<jsonpp::JsonInt>() == 5);
}

TEST_CASE("JsonParser, string")
{
	std::string jsonText = R"(
		"abc"
	)";
	auto parserType = PARSER_TYPES();
	jsonpp::JsonParser parser(parserType);
	metapp::Variant var = parser.parse(jsonText.c_str(), jsonText.size());
	REQUIRE(var.get<std::string &>() == "abc");
}

TEST_CASE("JsonParser, array")
{
	std::string jsonText = R"(
		[ 5, "abc" ]
	)";
	auto parserType = PARSER_TYPES();
	jsonpp::JsonParser parser(parserType);
	metapp::Variant var = parser.parse(jsonText.c_str(), jsonText.size());
	const jsonpp::JsonArray & array = var.get<const jsonpp::JsonArray &>();
	REQUIRE(array[0].get<jsonpp::JsonInt>() == 5);
	REQUIRE(array[1].get<std::string &>() == "abc");
}

TEST_CASE("JsonParser, array, proto")
{
	std::string jsonText = R"(
		[ 5, 6 ]
	)";
	auto parserType = PARSER_TYPES();
	jsonpp::JsonParser parser(parserType);
	metapp::Variant var = parser.parse(jsonText.c_str(), jsonText.size(), metapp::getMetaType<std::deque<long> >());
	const std::deque<long> & array = var.get<const std::deque<long> &>();
	REQUIRE(array[0] == 5);
	REQUIRE(array[1] == 6);
	REQUIRE(var.getMetaType()->getTypeKind() == metapp::tkStdDeque);
}

TEST_CASE("JsonParser, object")
{
	std::string jsonText = R"(
		{ "b" : 5, "a" : "hello" }
	)";
	auto parserType = PARSER_TYPES();
	jsonpp::JsonParser parser(parserType);
	metapp::Variant var = parser.parse(jsonText.c_str(), jsonText.size());
	const jsonpp::JsonObject & object = var.get<const jsonpp::JsonObject &>();
	REQUIRE(object.at("a").get<std::string &>() == "hello");
	REQUIRE(object.at("b").get<jsonpp::JsonInt>() == 5);
}

TEST_CASE("JsonParser, array in object")
{
	std::string jsonText = R"(
		{ "b" : 5, "a" : [ "hello", 38 ] }
	)";
	auto parserType = PARSER_TYPES();
	jsonpp::JsonParser parser(parserType);
	metapp::Variant var = parser.parse(jsonText.c_str(), jsonText.size());
	const jsonpp::JsonObject & object = var.get<const jsonpp::JsonObject &>();
	REQUIRE(object.at("b").get<jsonpp::JsonInt>() == 5);
	const jsonpp::JsonArray & array = object.at("a").get<const jsonpp::JsonArray &>();
	REQUIRE(array[0].get<std::string &>() == "hello");
	REQUIRE(array[1].cast<int>().template get<int>() == 38);
}

TEST_CASE("JsonParser, object in array")
{
	std::string jsonText = R"(
		[ 5, { "b" : 38, "a" : "hello" } ]
	)";
	auto parserType = PARSER_TYPES();
	jsonpp::JsonParser parser(parserType);
	metapp::Variant var = parser.parse(jsonText.c_str(), jsonText.size());
	const jsonpp::JsonArray & array = var.get<const jsonpp::JsonArray &>();
	REQUIRE(array[0].get<jsonpp::JsonInt>() == 5);
	const jsonpp::JsonObject & object = array[1].get<const jsonpp::JsonObject &>();
	REQUIRE(object.at("a").get<std::string &>() == "hello");
	REQUIRE(object.at("b").get<jsonpp::JsonInt>() == 38);
}

