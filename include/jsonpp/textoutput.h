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

#ifndef JSONPP_TEXTOUTPUT_H_821598293712
#define JSONPP_TEXTOUTPUT_H_821598293712

#include "implement/algorithms_i.h"
#include "jsonpp/dumper.h"

#include <memory>
#include <vector>
#include <ostream>
#include <sstream>
#include <cstring>
#include <climits>

namespace dragonbox {

char* Dtoa(char* buffer, double value);

} // namespace dragonbox

namespace jsonpp {

struct StringWriter
{
	StringWriter()
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

	std::string && takeString() const {
		return std::move(str);
	}

	const std::string & getString() const {
		return str;
	}

private:
	mutable std::string str;
};

struct VectorWriter
{
	VectorWriter()
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

	std::vector<char> && takeVector() const {
		return std::move(charList);
	}

	const std::vector<char> & getVector() const {
		return charList;
	}

private:
	mutable std::vector<char> charList;
};

struct StreamWriter
{
	explicit StreamWriter(std::ostream & stream)
		: stream(stream)
	{
	}

	void operator() (const char c) const {
		stream.put(c);
	}

	void operator() (const char * s, const std::size_t length) const {
		stream.write(s, static_cast<std::streamsize>(length));
	}

	std::ostream & getStream() const {
		return stream;
	}

private:
	std::ostream & stream;
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
extern const uint8_t encodeCharMap[];
extern const EscapeItem escapeItemList[];

template <typename Writer>
struct TextOutput
{
	explicit TextOutput(const Writer & writer)
		: TextOutput(DumperConfig(), writer)
	{
	}

	TextOutput(const DumperConfig & config, const Writer & writer)
		:
			config(config),
			writer(writer),
			indentLevel(0),
			indentList(),
			buffer()
	{
	}
	
	TextOutput(Writer && writer) = delete;
	TextOutput(const DumperConfig & config, Writer && writer) = delete;

	void writeNull() const {
		writer("null", 4);
	}

	void writeBoolean(const bool value) const {
		if(value) {
			writer("true", 4);
		}
		else {
			writer("false", 5);
		}
	}

	void writeNumber(const JsonInt value) const {
		const auto result = integerToString(value, buffer.data());
		writer(result.start, result.length);
	}

	void writeNumber(const JsonUnsignedInt value) const {
		const auto result = integerToString(value, buffer.data());
		writer(result.start, result.length);
	}

	void writeNumber(const double value) const {
		const auto result = doubleToString(value, buffer.data());
		writer(result.start, result.length);
	}

	void writeString(const char * const s) const {
		writeString(s, noneLength);
	}

	void writeString(const char * const s, const std::size_t length) const {
		writer('"');

		std::size_t previousIndex = 0;
		std::size_t index = 0;
		auto flush = [this, s, &previousIndex, &index]() {
			if(previousIndex < index) {
				writer(s + previousIndex, index - previousIndex);
				previousIndex = index;
			}
		};

		while(index < length) {
			const unsigned char c = static_cast<unsigned char>(s[index]);
			if(encodeCharMap[c] != 0) {
				if(c == 0 && length == noneLength) {
					break;
				}
				const EscapeItem * escapeItem = &escapeItemList[encodeCharMap[c]];
				flush();
				writer(escapeItem->str, escapeItem->length);
				++previousIndex;
			}
			++index;
		}

		flush();

		writer('"');
	}

	void beginArray() const {
		writer('[');
		writeLineBreak();
		increaseIndent();
	}

	void endArray() const {
		writeLineBreak();
		decreaseIndent();
		writeIndent();
		writer(']');
	}

	void beginObject() const {
		writer('{');
		writeLineBreak();
		increaseIndent();
	}

	void endObject() const {
		writeLineBreak();
		decreaseIndent();
		writeIndent();
		writer('}');
	}

	void beginObjectItem(const std::string & key, const std::size_t index) const {
		checkWriteComma(index);
		writeIndent();

		writeString(key.c_str(), key.size());
		writer(':');
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
	static constexpr std::size_t noneLength = std::numeric_limits<std::size_t>::max();

	void checkWriteComma(const std::size_t index) const {
		if(index > 0) {
			writer(',');
			writeLineBreak();
		}
	}

	void writeSpace() const {
		if(! config.allowBeautify()) {
			return;
		}
		writer(' ');
	}

	void writeLineBreak() const {
		if(! config.allowBeautify()) {
			return;
		}
		writer('\n');
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
			for(std::size_t i = 0; i < indentLevel; ++i) {
				indent.append(config.getIndent());
			}
		}
		writer(indent.c_str(), indent.size());
	}

private:
	DumperConfig config;
	const Writer & writer;
	mutable std::size_t indentLevel;
	mutable std::vector<std::string> indentList;
	mutable std::array<char, 128> buffer;
};


} // namespace jsonpp


#endif
