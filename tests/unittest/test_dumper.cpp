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

#include "jsonpp/dumper.h"
#include "jsonpp/parser.h"
#include "metapp/interfaces/metaindexable.h"
#include "metapp/allmetatypes.h"

#include <deque>
#include <unordered_map>
#include <iostream>

TEMPLATE_LIST_TEST_CASE("Dumper, dump/parse, null", "", BackendTypes)
{
	constexpr auto parserType = TestType::backendType;
	auto dumperConfig = DUMPER_CONFIGS();
	const std::string jsonText = jsonpp::Dumper(dumperConfig).dump(nullptr);
	metapp::Variant var = jsonpp::Parser(jsonpp::ParserConfig().setBackendType<parserType>()).parse(jsonText);
	REQUIRE(var.get<jsonpp::JsonNull>() == nullptr);
}

TEMPLATE_LIST_TEST_CASE("Dumper, dump/parse, bool", "", BackendTypes)
{
	constexpr auto parserType = TestType::backendType;
	auto dumperConfig = DUMPER_CONFIGS();
	SECTION("true") {
		const std::string jsonText = jsonpp::Dumper(dumperConfig).dump(true);
		metapp::Variant var = jsonpp::Parser(jsonpp::ParserConfig().setBackendType<parserType>()).parse(jsonText);
		REQUIRE(var.get<jsonpp::JsonBool>());
	}

	SECTION("false") {
		const std::string jsonText = jsonpp::Dumper(dumperConfig).dump(false);
		metapp::Variant var = jsonpp::Parser(jsonpp::ParserConfig().setBackendType<parserType>()).parse(jsonText);
		REQUIRE(! var.get<jsonpp::JsonBool>());
	}
}

TEMPLATE_LIST_TEST_CASE("Dumper, dump/parse, int", "", BackendTypes)
{
	constexpr auto parserType = TestType::backendType;
	auto dumperConfig = DUMPER_CONFIGS();
	SECTION("38") {
		const std::string jsonText = jsonpp::Dumper(dumperConfig).dump(38);
		metapp::Variant var = jsonpp::Parser(jsonpp::ParserConfig().setBackendType<parserType>()).parse(jsonText);
		REQUIRE(var.get<jsonpp::JsonInt>() == 38);
	}
	SECTION("-98765") {
		const std::string jsonText = jsonpp::Dumper(dumperConfig).dump(-98765);
		metapp::Variant var = jsonpp::Parser(jsonpp::ParserConfig().setBackendType<parserType>()).parse(jsonText);
		REQUIRE(var.get<jsonpp::JsonInt>() == -98765);
	}
}

TEMPLATE_LIST_TEST_CASE("Dumper, dump/parse, double", "", BackendTypes)
{
	constexpr auto parserType = TestType::backendType;
	auto dumperConfig = DUMPER_CONFIGS();
	SECTION("9.1") {
		const std::string jsonText = jsonpp::Dumper(dumperConfig).dump(9.1);
		metapp::Variant var = jsonpp::Parser(jsonpp::ParserConfig().setBackendType<parserType>()).parse(jsonText);
		REQUIRE(var.get<jsonpp::JsonReal>() == 9.1);
	}
	SECTION("-3.14") {
		const std::string jsonText = jsonpp::Dumper(dumperConfig).dump(-3.14);
		metapp::Variant var = jsonpp::Parser(jsonpp::ParserConfig().setBackendType<parserType>()).parse(jsonText);
		REQUIRE(var.get<jsonpp::JsonReal>() == -3.14);
	}
}

TEMPLATE_LIST_TEST_CASE("Dumper, dump/parse, string", "", BackendTypes)
{
	constexpr auto parserType = TestType::backendType;
	auto dumperConfig = DUMPER_CONFIGS();
	const std::string jsonText = jsonpp::Dumper(dumperConfig).dump("Hello world");
	metapp::Variant var = jsonpp::Parser(jsonpp::ParserConfig().setBackendType<parserType>()).parse(jsonText);
	REQUIRE(var.get<const std::string &>() == "Hello world");
}

TEMPLATE_LIST_TEST_CASE("Dumper, dump/parse, string, escape/unescape", "", BackendTypes)
{
	constexpr auto parserType = TestType::backendType;
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
	metapp::Variant var = jsonpp::Parser(jsonpp::ParserConfig().setBackendType<parserType>()).parse(jsonText);
	REQUIRE(var.get<const std::string &>() == text);
}

