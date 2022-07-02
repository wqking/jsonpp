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

#include "jsonpp/jsonparser.h"
#include "jsonpp/jsondumper.h"
#include "metapp/interfaces/metaindexable.h"
#include "metapp/allmetatypes.h"

#include <deque>
#include <unordered_map>
#include <iostream>

TEST_CASE("Test JsonDumper, 1")
{
	jsonpp::DumperConfig config;
	config.setBeautify(true);
	jsonpp::JsonDumper dumper(config);

	metapp::Variant value(std::map<std::string, metapp::Variant> {
		{ "first",  std::vector<metapp::Variant> {
			"abc",
				1,
				5.38,
				std::map<std::string, metapp::Variant> {
					{ "what", "good" },
					{ "next", 12345 },
			},
				std::unordered_map<std::string, std::string> {
					{ "hello", "def" },
					},
		}},
					{ "second", nullptr }
	});
	const std::string text = dumper.dump(value);
	//std::cout << text << std::endl;

	jsonpp::JsonParser parser;
	metapp::Variant parsed = parser.parse(text);
	const std::string text2 = dumper.dump(value);
	//std::cout << text2 << std::endl;
}
