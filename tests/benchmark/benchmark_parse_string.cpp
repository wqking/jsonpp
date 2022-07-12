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

#include "benchmark.h"

#include "jsonpp/parser.h"
#include "jsonpp/dumper.h"

namespace {

BenchmarkFunc
{
	auto parserType = PARSER_TYPES();
	constexpr int iterations = 1000 * 100;
	const std::string jsonText = R"([ 5, { "b" : 38, "a" : "hello" } ])";
	const auto t = measureElapsedTime([iterations, jsonText, parserType]() {
		jsonpp::Parser parser(jsonpp::ParserConfig().setBackendType(parserType));
		for(int i = 0; i < iterations; ++i) {
			parser.parse(jsonText);
		}
	});
	printResult(t, iterations, jsonpp::getParserTypeName(parserType) + " Parse string");
}



} //namespace
