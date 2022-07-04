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

#ifndef JSONPP_JSONDUMPER_IMPL_H_821598293712
#define JSONPP_JSONDUMPER_IMPL_H_821598293712

#include "algorithms_i.h"

#include "metapp/variant.h"
#include "drachennest/dragonbox.h"

#include <memory>
#include <ostream>
#include <cstdio>
#include <cstring>

namespace jsonpp {

namespace internal_ {

extern std::string doubleFormatString;

template <typename Outputter>
class JsonDumperImplement
{
public:
	JsonDumperImplement(const DumperConfig & config, const Outputter & outputter)
		: config(config), outputter(outputter), indentList(), buffer()
	{
	}

	void dump(const metapp::Variant & value) {
		if(metapp::getNonReferenceMetaType(value)->isPointer() && value.get<void *>() == nullptr) {
			doDumpValue(value, 0);
		}
		else {
			doDumpValue(metapp::depointer(value), 0);
		}
	}

private:
	void doDumpValue(const metapp::Variant & value, const size_t level) {
		auto metaType = metapp::getNonReferenceMetaType(value);
		if(metaType->isPointer() && value.get<void *>() == nullptr) {
			outputter("null", 4);
			return;
		}

		auto typeKind = metaType->getTypeKind();
		if(typeKind == metapp::tkVariant) {
			doDumpValue(value.get<metapp::Variant &>(), level);
			return;
		}
		if(typeKind == metapp::tkBool) {
			if(value.get<bool>()) {
				outputter("true", 4);
			}
			else {
				outputter("false", 5);
			}
			return;
		}
		if(metapp::typeKindIsIntegral(typeKind)) {
			if(metapp::typeKindIsSignedIntegral(typeKind)) {
				using Type = long long;
				const auto n = value.cast<Type>().template get<Type>();
				const int length = internal_::IntToString<Type>::toString(n, &buffer[internal_::intToStringBufferSize - 1]);
				outputter(&buffer[internal_::intToStringBufferSize - length], length);
			}
			else {
				using Type = unsigned long long;
				const auto n = value.cast<Type>().template get<Type>();
				const int length = internal_::IntToString<Type>::toString(n, &buffer[internal_::intToStringBufferSize - 1]);
				outputter(&buffer[internal_::intToStringBufferSize - length], length);
			}
			return;
		}
		if(metapp::typeKindIsReal(typeKind)) {
#if 1
			const auto d = value.cast<double>().template get<double>();
			const char * end = dragonbox::Dtoa(buffer.data(), d);
			outputter(buffer.data(), end - buffer.data());
#else
			const auto len = std::snprintf(
				buffer.data(),
				buffer.size(),
				doubleFormatString.c_str(),
				value.cast<double>().template get<double>()
			);
			outputter(buffer.data(), len);
#endif
			return;
		}
		{
			metapp::Variant casted = value.castSilently<std::string>();
			if(! casted.isEmpty()) {
				doDumpString(casted.get<std::string>());
				return;
			}
		}
		if(doDumpObject(value, level)) {
			return;
		}
		doDumpArray(value, level);
	}

