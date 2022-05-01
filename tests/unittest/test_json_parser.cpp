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

TEST_CASE("Test JsonParser, 1")
{
	std::string jsonText = R"(
		[ 5, "abc" ]
	)";
	jsonpp::JsonParser parser;
	metapp::Variant var = parser.parse(jsonText.c_str(), jsonText.size());
	REQUIRE(metapp::indexableGet(var, 0).cast<int>().get<int>() == 5);
	REQUIRE(metapp::indexableGet(var, 1).get<std::string &>() == "abc");
}

TEST_CASE("Test JsonParser, 2")
{
	std::string jsonText = R"(
		[ 5, 6 ]
	)";
	jsonpp::JsonParser parser;
	metapp::Variant var = parser.parse(jsonText.c_str(), jsonText.size(), metapp::getMetaType<std::deque<long> >());
	REQUIRE(metapp::indexableGet(var, 0).cast<int>().get<int>() == 5);
	REQUIRE(var.getMetaType()->getTypeKind() == metapp::tkStdDeque);
}
