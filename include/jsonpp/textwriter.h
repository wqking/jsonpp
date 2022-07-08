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

#ifndef JSONPP_TEXTWRITER_H_821598293712
#define JSONPP_TEXTWRITER_H_821598293712

#include "implement/algorithms_i.h"
#include "drachennest/dragonbox.h"
#include "jsonpp/jsondumper.h"

#include <memory>
#include <vector>
#include <ostream>
#include <sstream>

namespace jsonpp {

struct StreamOutputter
{
	StreamOutputter(std::ostream & stream)
		: stream(stream)
	{
	}

	void operator() (const char c) const {
		stream.put(c);
	}

	void operator() (const char * s, const std::size_t length) const {
		stream.write(s, static_cast<std::streamsize>(length));
	}

	std::ostream & stream;
};

struct StringOutputter
{
	StringOutputter()
		: str()
	{
		str.reserve(1024 * 16);
	}

	void operator() (const char c) const {
		str.push_back(c);
	}

	void operator() (const char * s, const std::size_t length) const {
		str.append(s, length);
	}

	std::string && getString() const {
		return std::move(str);
	}

	mutable std::string str;
};

struct VectorOutputter
{
	VectorOutputter()
		: charList()
	{
		charList.reserve(1024 * 16);
	}

	void operator() (const char c) const {
		charList.push_back(c);
	}

	void operator() (const char * s, const std::size_t length) const {
		const auto size = charList.size();
		charList.resize(size + length);
		memmove(charList.data() + size, s, length);
	}

	const char * getString() const {
		return charList.data();
	}

	std::size_t getLength() const {
		return charList.size();
	}

	mutable std::vector<char> charList;
};

constexpr int numberToStringBufferSize = 64;

struct NumberToStringResult
{
	char * start;
	int length;
};

template <typename T>
NumberToStringResult integerToString(const T value, char * buffer)
{
	const int length = internal_::IntToString<T>::toString(value, &buffer[numberToStringBufferSize - 1]);
	return {
		&buffer[numberToStringBufferSize - length],
		length
	};
}

inline NumberToStringResult doubleToString(const double value, char * buffer)
{
	const char * end = dragonbox::Dtoa(buffer, value);
	return {
		buffer,
		static_cast<int>(end - buffer)
	};
}

struct EscapeItem {
	const char * str;
	std::size_t length;
};
constexpr char escapeItemListSize = 32;
extern const std::array<EscapeItem, escapeItemListSize> escapeItemList;
extern const EscapeItem escapeItemQuoteMark;
extern const EscapeItem escapeItemBackSlash;

template <typename Outputter>
struct TextWriter
{
	TextWriter(const DumperConfig & config, const Outputter & outputter)
		:
		config(config),
		outputter(outputter),
		indentLevel(0),
		indentList(),
		buffer()
	{
	}
	
	TextWriter(const DumperConfig & config, Outputter && outputter) = delete;

	const Outputter & getOutputter() const {
		return outputter;
	}

	void writeNull() const {
		outputter("null", 4);
	}

	void writeBoolean(const bool value) const {
		if(value) {
			outputter("true", 4);
		}
		else {
			outputter("false", 5);
		}
	}

	void writeNumber(const int64_t value) const {
		const auto result = integerToString(value, buffer.data());
		outputter(result.start, result.length);
	}

	void writeNumber(const uint64_t value) const {
		const auto result = integerToString(value, buffer.data());
		outputter(result.start, result.length);
	}

	void writeNumber(const double value) const {
		const auto result = doubleToString(value, buffer.data());
		outputter(result.start, result.length);
	}

	void writeString(const std::string & s) const {
		outputter('"');

		std::size_t previousIndex = 0;
		std::size_t index = 0;
		auto flush = [this, &s, &previousIndex, &index]() {
			if(previousIndex < index) {
				outputter(s.c_str() + previousIndex, index - previousIndex);
				previousIndex = index;
			}
		};

		while(index < s.size()) {
			const char c = s[index];
			const EscapeItem * escapeItem = nullptr;
			if(c >= 0 && c < escapeItemListSize) {
				escapeItem = &escapeItemList[c];
			}
			else if(c == '"') {
				escapeItem = &escapeItemQuoteMark;
			}
			else if(c == '\\') {
				escapeItem = &escapeItemBackSlash;
			}
			if(escapeItem != nullptr) {
				flush();
				outputter(escapeItem->str, escapeItem->length);
				++previousIndex;
			}
			++index;
		}

		flush();

		outputter('"');
	}

	void beginArray() const {
		outputter('[');
		writeLineBreak();
		increaseIndent();
	}

	void endArray() const {
		writeLineBreak();
		decreaseIndent();
		writeIndent();
		outputter(']');
	}

	void beginObject() const {
		outputter('{');
		writeLineBreak();
		increaseIndent();
	}

	void endObject() const {
		writeLineBreak();
		decreaseIndent();
		writeIndent();
		outputter('}');
	}

	void beginObjectItem(const std::string & key, const std::size_t index) const {
		checkWriteComma(index);
		writeIndent();

		writeString(key);
		outputter(':');
		writeSpace();
	}

	void endObjectItem() const {
	}

	void beginArrayItem(const std::size_t index) const {
		checkWriteComma(index);
		writeIndent();
	}

	void endArrayItem() const {
	}

private:
	void checkWriteComma(const std::size_t index) const {
		if(index > 0) {
			outputter(',');
			writeLineBreak();
		}
	}

	void writeSpace() const {
		if(! config.allowBeautify()) {
			return;
		}
		outputter(' ');
	}

	void writeLineBreak() const {
		if(! config.allowBeautify()) {
			return;
		}
		outputter('\n');
	}

	void increaseIndent() const {
		++indentLevel;
	}

	void decreaseIndent() const {
		assert(indentLevel > 0);
		--indentLevel;
	}

	void writeIndent() const {
		if(! config.allowBeautify()) {
			return;
		}
		if(indentList.size() <= indentLevel) {
			indentList.resize(indentLevel + 4);
		}
		std::string & indent = indentList[indentLevel];
		if(indent.empty()) {
			indent.resize(indentLevel * 4, ' ');
		}
		outputter(indent.c_str(), indent.size());
	}

private:
	DumperConfig config;
	const Outputter & outputter;
	mutable std::size_t indentLevel;
	mutable std::vector<std::string> indentList;
	mutable std::array<char, 128> buffer;
};


} // namespace jsonpp


#endif
