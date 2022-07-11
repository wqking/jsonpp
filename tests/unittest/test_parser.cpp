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

TEST_CASE("Parser, error")
{
	auto parserType = PARSER_TYPES();
	const std::string jsonText = R"(
		5, 6
	)";
	jsonpp::Parser parser(parserType);
	SECTION("parse") {
		metapp::Variant var = parser.parse(jsonText);
		REQUIRE(var.isEmpty());
		REQUIRE(parser.hasError());
	}
	SECTION("parse<int>") {
		const int result = parser.parse<int>(jsonText);
		REQUIRE(result == 0);
		REQUIRE(parser.hasError());
	}
}

TEST_CASE("Parser, null")
{
	auto parserType = PARSER_TYPES();
	jsonpp::Parser parser(parserType);
	const std::string jsonText = R"(
		null
	)";
	SECTION("parse") {
		metapp::Variant var = parser.parse(jsonText);
		REQUIRE(jsonpp::getJsonType(var) == jsonpp::JsonType::jtNull);
		REQUIRE(var.get<jsonpp::JsonNull>() == nullptr);
	}
	SECTION("parse<int *>") {
		int * result = parser.parse<int *>(jsonText);
		REQUIRE(result == nullptr);
	}
}

TEST_CASE("Parser, bool")
{
	auto parserType = PARSER_TYPES();
	jsonpp::Parser parser(parserType);
	SECTION("true") {
		const std::string jsonText = R"(
			true
		)";
		metapp::Variant var = parser.parse(jsonText);
		REQUIRE(jsonpp::getJsonType(var) == jsonpp::JsonType::jtBool);
		REQUIRE(var.get<jsonpp::JsonBool>() == true);
	}
	SECTION("false") {
		const std::string jsonText = R"(
			false
		)";
		metapp::Variant var = parser.parse(jsonText);
		REQUIRE(jsonpp::getJsonType(var) == jsonpp::JsonType::jtBool);
		REQUIRE(var.get<jsonpp::JsonBool>() == false);
	}
	SECTION("true, parse<int>") {
		const std::string jsonText = R"(
			true
		)";
		const int result = parser.parse<int>(jsonText);
		REQUIRE(result == int(true));
	}
}

TEST_CASE("Parser, int")
{
	auto parserType = PARSER_TYPES();
	jsonpp::Parser parser(parserType);
	SECTION("positive") {
		const std::string jsonText = R"(
			9381538
		)";
		metapp::Variant var = parser.parse(jsonText);
		REQUIRE(jsonpp::getJsonType(var) == jsonpp::JsonType::jtInt);
		REQUIRE(var.get<jsonpp::JsonInt>() == 9381538);
	}
	SECTION("negative") {
		const std::string jsonText = R"(
			-19381538
		)";
		metapp::Variant var = parser.parse(jsonText);
		REQUIRE(jsonpp::getJsonType(var) == jsonpp::JsonType::jtInt);
		REQUIRE(var.get<jsonpp::JsonInt>() == -19381538);
	}
	SECTION("zero") {
		const std::string jsonText = R"(
			0
		)";
		metapp::Variant var = parser.parse(jsonText);
		REQUIRE(jsonpp::getJsonType(var) == jsonpp::JsonType::jtInt);
		REQUIRE(var.get<jsonpp::JsonInt>() == 0);
	}
	SECTION("positive, parse<long>") {
		const std::string jsonText = R"(
			9381538
		)";
		REQUIRE(parser.parse<long>(jsonText) == 9381538);
		REQUIRE(parser.parse<long>(jsonText.c_str(), jsonText.size()) == 9381538);
		REQUIRE(parser.parse<long>(jsonpp::ParserSource(jsonText.c_str(), jsonText.size())) == 9381538);
	}
}

