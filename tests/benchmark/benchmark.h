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

#ifndef BENCHMARK_H
#define BENCHMARK_H

#include "metapp/compiler.h"

#include "../catch.hpp"

#include <chrono>
#include <iostream>
#include <locale>
#include <string>
#include <fstream>
#include <sstream>
#include <array>

constexpr int generalIterations = 10 * 1000 * 1000;

template <typename F>
uint64_t measureElapsedTime(F f)
{
	std::chrono::steady_clock::time_point t = std::chrono::steady_clock::now();
	f();
	return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - t).count();
}

template <typename T>
inline std::string intToString(const T n)
{
	std::string text = std::to_string(n);
	std::string result;
	int count = 0;
	for(int i = (int)text.size() - 1; i >= 0; --i) {
		if(count > 0 && count % 3 == 0) {
			result.push_back(',');
		}
		result.push_back(text[i]);
		++count;
	}
	std::reverse(result.begin(), result.end());
	return result;
}

inline std::string sizeToStorage(const uint64_t size)
{
	struct Item {
		uint64_t size;
		const char * postfix;
	};
	static std::array<Item, 4> itemList {{
		{ 1024 * 1024 * 1024, "GB" },
		{ 1024 * 1024, "MB" },
		{ 1024, "KB" },
		{ 0, "B" },
	}};

	for(const auto & item : itemList) {
		if(size >= item.size * 10) {
			auto n = item.size;
			if(n == 0) {
				n = 1;
			}
			return intToString(size / n) + " " + item.postfix;
		}
	}

	return "N/A";
}

inline void printResult(const uint64_t time, const int iterations, const std::string & message)
{
	std::cout
		<< message
		<< ": "
		<< intToString(time) << " ms "
		<< intToString(iterations) << " times"
		<< std::endl
	;
}

inline void printTps(uint64_t time, const int iterations, const uint64_t size, const std::string & message)
{
	if(time == 0) {
		time = 1;
	}
	const uint64_t tps = size * iterations * 1000 / time;
	std::cout
		<< message
		<< ": "
		<< intToString(time) << " ms "
		<< intToString(iterations) << " times"
		<< " FileSize: " << sizeToStorage(size)
		<< " TPS: " << sizeToStorage(tps) << " per second"
		<< std::endl
	;
}

#define I_UFN_STRINGIZE(s) #s
#define UFN_STRINGIZE(s) I_UFN_STRINGIZE(s)
#define I_UFN_CONCAT(a, b) a ## b
#define UFN_CONCAT(a, b) I_UFN_CONCAT(a, b)
#define BenchmarkFunc TEST_CASE(UFN_STRINGIZE(__FILE__) UFN_STRINGIZE(__LINE__))

#if defined(METAPP_COMPILER_GCC) || defined(METAPP_COMPILER_CLANG)
template <typename T>
void dontOptimizeAway(T && value)
{
	__asm__ __volatile__("" :: "m" (value));
}
#else
template <typename T>
void dontOptimizeAway(T && )
{
	// We don't benchmark in MSVC
}
#endif

inline std::string readFile(const std::string & fileName)
{
	std::ifstream f(fileName);
	if(! f) {
		std::cerr << fileName << " doesn't exist or can't be read." << std::endl;
		return "";
	}
	std::ostringstream ss;
	ss << f.rdbuf();
	return ss.str();
}

#define PARSER_TYPES() GENERATE(jsonpp::ParserBackendType::simdjson, jsonpp::ParserBackendType::cparser)

#endif
