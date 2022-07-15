// jsonpp library
// Copyright (C) 2022 Wang Qi (wqking)
// Github: https://github.com/wqking/eventpp
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//   http://www.apache.org/licenses/LICENSE-2.0
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "jsonpp/dumper.h"
#include "jsonpp/parser.h"

#include <iostream>

int main()
{
	const std::string text = jsonpp::Dumper(jsonpp::DumperConfig().enableBeautify(true)).dump(jsonpp::JsonObject {
		{ "first", "hello" },
		{ "second", nullptr },
		{ "third", std::vector<int> { 5, 6, 7 } },
		{ "fourth", jsonpp::JsonArray { "abc", 9.1 } },
	});
	std::cout << text << std::endl;
	jsonpp::Parser().parse(text);

	return 0;
}