TEMPLATE_LIST_TEST_CASE("Dumper, dump/parse, array", "", BackendTypes)
{
	constexpr auto parserType = TestType::backendType;
	auto dumperConfig = DUMPER_CONFIGS();
	const std::string jsonText = jsonpp::Dumper(dumperConfig).dump(std::make_tuple("Hello world", 5, -9, "good"));
	metapp::Variant var = jsonpp::Parser(jsonpp::ParserConfig().setBackendType<parserType>()).parse(jsonText);
	const jsonpp::JsonArray & array = var.get<const jsonpp::JsonArray &>();
	REQUIRE(array[0].get<const std::string &>() == "Hello world");
	REQUIRE(array[1].get<jsonpp::JsonInt>() == 5);
	REQUIRE(array[2].get<jsonpp::JsonInt>() == -9);
	REQUIRE(array[3].get<const std::string &>() == "good");
}

TEMPLATE_LIST_TEST_CASE("Dumper, dump/parse, TestClass2", "", BackendTypes)
{
	constexpr auto parserType = TestType::backendType;
	auto dumperConfig = DUMPER_CONFIGS();
	const std::string jsonText = jsonpp::Dumper(jsonpp::DumperConfig(dumperConfig).enableBeautify(true)).dump(makeTestClass2(0));
	metapp::Variant var = jsonpp::Parser(jsonpp::ParserConfig().setBackendType<parserType>()).parse(jsonText, metapp::getMetaType<TestClass2>());
	REQUIRE(var.get<const TestClass2 &>() == makeTestClass2(0));
}

TEMPLATE_LIST_TEST_CASE("Dumper, dump/parse, std::vector<TestClass2>", "", BackendTypes)
{
	constexpr auto parserType = TestType::backendType;
	auto dumperConfig = DUMPER_CONFIGS();
	const std::string jsonText = jsonpp::Dumper(dumperConfig).dump(
		std::vector<TestClass2> {
			makeTestClass2(0),
			makeTestClass2(1),
		}
	);
	metapp::Variant var = jsonpp::Parser(jsonpp::ParserConfig().setBackendType<parserType>()).parse(jsonText, metapp::getMetaType<std::vector<TestClass2> >());
	REQUIRE(var.get<const std::vector<TestClass2> &>()[0] == makeTestClass2(0));
	REQUIRE(var.get<const std::vector<TestClass2> &>()[1] == makeTestClass2(1));
}

TEMPLATE_LIST_TEST_CASE("Dumper, dump/parse, enum", "", BackendTypes)
{
	constexpr auto parserType = TestType::backendType;
	enum class MyEnum { one = 1, two = 2 };
	auto dumperConfig = DUMPER_CONFIGS();
	const std::string jsonText = jsonpp::Dumper(dumperConfig).dump(MyEnum::one);
	metapp::Variant var = jsonpp::Parser(jsonpp::ParserConfig().setBackendType<parserType>()).parse(jsonText);
	REQUIRE(var.get<jsonpp::JsonInt>() == 1);
}

TEST_CASE("Dumper, dump/parse, named enum")
{
	SECTION("TestEnum1") {
		const std::string jsonText = jsonpp::Dumper(jsonpp::DumperConfig().enableNamedEnum(true)).dump(TestEnum1::dog);
		REQUIRE(jsonText == R"("dog")");
		auto var = jsonpp::Parser().parse(jsonText, metapp::getMetaType<TestEnum1>());
		REQUIRE(var.get<TestEnum1>() == TestEnum1::dog);
	}

	SECTION("TestEnum1, unregistered") {
		const std::string jsonText = jsonpp::Dumper(jsonpp::DumperConfig().enableNamedEnum(true)).dump(TestEnum1(1235));
		REQUIRE(jsonText == R"(1235)");
		auto var = jsonpp::Parser().parse(jsonText, metapp::getMetaType<TestEnum1>());
		REQUIRE(var.get<TestEnum1>() == TestEnum1(1235));
	}

	SECTION("std::vector<TestEnum1>") {
		const std::string jsonText = jsonpp::Dumper(jsonpp::DumperConfig().enableNamedEnum(true)).dump(
			std::vector<TestEnum1> {
			TestEnum1::second,
			TestEnum1::dog,
			TestEnum1::first,
			TestEnum1::cat,
		});
		REQUIRE(jsonText == R"(["second","dog","first","cat"])");
		auto var = jsonpp::Parser().parse(jsonText, metapp::getMetaType<std::vector<TestEnum1> >());
		const auto & enumList = var.get<const std::vector<TestEnum1> &>();
		REQUIRE(enumList[0] == TestEnum1::second);
		REQUIRE(enumList[1] == TestEnum1::dog);
		REQUIRE(enumList[2] == TestEnum1::first);
		REQUIRE(enumList[3] == TestEnum1::cat);
	}

}
