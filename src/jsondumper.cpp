// metapp library
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

#include "jsonpp/jsondumper.h"
#include "metapp/allmetatypes.h"
#include "metapp/interfaces/metaclass.h"
#include "metapp/interfaces/metaindexable.h"
#include "metapp/interfaces/metamappable.h"
#include "metapp/compiler.h"

#include <sstream>
#include <vector>

namespace jsonpp {

namespace jsondumper_internal_ {

class Implement
{
public:
	explicit Implement(const DumperConfig & config);
	~Implement();

	void dump(const metapp::Variant & value, std::ostream & stream_);

private:
	void doDumpValue(const metapp::Variant & value, const size_t level);

	void doDumpString(const std::string & s);
	bool doDumpObject(const metapp::Variant & value, const size_t level);
	bool doDumpArray(const metapp::Variant & value, const size_t level);
		
	void doDumpSpace();
	void doDumpLineBreak();
	void doDumpIndent(const size_t level);

	std::ostream & getStream() const {
		return *stream;
	}

private:
	DumperConfig config;
	std::ostream * stream;
	std::vector<std::string> indentList;
};

Implement::Implement(const DumperConfig & config)
	: config(config)
{
}

Implement::~Implement()
{
}

void Implement::dump(const metapp::Variant & value, std::ostream & stream_)
{
	stream = &stream_;
	doDumpValue(value, 0);
	getStream().flush();
}

void Implement::doDumpValue(const metapp::Variant & value, const size_t level)
{
	auto metaType = metapp::getNonReferenceMetaType(value.getMetaType());
	if(metaType->isPointer() && value.get<void *>() == nullptr) {
		getStream() << "null";
		return;
	}

	auto typeKind = metaType->getTypeKind();
	if(typeKind == metapp::tkVariant) {
		doDumpValue(value.get<metapp::Variant &>(), level);
		return;
	}
	if(metapp::typeKindIsInteger(typeKind)) {
		getStream() << value.cast<long long>();
		return;
	}
	if(metapp::typeKindIsReal(typeKind)) {
		getStream() << value.cast<long double>();
		return;
	}
	{
		metapp::Variant casted = value.castSilently<std::string>();
		if(! casted.isEmpty()) {
			doDumpString(casted.get<std::string>());
		}
	}
	if(doDumpObject(value, level)) {
		return;
	}
	doDumpArray(value, level);
}

void Implement::doDumpString(const std::string & s)
{
	getStream() << '"' << s << '"';
}

bool Implement::doDumpObject(const metapp::Variant & value, const size_t level)
{
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
		auto indexable = metapp::getReferredMetaType(item.getMetaType())->getMetaIndexable();
		if(indexable == nullptr || indexable->getSize(item) < 2) {
			return true;
		}
		if(! firstItem) {
			getStream() << ",";
			doDumpLineBreak();
		}
		firstItem = false;
		doDumpIndent(level + 1);
		doDumpString(indexable->get(item, 0).cast<std::string>().get<std::string>());
		getStream() << ":";
		doDumpSpace();
		doDumpValue(indexable->get(item, 1), level + 1);

		return true;
	};

	getStream() << "{";
	doDumpLineBreak();

	if(as == asMap) {
		metaIterable->forEach(value, itemDumper);
	}
	else if(as == asIndexable) {
		const size_t size = metaIndexable->getSize(value);
		for(size_t i = 0; i < size; ++i) {
			const metapp::Variant item = metaIndexable->get(value, i);
			itemDumper(item);
		}
	}
	else {
		const auto fieldList = metaClass->getAccessibleList();
		for(const auto field : fieldList) {
			if(! firstItem) {
				getStream() << ",";
				doDumpLineBreak();
			}
			firstItem = false;
			doDumpIndent(level + 1);
			doDumpString(field.getName());
			getStream() << ":";
			doDumpSpace();
			doDumpValue(metapp::accessibleGet(field, value.getAddress()), level + 1);
		}
	}

	if(! firstItem) {
		doDumpLineBreak();
	}
	doDumpIndent(level);
	getStream() << "}";

	return true;
}

bool Implement::doDumpArray(const metapp::Variant & value, const size_t level)
{
	auto metaType = metapp::getNonReferenceMetaType(value.getMetaType());
	const metapp::MetaIterable * metaIterable = metaType->getMetaIterable();
	const metapp::MetaIndexable * metaIndexable = nullptr;
	if(metaIterable == nullptr) {
		metaIndexable = metaType->getMetaIndexable();
		if(metaIndexable == nullptr) {
			return false;
		}
	}
	getStream() << "[";
	doDumpLineBreak();
	bool firstItem = true;
	auto itemDumper = [this, &firstItem, level](const metapp::Variant & item) -> bool {
		if(! firstItem) {
			getStream() << ",";
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
		const size_t size = metaIndexable->getSize(value);
		for(size_t i = 0; i < size; ++i) {
			const metapp::Variant item = metaIndexable->get(value, i);
			itemDumper(item);
		}
	}
	if(! firstItem) {
		doDumpLineBreak();
	}
	doDumpIndent(level);
	getStream() << "]";
	return true;
}

void Implement::doDumpSpace()
{
	if(! config.allowBeautify()) {
		return;
	}
	getStream() << ' ';
}

void Implement::doDumpLineBreak()
{
	if(! config.allowBeautify()) {
		return;
	}
	// dont use std::endl, we don't want to flush frequently
	getStream() << '\n';
}

void Implement::doDumpIndent(const size_t level)
{
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
	getStream() << indent;
}

} // namespace jsondumper_internal_


JsonDumper::JsonDumper()
	: JsonDumper(DumperConfig())
{
}

JsonDumper::JsonDumper(const DumperConfig & config)
	: implement(new jsondumper_internal_::Implement(config))
{
}

JsonDumper::~JsonDumper()
{
}

void JsonDumper::dump(const metapp::Variant & value, std::ostream & stream)
{
	implement->dump(value, stream);
}

std::string JsonDumper::dump(const metapp::Variant & value)
{
	std::stringstream ss;
	dump(value, ss);
	return ss.str();
}

} // namespace jsonpp