	void doDumpString(const std::string & s) {
		struct EscapeItem {
			const char * str;
			std::size_t length;
		};
		constexpr char escapeItemListSize = 32;
		static const std::array<EscapeItem, escapeItemListSize> escapeItemList {{
			{ "\\0", 2 }, // 0
			{ "\\u0001", 6 }, // 1
			{ "\\u0002", 6 }, // 2
			{ "\\u0003", 6 }, // 3
			{ "\\u0004", 6 }, // 4
			{ "\\u0005", 6 }, // 5
			{ "\\u0006", 6 }, // 6
			{ "\\u0007", 6 }, // 7
			{ "\\b", 2 }, // 8 \b
			{ "\\t", 2 }, // 9 \t
			{ "\\n", 2 }, // 10 \n
			{ "\\u000b", 6 }, // 11
			{ "\\f", 2 }, // 12 \f
			{ "\\r", 2 }, // 13 \r
			{ "\\u000e", 6 }, // 14
			{ "\\u000f", 6 }, // 15
			{ "\\u0010", 6 }, // 16
			{ "\\u0011", 6 }, // 17
			{ "\\u0012", 6 }, // 18
			{ "\\u0013", 6 }, // 19
			{ "\\u0014", 6 }, // 20
			{ "\\u0015", 6 }, // 21
			{ "\\u0016", 6 }, // 22
			{ "\\u0017", 6 }, // 23
			{ "\\u0018", 6 }, // 24
			{ "\\u0019", 6 }, // 25
			{ "\\u001a", 6 }, // 26
			{ "\\u001b", 6 }, // 27
			{ "\\u001c", 6 }, // 28
			{ "\\u001d", 6 }, // 29
			{ "\\u001e", 6 }, // 30
			{ "\\u001f", 6 }, // 31
		}};
		static const EscapeItem escapeItemQuoteMark = { "\\\"", 2 };
		static const EscapeItem escapeItemBackSlash = { "\\\\", 2 };

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

	bool doDumpObject(const metapp::Variant & value, const size_t level) {
		auto metaType = metapp::getNonReferenceMetaType(value.getMetaType());
		if(config.isArrayType(metaType)) {
			return false;
		}

		constexpr int asNone = 0;
		constexpr int asMap = 1;
		constexpr int asIndexable = 2;
		constexpr int asClass = 3;
		int as = asNone;

		const metapp::MetaMappable * metaMappable = metaType->getMetaMappable();
		const metapp::MetaIterable * metaIterable = metaType->getMetaIterable();
		const metapp::MetaIndexable * metaIndexable = nullptr;
		if(config.isObjectType(metaType)) {
			metaIndexable = metaType->getMetaIndexable();
		}
		const metapp::MetaClass * metaClass = nullptr;

		if(metaMappable != nullptr && metaIterable != nullptr) {
			as = asMap;
		}
		else if(metaIndexable != nullptr) {
			as = asIndexable;
		}
		else {
			metaClass = metaType->getMetaClass();
			if(metaClass != nullptr) {
				as = asClass;
			}
		}

		if(as == asNone) {
			return false;
		}

		bool firstItem = true;
		auto itemDumper = [this, &firstItem, level](const metapp::Variant & item) -> bool {
			auto indexable = metapp::getNonReferenceMetaType(item)->getMetaIndexable();
			if(indexable == nullptr || indexable->getSizeInfo(item).getSize() < 2) {
				return true;
			}
			if(! firstItem) {
				outputter(',');
				doDumpLineBreak();
			}
			firstItem = false;
			doDumpIndent(level + 1);
			doDumpString(indexable->get(item, 0).template cast<std::string>().template get<std::string>());
			outputter(':');
			doDumpSpace();
			doDumpValue(indexable->get(item, 1), level + 1);

			return true;
		};

		outputter('{');
		doDumpLineBreak();

		if(as == asMap) {
			metaIterable->forEach(value, itemDumper);
		}
		else if(as == asIndexable) {
			const size_t size = metaIndexable->getSizeInfo(value).getSize();
			for(size_t i = 0; i < size; ++i) {
				const metapp::Variant item = metaIndexable->get(value, i);
				itemDumper(item);
			}
		}
		else {
			const auto fieldView = metaClass->getAccessibleView();
			for(const auto & field : fieldView) {
				if(! firstItem) {
					outputter(',');
					doDumpLineBreak();
				}
				firstItem = false;
				doDumpIndent(level + 1);
				doDumpString(field.getName());
				outputter(':');
				doDumpSpace();
				doDumpValue(metapp::accessibleGet(field, value.getAddress()), level + 1);
			}
		}

		if(! firstItem) {
			doDumpLineBreak();
		}
		doDumpIndent(level);
		outputter('}');

		return true;
	}

	bool doDumpArray(const metapp::Variant & value, const size_t level) {
		auto metaType = metapp::getNonReferenceMetaType(value.getMetaType());
		const metapp::MetaIterable * metaIterable = metaType->getMetaIterable();
		const metapp::MetaIndexable * metaIndexable = nullptr;
		if(metaIterable == nullptr) {
			metaIndexable = metaType->getMetaIndexable();
			if(metaIndexable == nullptr) {
				return false;
			}
		}
		outputter('[');
		doDumpLineBreak();
		bool firstItem = true;
		auto itemDumper = [this, &firstItem, level](const metapp::Variant & item) -> bool {
			if(! firstItem) {
				outputter(',');
				doDumpLineBreak();
			}
			firstItem = false;
			doDumpIndent(level + 1);
			doDumpValue(item, level + 1);

			return true;
		};
		if(metaIterable != nullptr) {
			metaIterable->forEach(value, itemDumper);
		}
		else {
			const size_t size = metaIndexable->getSizeInfo(value).getSize();
			for(size_t i = 0; i < size; ++i) {
				const metapp::Variant item = metaIndexable->get(value, i);
				itemDumper(item);
			}
		}
		if(! firstItem) {
			doDumpLineBreak();
		}
		doDumpIndent(level);
		outputter(']');
		return true;
	}

	void doDumpSpace() {
		if(! config.allowBeautify()) {
			return;
		}
		outputter(' ');
	}

	void doDumpLineBreak() {
		if(! config.allowBeautify()) {
			return;
		}
		outputter('\n');
	}

	void doDumpIndent(const size_t level) {
		if(! config.allowBeautify()) {
			return;
		}
		if(indentList.size() <= level) {
			indentList.resize(level + 4);
		}
		std::string & indent = indentList[level];
		if(indent.empty()) {
			indent.resize(level * 4, ' ');
		}
		outputter(indent.c_str(), indent.size());
	}

private:
	DumperConfig config;
	const Outputter & outputter;
	std::vector<std::string> indentList;
	std::array<char, 128> buffer;
};


} // namespace internal_

} // namespace jsonpp

#endif
