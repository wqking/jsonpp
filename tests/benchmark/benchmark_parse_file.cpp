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

#if defined(METAPP_COMPILER_VC)
#pragma warning(push)
#pragma warning(disable: 4100 4127 4244 4245 4267 4458 4459 4996 4706)
#endif

#include "../src/thirdparty/simdjson/simdjson.h"
#include "thirdparty/metrics.hpp"

#if defined(METAPP_COMPILER_VC)
#pragma warning(pop)
#endif

// Uncomment it to enable benchmarking nlohmann json, the proper header must be included
//#define NLOH

#ifdef NLOH
#include "json.hpp"
#endif

#include <string>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <map>
#include <vector>

namespace {

struct FileInfo
{
	std::string fileName;
	int iterations;
};

FileInfo fileInfoList[] = {
	{ "testdata/canada.json", 10 },
	{ "testdata/citm_catalog.json", 100 },
	{ "testdata/twitter.json", 100 },
	{ "testdata/airlines.json", 10 },
	{ "testdata/tiny.json", 10000 },
	//{ "testdata/Zurich_Building_LoD2_V10.city.json", 1 },
};

std::string formatSingleTime(uint64_t time, uint64_t iterations)
{
	char buffer[100];
	sprintf(buffer, "%g", (double)time / (double)iterations);
	return buffer;
}

std::string extractFileName(const std::string & fullFileName)
{
	namespace fs = std::filesystem;
	return fs::path(fullFileName).filename().string();
}

std::string ensureTextWidth(std::string text, const std::size_t width)
{
	while(text.size() < width) {
		text = " " + text;
	}
	return text;
}

constexpr jsonpp::ParserBackendType noBackend = jsonpp::ParserBackendType(-1);

class BenchmarkDataCollection
{
private:
	struct BenchmarkData
	{
		std::string fileName;
		std::string libName;
		uint64_t fileSize;
		uint64_t time;
		uint64_t iterations;
	};

	struct BenchmarkDataList
	{
		std::vector<BenchmarkData> dataList;
		std::vector<std::string> fileList;
		std::vector<std::string> libList;

		void add(BenchmarkData data) {
			if(data.fileName == "Zurich_Building_LoD2_V10.city.json") {
				data.fileName = "Zurich_Building.json";
			}
			addText(data.fileName, fileList);
			addText(data.libName, libList);
			dataList.push_back(data);
		}

		const BenchmarkData * findData(const std::string & fileName, const std::string & libName) {
			for(const auto & data : dataList) {
				if(data.fileName == fileName && data.libName == libName) {
					return &data;
				}
			}
			return nullptr;
		}

	private:
		void addText(const std::string & text, std::vector<std::string> & textList) {
			if(std::find(textList.begin(), textList.end(), text) == textList.end()) {
				textList.push_back(text);
			}
		}

	};

	struct ParseData
	{
		std::string fileName;
		std::string libName;
		uint64_t fileSize;
		uint64_t time;
		uint64_t iterations;
	};

	struct DumpData
	{
		std::string fileName;
		std::string libName;
		uint64_t fileSize;
		uint64_t time;
		uint64_t iterations;
	};

public:
	void dumpMarkdownTables() {
		std::cout << std::endl;
		dumpParserMarkdownTables();

		std::cout << std::endl;
		dumpDumpMarkdownTables();
	}

	void addParseResult(
		const std::string & fullFileName,
		const std::string & libName,
		const jsonpp::ParserBackendType backendType,
		const uint64_t fileSize,
		const uint64_t time,
		const uint64_t iterations
	) {
		std::string fullLibName = libName;
		if(backendType != noBackend) {
			fullLibName += " (" + jsonpp::getParserBackendName(backendType) + ")";
		}
		const std::string pureFileName = extractFileName(fullFileName);

		parseDataList.add({
			pureFileName,
			fullLibName,
			fileSize,
			time,
			iterations
		});
	}