TEST_CASE("Parser, double")
{
	auto parserType = PARSER_TYPES();
	jsonpp::Parser parser(parserType);
	SECTION("positive") {
		const std::string jsonText = R"(
			3.1415
		)";
		metapp::Variant var = parser.parse(jsonText);
		REQUIRE(jsonpp::getJsonType(var) == jsonpp::JsonType::jtReal);
		REQUIRE(var.get<jsonpp::JsonReal>() == 3.1415);
	}
	SECTION("negative") {
		const std::string jsonText = R"(
			-3.1415
		)";
		metapp::Variant var = parser.parse(jsonText);
		REQUIRE(jsonpp::getJsonType(var) == jsonpp::JsonType::jtReal);
		REQUIRE(var.get<jsonpp::JsonReal>() == -3.1415);
	}
	SECTION("zero") {
		const std::string jsonText = R"(
			0.0
		)";
		metapp::Variant var = parser.parse(jsonText);
		REQUIRE(jsonpp::getJsonType(var) == jsonpp::JsonType::jtReal);
		REQUIRE(var.get<jsonpp::JsonReal>() == 0.0);
	}
	SECTION("scientific, positive") {
		const std::string jsonText = R"(
			1.618e+2
		)";
		metapp::Variant var = parser.parse(jsonText);
		REQUIRE(jsonpp::getJsonType(var) == jsonpp::JsonType::jtReal);
		REQUIRE(var.get<jsonpp::JsonReal>() == Approx(161.8));
	}
	SECTION("scientific, negative") {
		const std::string jsonText = R"(
			-1.618e+2
		)";
		metapp::Variant var = parser.parse(jsonText);
		REQUIRE(jsonpp::getJsonType(var) == jsonpp::JsonType::jtReal);
		REQUIRE(var.get<jsonpp::JsonReal>() == Approx(-161.8));
	}
	SECTION("scientific, zero") {
		const std::string jsonText = R"(
			0.0e+2
		)";
		metapp::Variant var = parser.parse(jsonText);
		REQUIRE(jsonpp::getJsonType(var) == jsonpp::JsonType::jtReal);
		REQUIRE(var.get<jsonpp::JsonReal>() == Approx(0.0));
	}
}

TEST_CASE("Parser, string")
{
	auto parserType = PARSER_TYPES();
	jsonpp::Parser parser(parserType);
	const std::string jsonText = R"(
		"abc"
	)";
	metapp::Variant var = parser.parse(jsonText);
	REQUIRE(jsonpp::getJsonType(var) == jsonpp::JsonType::jtString);
	REQUIRE(var.get<std::string &>() == "abc");
}

TEST_CASE("Parser, array")
{
	auto parserType = PARSER_TYPES();
	jsonpp::Parser parser(parserType);
	const std::string jsonText = R"(
		[ 5, "abc" ]
	)";
	metapp::Variant var = parser.parse(jsonText);
	REQUIRE(jsonpp::getJsonType(var) == jsonpp::JsonType::jtArray);
	const jsonpp::JsonArray & array = var.get<const jsonpp::JsonArray &>();
	REQUIRE(array[0].get<jsonpp::JsonInt>() == 5);
	REQUIRE(array[1].get<std::string &>() == "abc");
}

TEST_CASE("Parser, array, proto")
{
	auto parserType = PARSER_TYPES();
	jsonpp::Parser parser(parserType);

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

TEST_CASE("Parser, object")
{
	auto parserType = PARSER_TYPES();
	jsonpp::Parser parser(parserType);
	const std::string jsonText = R"(
		{ "b" : 5, "a" : "hello" }
	)";
	metapp::Variant var = parser.parse(jsonText);
	REQUIRE(jsonpp::getJsonType(var) == jsonpp::JsonType::jtObject);
	const jsonpp::JsonObject & object = var.get<const jsonpp::JsonObject &>();
	REQUIRE(object.at("a").get<std::string &>() == "hello");
	REQUIRE(object.at("b").get<jsonpp::JsonInt>() == 5);
}

TEST_CASE("Parser, object, proto")
{
	auto parserType = PARSER_TYPES();
	jsonpp::Parser parser(parserType);
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

TEST_CASE("Parser, array in object")
{
	auto parserType = PARSER_TYPES();
	jsonpp::Parser parser(parserType);
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

TEST_CASE("Parser, object in array")
{
	auto parserType = PARSER_TYPES();
	jsonpp::Parser parser(parserType);
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

TEST_CASE("Parser, object, parse as array")
{
	auto parserType = PARSER_TYPES();
	jsonpp::Parser parser(parserType);
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

