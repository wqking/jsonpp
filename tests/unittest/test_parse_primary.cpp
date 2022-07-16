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

TEMPLATE_LIST_TEST_CASE("Parse, error", "", BackendTypes)
{
	constexpr auto backendType = TestType::backendType;
	const std::string jsonText = R"(
		5, 6
	)";
	jsonpp::Parser parser(jsonpp::ParserConfig().setBackendType<backendType>());
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

TEMPLATE_LIST_TEST_CASE("Parse, null", "", BackendTypes)
{
	constexpr auto backendType = TestType::backendType;
	jsonpp::Parser parser(jsonpp::ParserConfig().setBackendType<backendType>());
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

TEMPLATE_LIST_TEST_CASE("Parse, bool", "", BackendTypes)
{
	constexpr auto backendType = TestType::backendType;
	jsonpp::Parser parser(jsonpp::ParserConfig().setBackendType<backendType>());
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

TEMPLATE_LIST_TEST_CASE("Parse, int", "", BackendTypes)
{
	constexpr auto backendType = TestType::backendType;
	jsonpp::Parser parser(jsonpp::ParserConfig().setBackendType<backendType>());
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

TEMPLATE_LIST_TEST_CASE("Parse, double", "", BackendTypes)
{
	constexpr auto backendType = TestType::backendType;
	jsonpp::Parser parser(jsonpp::ParserConfig().setBackendType<backendType>());
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

TEMPLATE_LIST_TEST_CASE("Parse, string", "", BackendTypes)
{
	constexpr auto backendType = TestType::backendType;
	jsonpp::Parser parser(jsonpp::ParserConfig().setBackendType<backendType>());
	const std::string jsonText = R"(
		"abc"
	)";
	metapp::Variant var = parser.parse(jsonText);
	REQUIRE(jsonpp::getJsonType(var) == jsonpp::JsonType::jtString);
	REQUIRE(var.get<std::string &>() == "abc");
}

