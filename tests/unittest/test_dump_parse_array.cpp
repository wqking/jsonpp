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

TEMPLATE_LIST_TEST_CASE("DumpAndParse, array", "", BackendTypes)
{
	constexpr auto backendType = TestType::backendType;
	auto dumperConfig = DUMPER_CONFIGS();
	const std::string jsonText = jsonpp::Dumper(dumperConfig).dump(std::make_tuple("Hello world", 5, -9, "good"));
	metapp::Variant var = jsonpp::Parser(jsonpp::ParserConfig().setBackendType<backendType>()).parse(jsonText);
	const jsonpp::JsonArray & array = var.get<const jsonpp::JsonArray &>();
	REQUIRE(array[0].get<const std::string &>() == "Hello world");
	REQUIRE(array[1].get<jsonpp::JsonInt>() == 5);
	REQUIRE(array[2].get<jsonpp::JsonInt>() == -9);
	REQUIRE(array[3].get<const std::string &>() == "good");
}

