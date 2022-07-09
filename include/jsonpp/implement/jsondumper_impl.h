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

#include "metapp/variant.h"

#include <memory>
#include <ostream>
#include <cstdio>
#include <cstring>

namespace jsonpp {

namespace internal_ {

template <typename Output>
class JsonDumperImplement
{
public:
	JsonDumperImplement(const DumperConfig & config, const Output & output)
		: config(config), output(output), indentList(), buffer()
	{
	}

	void dump(const metapp::Variant & value) {
		if(metapp::getNonReferenceMetaType(value)->isPointer() && value.get<void *>() == nullptr) {
			doDumpValue(value);
		}
		else {
			doDumpValue(metapp::depointer(value));
		}
	}

private:
	void doDumpValue(const metapp::Variant & value) {
		auto metaType = metapp::getNonReferenceMetaType(value);
		if(metaType->isPointer() && value.get<void *>() == nullptr) {
			output.writeNull();
			return;
		}

		auto typeKind = metaType->getTypeKind();
		if(typeKind == metapp::tkVariant) {
			doDumpValue(value.get<metapp::Variant &>());
			return;
		}
		if(typeKind == metapp::tkBool) {
			output.writeBoolean(value.get<bool>());
			return;
		}
		if(metapp::typeKindIsIntegral(typeKind)) {
			if(metapp::typeKindIsSignedIntegral(typeKind)) {
				using Type = JsonInt;
				const auto n = value.cast<Type>().template get<Type>();
				output.writeNumber(n);
			}
			else {
				using Type = JsonUnsignedInt;
				const auto n = value.cast<Type>().template get<Type>();
				output.writeNumber(n);
			}
			return;
		}
		if(metapp::typeKindIsReal(typeKind)) {
			const auto d = value.cast<double>().template get<double>();
			output.writeNumber(d);
			return;
		}
		{
			metapp::Variant casted = value.castSilently<std::string>();
			if(! casted.isEmpty()) {
				doDumpString(casted.get<std::string>());
				return;
			}
		}
		if(doDumpObject(value)) {
			return;
		}
		doDumpArray(value);
	}

	void doDumpString(const std::string & s) {
		output.writeString(s);
	}

	bool doDumpObject(const metapp::Variant & value) {
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

		std::size_t index = 0;
		auto itemDumper = [this, &index](const metapp::Variant & item) -> bool {
			auto indexable = metapp::getNonReferenceMetaType(item)->getMetaIndexable();
			if(indexable == nullptr || indexable->getSizeInfo(item).getSize() < 2) {
				return true;
			}
			output.beginObjectItem(indexable->get(item, 0).template cast<std::string>().template get<std::string>(), index++);
			doDumpValue(indexable->get(item, 1));
			output.endObjectItem();

			return true;
		};

		output.beginObject();

		if(as == asMap) {
			metaIterable->forEach(value, itemDumper);
		}
		else if(as == asIndexable) {
			const std::size_t size = metaIndexable->getSizeInfo(value).getSize();
			for(std::size_t i = 0; i < size; ++i) {
				const metapp::Variant item = metaIndexable->get(value, i);
				itemDumper(item);
			}
		}
		else {
			const auto fieldView = metaClass->getAccessibleView();
			std::size_t i = 0;
			for(const auto & field : fieldView) {
				output.beginObjectItem(field.getName(), i++);
				doDumpValue(metapp::accessibleGet(field, value.getAddress()));
				output.endObjectItem();
			}
		}

		output.endObject();

		return true;
	}

	bool doDumpArray(const metapp::Variant & value) {
		auto metaType = metapp::getNonReferenceMetaType(value.getMetaType());
		const metapp::MetaIterable * metaIterable = metaType->getMetaIterable();
		const metapp::MetaIndexable * metaIndexable = nullptr;
		if(metaIterable == nullptr) {
			metaIndexable = metaType->getMetaIndexable();
			if(metaIndexable == nullptr) {
				return false;
			}
		}
		output.beginArray();
		std::size_t index = 0;
		auto itemDumper = [this, &index](const metapp::Variant & item) -> bool {
			output.beginArrayItem(index++);
			doDumpValue(item);
			output.endArrayItem();

			return true;
		};
		if(metaIterable != nullptr) {
			metaIterable->forEach(value, itemDumper);
		}
		else {
			const std::size_t size = metaIndexable->getSizeInfo(value).getSize();
			for(std::size_t i = 0; i < size; ++i) {
				const metapp::Variant item = metaIndexable->get(value, i);
				itemDumper(item);
			}
		}
		output.endArray();
		return true;
	}

private:
	DumperConfig config;
	const Output & output;
	std::vector<std::string> indentList;
	std::array<char, 128> buffer;
};


} // namespace internal_

} // namespace jsonpp

#endif
