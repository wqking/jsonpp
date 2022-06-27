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

#include "test.h"

#include "jsonpp/jsonparser.h"
#include "metapp/interfaces/metaindexable.h"
#include "metapp/allmetatypes.h"

#include <deque>
#include <unordered_map>
#include <iostream>

TEST_CASE("Test JsonParser, error")
{
	std::string jsonText = R"(
		5, 6
	)";
	jsonpp::JsonParser parser;
	metapp::Variant var = parser.parse(jsonText.c_str(), jsonText.size());
	REQUIRE(var.isEmpty());
	REQUIRE(parser.hasError());
}

TEST_CASE("Test JsonParser, int")
{
	std::string jsonText = R"(
		5
	)";
	jsonpp::JsonParser parser;
	metapp::Variant var = parser.parse(jsonText.c_str(), jsonText.size());
	REQUIRE(var.cast<int>().get<int>() == 5);
}

TEST_CASE("Test JsonParser, string")
{
	std::string jsonText = R"(
		"abc"
	)";
	jsonpp::JsonParser parser;
	metapp::Variant var = parser.parse(jsonText.c_str(), jsonText.size());
	REQUIRE(var.get<std::string &>() == "abc");
}

TEST_CASE("Test JsonParser, array")
{
	std::string jsonText = R"(
		[ 5, "abc" ]
	)";
	jsonpp::JsonParser parser;
	metapp::Variant var = parser.parse(jsonText.c_str(), jsonText.size());
	REQUIRE(metapp::indexableGet(var, 0).cast<int>().get<int>() == 5);
	REQUIRE(metapp::indexableGet(var, 1).get<std::string &>() == "abc");
}

TEST_CASE("Test JsonParser, array, proto")
{
	std::string jsonText = R"(
		[ 5, 6 ]
	)";
	jsonpp::JsonParser parser;
	metapp::Variant var = parser.parse(jsonText.c_str(), jsonText.size(), metapp::getMetaType<std::deque<long> >());
	REQUIRE(metapp::indexableGet(var, 0).cast<int>().get<int>() == 5);
	REQUIRE(metapp::indexableGet(var, 1).cast<int>().get<int>() == 6);
	REQUIRE(var.getMetaType()->getTypeKind() == metapp::tkStdDeque);
}

TEST_CASE("Test JsonParser, object")
{
	std::string jsonText = R"(
		{ "b" : 5, "a" : "hello" }
	)";
	jsonpp::JsonParser parser;
	metapp::Variant var = parser.parse(jsonText.c_str(), jsonText.size());
	REQUIRE(metapp::mappableGet(var, "a").get<std::string &>() == "hello");
	REQUIRE(metapp::mappableGet(var, "b").cast<int>().get<int>() == 5);
}

TEST_CASE("Test JsonParser, array in object")
{
	std::string jsonText = R"(
		{ "b" : 5, "a" : [ "hello", 38 ] }
	)";
	jsonpp::JsonParser parser;
	metapp::Variant var = parser.parse(jsonText.c_str(), jsonText.size());
	REQUIRE(metapp::mappableGet(var, "b").cast<int>().get<int>() == 5);
	metapp::Variant a = metapp::mappableGet(var, "a").get<metapp::Variant>();
	REQUIRE(metapp::indexableGet(a, 0).get<std::string &>() == "hello");
	REQUIRE(metapp::indexableGet(a, 1).cast<int>().template get<int>() == 38);
}

TEST_CASE("Test JsonParser, object in array")
{
	std::string jsonText = R"(
		[ 5, { "b" : 38, "a" : "hello" } ]
	)";
	jsonpp::JsonParser parser;
	metapp::Variant var = parser.parse(jsonText.c_str(), jsonText.size());
	REQUIRE(metapp::indexableGet(var, 0).cast<int>().get<int>() == 5);
	metapp::Variant second = metapp::indexableGet(var, 1).get<metapp::Variant>();
	REQUIRE(metapp::mappableGet(second, "a").get<std::string &>() == "hello");
	REQUIRE(metapp::mappableGet(second, "b").cast<int>().get<int>() == 38);
}

