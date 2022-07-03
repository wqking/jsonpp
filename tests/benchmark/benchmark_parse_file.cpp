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

#include "jsonpp/jsonparser.h"
#include "jsonpp/jsondumper.h"

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
};

void doBenchmarkParseFile(const FileInfo & fileInfo, const jsonpp::ParserType parserType)
{
	namespace fs = std::filesystem;

	const std::string & fullFileName = fileInfo.fileName;
	const int iterations = fileInfo.iterations;

	const std::string jsonText = readFile(fullFileName);
	if(jsonText.empty()) {
		std::cerr << fullFileName << " doesn't exist or can't be read." << std::endl;
		return;
	}

	const std::string pureFileName = fs::path(fullFileName).filename().string();

	metapp::Variant var;
	const auto t = measureElapsedTime([&var, iterations, jsonText, parserType]() {
		jsonpp::JsonParser parser(parserType);
		for(int i = 0; i < iterations; ++i) {
			var = parser.parse(jsonText);
		}
	});

	printResult(t, iterations, jsonpp::getParserTypeName(parserType) + " Parse file " + pureFileName);

	//jsonpp::JsonDumper dumper(jsonpp::DumperConfig().setBeautify(true));
	//std::ofstream f(fullFileName + ".output");
	//dumper.dump(var, f);
}

BenchmarkFunc
{
	auto parserType = PARSER_TYPES();
	for(std::size_t i = 0; i < sizeof(fileInfoList) / sizeof(fileInfoList[0]); ++i) {
		doBenchmarkParseFile(fileInfoList[i], parserType);
	}
}

void doBenchmarkDumpJson(const FileInfo & fileInfo, const bool beaufify)
{
	namespace fs = std::filesystem;

	const std::string & fullFileName = fileInfo.fileName;
	const int iterations = fileInfo.iterations;

	const std::string jsonText = readFile(fullFileName);
	if(jsonText.empty()) {
		std::cerr << fullFileName << " doesn't exist or can't be read." << std::endl;
		return;
	}

	const std::string pureFileName = fs::path(fullFileName).filename().string();

	metapp::Variant var = jsonpp::JsonParser().parse(jsonText);
	const auto t = measureElapsedTime([&var, iterations, beaufify]() {
		jsonpp::JsonDumper dumper(jsonpp::DumperConfig().setBeautify(beaufify));
		for(int i = 0; i < iterations; ++i) {
			dumper.dump(var);
		}
	});

	printResult(t, iterations, std::string(beaufify ? "Beautify" : "Minify") + " Dump file " + pureFileName);

	const std::string dumpedText = jsonpp::JsonDumper(jsonpp::DumperConfig().setBeautify(beaufify)).dump(var);
	metapp::Variant newVar = jsonpp::JsonParser().parse(dumpedText);
	REQUIRE(! newVar.isEmpty());
}

BenchmarkFunc
{
	for(std::size_t i = 0; i < sizeof(fileInfoList) / sizeof(fileInfoList[0]); ++i) {
		doBenchmarkDumpJson(fileInfoList[i], true);
		doBenchmarkDumpJson(fileInfoList[i], false);
	}
}


} //namespace