	void addDumpResult(
		const std::string & fullFileName,
		const std::string & libName,
		const bool beautify,
		const uint64_t fileSize,
		const uint64_t time,
		const uint64_t iterations
	) {
		std::string fullLibName = libName;
		fullLibName += std::string(" (") + (beautify ? "beautify" : "minify") + ")";
		const std::string pureFileName = extractFileName(fullFileName);

		dumpDataList.add({
			pureFileName,
			fullLibName,
			fileSize,
			time,
			iterations
		});
	}

private:
	void dumpParserMarkdownTables() {
		metrics::table_markdown table;
		table.add_column_left("File name")
			.add_column_right("File size")
		;
		for(const auto & item : parseDataList.libList) {
			table.add_column_right(item);
		}

		for(const auto & fileName : parseDataList.fileList) {
			table << fileName << sizeToStorage(parseDataList.findData(fileName, parseDataList.libList[0])->fileSize);
			for(const auto & libName : parseDataList.libList) {
				const auto parseData = parseDataList.findData(fileName, libName);
				table << formatCell(parseData);
			}
		}

		table.print(std::cout, false);
	}

	void dumpDumpMarkdownTables() {
		metrics::table_markdown table;
		table.add_column_left("File name")
		;
		for(const auto & item : dumpDataList.libList) {
			table.add_column_right(item);
		}

		for(const auto & fileName : dumpDataList.fileList) {
			table << fileName;
			for(const auto & libName : dumpDataList.libList) {
				const auto dumpData = dumpDataList.findData(fileName, libName);
				table << formatCell(dumpData);
			}
		}

		table.print(std::cout, false);
	}

