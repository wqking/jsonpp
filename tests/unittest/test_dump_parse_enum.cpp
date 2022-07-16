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

TEMPLATE_LIST_TEST_CASE("DumpAndParse, enum", "", BackendTypes)
{
	constexpr auto backendType = TestType::backendType;
	enum class MyEnum { one = 1, two = 2 };
	auto dumperConfig = DUMPER_CONFIGS();
	const std::string jsonText = jsonpp::Dumper(dumperConfig).dump(MyEnum::one);
	metapp::Variant var = jsonpp::Parser(jsonpp::ParserConfig().setBackendType<backendType>()).parse(jsonText);
	REQUIRE(var.get<jsonpp::JsonInt>() == 1);
}

TEST_CASE("DumpAndParse, named enum")
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
