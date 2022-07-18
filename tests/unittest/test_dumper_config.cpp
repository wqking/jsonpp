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

TEST_CASE("DumperConfig, addObjectType")
{
	jsonpp::DumperConfig dumperConfig;
	dumperConfig.addObjectType<std::deque<std::pair<std::string, int> > >();
	jsonpp::Dumper dumper(dumperConfig);
	const std::string jsonText = dumper.dump(
		std::deque<std::pair<std::string, int> > {
			{ "a", 5 },
			{ "b", 6 },
			{ "c", 7 }
		}
	);
	const metapp::Variant var = jsonpp::Parser().parse(jsonText);
	const auto & object = var.get<const jsonpp::JsonObject &>();
	REQUIRE(object.at("a").get<jsonpp::JsonInt>() == 5);
	REQUIRE(object.at("b").get<jsonpp::JsonInt>() == 6);
	REQUIRE(object.at("c").get<jsonpp::JsonInt>() == 7);
}

TEST_CASE("DumperConfig, addArrayType")
{
	jsonpp::DumperConfig dumperConfig;
	dumperConfig.addArrayType<std::map<std::string, int> >();
	jsonpp::Dumper dumper(dumperConfig);
	const std::string jsonText = dumper.dump(
		std::map<std::string, int> {
			{ "a", 5 },
			{ "b", 6 },
			{ "c", 7 }
		}
	);
	const metapp::Variant var = jsonpp::Parser().parse(jsonText);
	const auto & array = var.get<const jsonpp::JsonArray &>();
	REQUIRE(array[0].get<const jsonpp::JsonArray &>()[0].get<const std::string &>() == "a");
	REQUIRE(array[0].get<const jsonpp::JsonArray &>()[1].get<jsonpp::JsonInt>() == 5);
	REQUIRE(array[1].get<const jsonpp::JsonArray &>()[0].get<const std::string &>() == "b");
	REQUIRE(array[1].get<const jsonpp::JsonArray &>()[1].get<jsonpp::JsonInt>() == 6);
	REQUIRE(array[2].get<const jsonpp::JsonArray &>()[0].get<const std::string &>() == "c");
	REQUIRE(array[2].get<const jsonpp::JsonArray &>()[1].get<jsonpp::JsonInt>() == 7);
}

