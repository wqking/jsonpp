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

#include "../src/thirdparty/simdjson/simdjson.h"

//#define NLOH

#ifdef NLOH
#include "json.hpp"
#endif

#include <string>
#include <fstream>
#include <sstream>
#include <filesystem>

namespace {

struct FileInfo
{
	std::string fileName;
	int iterations;
};

FileInfo fileInfoList[] = {
	{ "testdata/canada.json", 10 },
	{ "testdata/citm_catalog.json", 10 },
	{ "testdata/twitter.json", 10 },
	{ "testdata/airlines.json", 10 },
	{ "testdata/tiny.json", 10000 },
	//{ "testdata/Zurich_Building_LoD2_V10.city.json", 1 },
};

void doBenchmarkParseFile(const FileInfo & fileInfo, const jsonpp::ParserType parserType)
{
	namespace fs = std::filesystem;

	const std::string & fullFileName = fileInfo.fileName;
	const int iterations = fileInfo.iterations;

	std::string jsonText = readFile(fullFileName);
	if(jsonText.empty()) {
		return;
	}

	const std::string pureFileName = fs::path(fullFileName).filename().string();

	const auto fileSize = jsonText.size();
	jsonpp::Parser parser(jsonpp::ParserConfig().setBackendType(parserType));
	auto source = jsonpp::ParserSource(std::move(jsonText));
	REQUIRE(jsonText.empty());
	const auto t = measureElapsedTime([&parser, iterations, &source, parserType]() {
		for(int i = 0; i < iterations; ++i) {
			parser.parse(source);
		}
	});

	printTps(t, iterations, fileSize, jsonpp::getParserTypeName(parserType) + " Parse file " + pureFileName);
}

BenchmarkFunc
{
	std::cout << std::endl;

	auto parserType = PARSER_TYPES();
	for(const auto & fileInfo : fileInfoList) {
		doBenchmarkParseFile(fileInfo, parserType);
	}
}

void doBenchmarkDumpJson(const FileInfo & fileInfo, const bool beaufify)
{
	namespace fs = std::filesystem;

	const std::string & fullFileName = fileInfo.fileName;
	const int iterations = fileInfo.iterations;

	const std::string jsonText = readFile(fullFileName);
	if(jsonText.empty()) {
		return;
	}

	const std::string pureFileName = fs::path(fullFileName).filename().string();

	metapp::Variant var = jsonpp::Parser().parse(jsonText);
	const auto t = measureElapsedTime([&var, iterations, beaufify]() {
		jsonpp::Dumper dumper(jsonpp::DumperConfig().enableBeautify(beaufify));
		for(int i = 0; i < iterations; ++i) {
			dumper.dump(var);
		}
	});

	const std::string dumpedText = jsonpp::Dumper(jsonpp::DumperConfig().enableBeautify(beaufify)).dump(var);
	metapp::Variant newVar = jsonpp::Parser().parse(dumpedText);
	REQUIRE(! newVar.isEmpty());

	printTps(t, iterations, dumpedText.size(), std::string(beaufify ? "Beautify" : "Minify") + " Dump file " + pureFileName);
}

BenchmarkFunc
{
	std::cout << std::endl;

	for(const auto & fileInfo : fileInfoList) {
		doBenchmarkDumpJson(fileInfo, true);
		doBenchmarkDumpJson(fileInfo, false);
	}
}

BenchmarkFunc
{
	std::cout << std::endl;

	for(const auto & fileInfo : fileInfoList) {
		namespace fs = std::filesystem;

		const std::string & fullFileName = fileInfo.fileName;
		const int iterations = fileInfo.iterations;

		const std::string jsonText = readFile(fullFileName);
		if(jsonText.empty()) {
			return;
		}

		const std::string pureFileName = fs::path(fullFileName).filename().string();

		simdjson::dom::parser parser;
		simdjson::padded_string json(jsonText.c_str(), jsonText.size());
		const auto t1 = measureElapsedTime([iterations, &json, &parser]() {
			for(int i = 0; i < iterations; ++i) {
				simdjson::dom::element element;
				auto r = parser.parse(json).get(element);
				(void)r;
			}
		});

		printTps(t1, iterations, jsonText.size(), "simdjson DOM Parse file " + pureFileName);
	}
}

// Change it to #if 1 to enable benchmarking nlohmann json, the proper header must be included
#ifdef NLOH
BenchmarkFunc
{
	std::cout << std::endl;

	for(const auto & fileInfo : fileInfoList) {
		namespace fs = std::filesystem;

		const std::string & fullFileName = fileInfo.fileName;
		const int iterations = fileInfo.iterations;

		const std::string jsonText = readFile(fullFileName);
		if(jsonText.empty()) {
			return;
		}

		const std::string pureFileName = fs::path(fullFileName).filename().string();

		const auto t1 = measureElapsedTime([iterations, jsonText]() {
			for(int i = 0; i < iterations; ++i) {
				auto result = nlohmann::json::parse(jsonText);
			}
		});

		printTps(t1, iterations, jsonText.size(), "nlo Parse file " + pureFileName);

		auto parsed = nlohmann::json::parse(jsonText);
		const auto t2 = measureElapsedTime([iterations, &parsed]() {
			for(int i = 0; i < iterations; ++i) {
				parsed.dump();
			}
		});

		printTps(t2, iterations, jsonText.size(), "nlo Dump file " + pureFileName);
	}
}
#endif


} //namespace
