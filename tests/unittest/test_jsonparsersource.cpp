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
#define private public
#include "jsonpp/parser.h"

TEST_CASE("ParserSource, empty")
{
	SECTION("storage type") {
		REQUIRE(jsonpp::ParserSource().storageType == jsonpp::ParserSource::StorageType::string);
	}
}

TEST_CASE("ParserSource, c string")
{
	SECTION("storage type") {
		const char * s = "a";
		REQUIRE(jsonpp::ParserSource(s, 1).storageType == jsonpp::ParserSource::StorageType::cstr);
	}

	SECTION("attributes") {
		const char * s = "abcdef";
		const auto len = 6;
		auto source = jsonpp::ParserSource(s, len);
		REQUIRE(source.storageType == jsonpp::ParserSource::StorageType::cstr);
		REQUIRE(source.getText() == s);
		REQUIRE(source.getTextLength() == len);
		REQUIRE(source.getCapacity() == len);
	}

	SECTION("pad") {
		const char * s = "abcdef";
		const auto len = 6;
		auto source = jsonpp::ParserSource(s, len);
		REQUIRE(source.storageType == jsonpp::ParserSource::StorageType::cstr);
		source.pad(32);
		REQUIRE(source.storageType == jsonpp::ParserSource::StorageType::string);
		REQUIRE(source.getText() == &source.str[0]);
		REQUIRE(source.str == s);
		REQUIRE(source.getTextLength() == len);
		// The capacity may be larger then pad size
		REQUIRE(source.getCapacity() >= len + 32);
	}
}

TEST_CASE("ParserSource, ref")
{
	SECTION("storage type") {
		const std::string s;
		REQUIRE(jsonpp::ParserSource(s).storageType == jsonpp::ParserSource::StorageType::ref);
	}

	SECTION("attributes") {
		const std::string s = "abcdef";
		auto source = jsonpp::ParserSource(s);
		REQUIRE(source.storageType == jsonpp::ParserSource::StorageType::ref);
		REQUIRE(source.getText() == &s[0]);
		REQUIRE(source.getTextLength() == s.size());
		REQUIRE(source.getCapacity() >= s.size());
	}

	SECTION("pad") {
		const std::string s = "abcdef";
		auto source = jsonpp::ParserSource(s);
		REQUIRE(source.storageType == jsonpp::ParserSource::StorageType::ref);
		source.pad(32);
		REQUIRE(source.storageType == jsonpp::ParserSource::StorageType::string);
		REQUIRE(source.getText() == &source.str[0]);
		REQUIRE(source.str == s);
		REQUIRE(source.getTextLength() == s.size());
		REQUIRE(source.getCapacity() >= s.size() + 32);
	}
}

TEST_CASE("ParserSource, std::string")
{
	SECTION("storage type") {
		REQUIRE(jsonpp::ParserSource(std::string()).storageType == jsonpp::ParserSource::StorageType::string);
		std::string s;
		REQUIRE(jsonpp::ParserSource(std::move(s)).storageType == jsonpp::ParserSource::StorageType::string);
	}

	SECTION("attributes") {
		auto source = jsonpp::ParserSource(std::string("abcdef"));
		REQUIRE(source.storageType == jsonpp::ParserSource::StorageType::string);
		REQUIRE(source.getText() == &source.str[0]);
		REQUIRE(source.getTextLength() == 6);
		REQUIRE(source.getCapacity() >= 6);
	}

	SECTION("pad") {
		auto source = jsonpp::ParserSource(std::string("abcdef"));
		REQUIRE(source.storageType == jsonpp::ParserSource::StorageType::string);
		source.pad(32);
		REQUIRE(source.getText() == &source.str[0]);
		REQUIRE(source.getTextLength() == 6);
		REQUIRE(source.getCapacity() >= 38);
	}
}