	std::string formatCell(const BenchmarkData * data) {
		const std::string timeText = formatSingleTime(data->time, data->iterations) + " ms";
		const uint64_t tps = data->fileSize * data->iterations * 1000 / data->time;
		const std::string tpsText = sizeToStorage(tps) + "/s";
		return ensureTextWidth(timeText, 10) + ", " + ensureTextWidth(tpsText, 9);
	}

private:
	BenchmarkDataList parseDataList;
	BenchmarkDataList dumpDataList;
};

BenchmarkDataCollection benchmarkDataCollection;

template <jsonpp::ParserBackendType backendType>
metapp::Variant doBenchmarkParseFile(const FileInfo & fileInfo)
{
	const std::string & fullFileName = fileInfo.fileName;
	const int iterations = fileInfo.iterations;

	std::string jsonText = readFile(fullFileName);
	if(jsonText.empty()) {
		return metapp::Variant();
	}

	const auto fileSize = jsonText.size();
	jsonpp::Parser parser(jsonpp::ParserConfig().setBackendType<backendType>());
	auto source = jsonpp::ParserSource(std::move(jsonText));
	REQUIRE(jsonText.empty());

	metapp::Variant result;
	const auto t = measureElapsedTime([&parser, iterations, &source, &result]() {
		for(int i = 0; i < iterations; ++i) {
			result = parser.parse(source);
		}
	});

	const std::string pureFileName = extractFileName(fullFileName);
	printTps(t, iterations, fileSize, jsonpp::getParserBackendName(backendType) + " Parse file " + pureFileName);
	
	benchmarkDataCollection.addParseResult(pureFileName, "jsonpp", backendType, fileSize, t, iterations);

	return result;
}

void doBenchmarkDumpJson(const metapp::Variant & parsedObject, const FileInfo & fileInfo, const bool beautify)
{
	const std::string & fullFileName = fileInfo.fileName;
	const int iterations = fileInfo.iterations;

	if(parsedObject.isEmpty()) {
		return;
	}

	std::string dumpedText;
	const auto t = measureElapsedTime([&parsedObject, iterations, beautify, &dumpedText]() {
		jsonpp::Dumper dumper(jsonpp::DumperConfig().enableBeautify(beautify));
		for(int i = 0; i < iterations; ++i) {
			dumpedText = dumper.dump(parsedObject);
		}
	});

	//metapp::Variant newVar = jsonpp::Parser().parse(dumpedText);
	//REQUIRE(! newVar.isEmpty());

	const std::string pureFileName = extractFileName(fullFileName);
	printTps(t, iterations, dumpedText.size(), std::string(beautify ? "Beautify" : "Minify") + " Dump file " + pureFileName);

	benchmarkDataCollection.addDumpResult(pureFileName, "jsonpp", beautify, dumpedText.size(), t, iterations);
}

BenchmarkFunc
{
	std::cout << std::endl;

	for(const auto & fileInfo : fileInfoList) {
		metapp::Variant parsedObject = doBenchmarkParseFile<jsonpp::ParserBackendType::simdjson>(fileInfo);
		doBenchmarkParseFile<jsonpp::ParserBackendType::cparser>(fileInfo);

		doBenchmarkDumpJson(parsedObject, fileInfo, true);
		doBenchmarkDumpJson(parsedObject, fileInfo, false);
		std::cout << std::endl;
	}
}

BenchmarkFunc
{
	std::cout << std::endl;

	for(const auto & fileInfo : fileInfoList) {
		const std::string & fullFileName = fileInfo.fileName;
		const int iterations = fileInfo.iterations;

		const std::string jsonText = readFile(fullFileName);
		if(jsonText.empty()) {
			return;
		}

		simdjson::dom::parser parser;
		simdjson::padded_string json(jsonText.c_str(), jsonText.size());
		const auto t1 = measureElapsedTime([iterations, &json, &parser]() {
			for(int i = 0; i < iterations; ++i) {
				simdjson::dom::element element;
				auto r = parser.parse(json).get(element);
				(void)r;
			}
		});

		const std::string pureFileName = extractFileName(fullFileName);
		printTps(t1, iterations, jsonText.size(), "simdjson DOM Parse file " + pureFileName);
	}
}

#ifdef NLOH
BenchmarkFunc
{
	std::cout << std::endl;

	for(const auto & fileInfo : fileInfoList) {
		const std::string & fullFileName = fileInfo.fileName;
		const int iterations = fileInfo.iterations;

		const std::string jsonText = readFile(fullFileName);
		if(jsonText.empty()) {
			return;
		}

		const std::string pureFileName = extractFileName(fullFileName);

		nlohmann::basic_json parsedObject;

		const auto t1 = measureElapsedTime([&parsedObject, iterations, jsonText]() {
			for(int i = 0; i < iterations; ++i) {
				parsedObject = nlohmann::json::parse(jsonText);
			}
		});

		printTps(t1, iterations, jsonText.size(), "nlo Parse file " + pureFileName);

		benchmarkDataCollection.addParseResult(pureFileName, "nlohmann", noBackend, jsonText.size(), t1, iterations);

		std::string dumpedText;
		const auto t2 = measureElapsedTime([iterations, &parsedObject, &dumpedText]() {
			for(int i = 0; i < iterations; ++i) {
				dumpedText = parsedObject.dump(4);
			}
		});
		printTps(t2, iterations, dumpedText.size(), "nlo Dump file beautify " + pureFileName);

		benchmarkDataCollection.addDumpResult(pureFileName, "nlohmann", true, dumpedText.size(), t2, iterations);

		const auto t3 = measureElapsedTime([iterations, &parsedObject, &dumpedText]() {
			for(int i = 0; i < iterations; ++i) {
				dumpedText = parsedObject.dump();
			}
		});
		printTps(t3, iterations, dumpedText.size(), "nlo Dump file minify " + pureFileName);

		benchmarkDataCollection.addDumpResult(pureFileName, "nlohmann", false, dumpedText.size(), t3, iterations);
	}
}
#endif

BenchmarkFunc
{
	benchmarkDataCollection.dumpMarkdownTables();
}

} //namespace
