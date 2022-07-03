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
#include "classes.h"
#include "test_parser.h"

#include "jsonpp/jsondumper.h"
#include "jsonpp/jsonparser.h"
#include "metapp/interfaces/metaindexable.h"
#include "metapp/allmetatypes.h"

#include <deque>
#include <unordered_map>
#include <iostream>

TEST_CASE("JsonDumper, dump/parse, null")
{
	auto parserType = PARSER_TYPES();
	std::string jsonText = jsonpp::JsonDumper().dump(nullptr);
	metapp::Variant var = jsonpp::JsonParser(parserType).parse(jsonText);
	REQUIRE(var.get<jsonpp::JsonNull>() == nullptr);
}

TEST_CASE("JsonDumper, dump/parse, bool")
{
	auto parserType = PARSER_TYPES();
	SECTION("true") {
		std::string jsonText = jsonpp::JsonDumper().dump(true);
		metapp::Variant var = jsonpp::JsonParser(parserType).parse(jsonText);
		REQUIRE(var.get<jsonpp::JsonBool>());
	}

	SECTION("false") {
		std::string jsonText = jsonpp::JsonDumper().dump(false);
		metapp::Variant var = jsonpp::JsonParser(parserType).parse(jsonText);
		REQUIRE(! var.get<jsonpp::JsonBool>());
	}
}

TEST_CASE("JsonDumper, dump/parse, int")
{
	auto parserType = PARSER_TYPES();
	SECTION("38") {
		std::string jsonText = jsonpp::JsonDumper().dump(38);
		metapp::Variant var = jsonpp::JsonParser(parserType).parse(jsonText);
		REQUIRE(var.get<jsonpp::JsonInt>() == 38);
	}
	SECTION("-98765") {
		std::string jsonText = jsonpp::JsonDumper().dump(-98765);
		metapp::Variant var = jsonpp::JsonParser(parserType).parse(jsonText);
		REQUIRE(var.get<jsonpp::JsonInt>() == -98765);
	}
}

TEST_CASE("JsonDumper, dump/parse, double")
{
	auto parserType = PARSER_TYPES();
	SECTION("9.1") {
		std::string jsonText = jsonpp::JsonDumper().dump(9.1);
		metapp::Variant var = jsonpp::JsonParser(parserType).parse(jsonText);
		REQUIRE(var.get<jsonpp::JsonReal>() == 9.1);
	}
	SECTION("-3.14") {
		std::string jsonText = jsonpp::JsonDumper().dump(-3.14);
		metapp::Variant var = jsonpp::JsonParser(parserType).parse(jsonText);
		REQUIRE(var.get<jsonpp::JsonReal>() == -3.14);
	}
}

TEST_CASE("JsonDumper, dump/parse, string")
{
	auto parserType = PARSER_TYPES();
	std::string jsonText = jsonpp::JsonDumper().dump("Hello world");
	metapp::Variant var = jsonpp::JsonParser(parserType).parse(jsonText);
	REQUIRE(var.get<const std::string &>() == "Hello world");
}

TEST_CASE("JsonDumper, dump/parse, string, escape/unescape")
{
	auto parserType = PARSER_TYPES();
	const std::string text = "\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1a\x1b\x1c\x1d\x1e\x1f";
	std::string jsonText = jsonpp::JsonDumper().dump(text);
	metapp::Variant var = jsonpp::JsonParser(parserType).parse(jsonText);
	REQUIRE(var.get<const std::string &>() == text);
}

TEST_CASE("JsonDumper, dump/parse, array")
{
	auto parserType = PARSER_TYPES();
	std::string jsonText = jsonpp::JsonDumper().dump(std::make_tuple("Hello world", 5, -9, "good"));
	metapp::Variant var = jsonpp::JsonParser(parserType).parse(jsonText);
	const jsonpp::JsonArray & array = var.get<const jsonpp::JsonArray &>();
	REQUIRE(array[0].get<const std::string &>() == "Hello world");
	REQUIRE(array[1].get<jsonpp::JsonInt>() == 5);
	REQUIRE(array[2].get<jsonpp::JsonInt>() == -9);
	REQUIRE(array[3].get<const std::string &>() == "good");
}

TEST_CASE("JsonDumper, dump/parse, TestClass2")
{
	auto parserType = PARSER_TYPES();
	std::string jsonText = jsonpp::JsonDumper(jsonpp::DumperConfig().setBeautify(true)).dump(makeTestClass2(0));
	metapp::Variant var = jsonpp::JsonParser(parserType).parse(jsonText, metapp::getMetaType<TestClass2>());
	REQUIRE(var.get<const TestClass2 &>() == makeTestClass2(0));
}

TEST_CASE("JsonDumper, dump/parse, std::vector<TestClass2>")
{
	auto parserType = PARSER_TYPES();
	std::string jsonText = jsonpp::JsonDumper(jsonpp::DumperConfig().setBeautify(true)).dump(
		std::vector<TestClass2> {
			makeTestClass2(0),
			makeTestClass2(1),
		}
	);
	metapp::Variant var = jsonpp::JsonParser(parserType).parse(jsonText, metapp::getMetaType<std::vector<TestClass2> >());
	REQUIRE(var.get<const std::vector<TestClass2> &>()[0] == makeTestClass2(0));
	REQUIRE(var.get<const std::vector<TestClass2> &>()[1] == makeTestClass2(1));
}

