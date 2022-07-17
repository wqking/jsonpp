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

TEMPLATE_LIST_TEST_CASE("DumpAndParse, array, std::tuple", "", BackendTypes)
{
	constexpr auto backendType = TestType::backendType;
	auto dumperConfig = DUMPER_CONFIGS();
	const std::string jsonText = jsonpp::Dumper(dumperConfig).dump(std::make_tuple("Hello world", 5, -9, "good"));
	jsonpp::ParserConfig parserConfig;
	parserConfig.setBackendType<backendType>();

	SECTION("parse as default") {
		const metapp::Variant var = jsonpp::Parser(parserConfig).parse(jsonText);
		const jsonpp::JsonArray & array = var.get<const jsonpp::JsonArray &>();
		REQUIRE(array[0].get<const std::string &>() == "Hello world");
		REQUIRE(array[1].get<jsonpp::JsonInt>() == 5);
		REQUIRE(array[2].get<jsonpp::JsonInt>() == -9);
		REQUIRE(array[3].get<const std::string &>() == "good");
	}
	SECTION("parse as prototype") {
		using T = std::tuple<std::string, double, long, std::string>;
		const metapp::Variant var = jsonpp::Parser(parserConfig).parse(jsonText, metapp::getMetaType<T>());
		const T & tuple = var.get<const T &>();
		REQUIRE(std::get<0>(tuple) == "Hello world");
		REQUIRE(std::get<1>(tuple) == 5.0);
		REQUIRE(std::get<2>(tuple) == -9);
		REQUIRE(std::get<3>(tuple) == "good");
	}
	SECTION("parse as template") {
		using T = std::tuple<std::string, double, long, std::string>;
		const T tuple = jsonpp::Parser(parserConfig).parse<T>(jsonText);
		REQUIRE(std::get<0>(tuple) == "Hello world");
		REQUIRE(std::get<1>(tuple) == 5.0);
		REQUIRE(std::get<2>(tuple) == -9);
		REQUIRE(std::get<3>(tuple) == "good");
	}
}

using ArrayTypeList = std::tuple<
	std::vector<int>,
	std::vector<double>,
	std::deque<long>,
	std::deque<int>,
	std::list<long long>,
	std::array<int, 5>
>;

TEMPLATE_LIST_TEST_CASE("DumpAndParse, array, ArrayTypeList", "", ArrayTypeList)
{
	using ArrayType = TestType;
	auto dumperConfig = DUMPER_CONFIGS();
	const std::string jsonText = jsonpp::Dumper(dumperConfig).dump(ArrayType { 253291, -19, 0, 5, -38 });
	jsonpp::ParserConfig parserConfig;

	SECTION("parse as default") {
		const metapp::Variant var = jsonpp::Parser(parserConfig).parse(jsonText);
		const jsonpp::JsonArray & array = var.get<const jsonpp::JsonArray &>();
		REQUIRE(array[0].get<jsonpp::JsonInt>() == 253291);
		REQUIRE(array[1].get<jsonpp::JsonInt>() == -19);
		REQUIRE(array[2].get<jsonpp::JsonInt>() == 0);
		REQUIRE(array[3].get<jsonpp::JsonInt>() == 5);
		REQUIRE(array[4].get<jsonpp::JsonInt>() == -38);
	}
	SECTION("parse as prototype, std::vector<int>") {
		using T = std::vector<int>;
		const metapp::Variant var = jsonpp::Parser(parserConfig).parse(jsonText, metapp::getMetaType<T>());
		const T & array = var.get<const T &>();
		REQUIRE(array[0] == 253291);
		REQUIRE(array[1] == -19);
		REQUIRE(array[2] == 0);
		REQUIRE(array[3] == 5);
		REQUIRE(array[4] == -38);
	}
	SECTION("parse as template, std::deque<long long>") {
		using T = std::deque<long long>;
		const T array = jsonpp::Parser(parserConfig).parse<T>(jsonText);
		REQUIRE(array[0] == 253291);
		REQUIRE(array[1] == -19);
		REQUIRE(array[2] == 0);
		REQUIRE(array[3] == 5);
		REQUIRE(array[4] == -38);
	}
}

