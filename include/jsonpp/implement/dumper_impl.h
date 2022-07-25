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

#ifndef JSONPP_DUMPER_IMPL_H_821598293712
#define JSONPP_DUMPER_IMPL_H_821598293712

#include "metapp/variant.h"

#include <memory>
#include <ostream>
#include <cstdio>
#include <cstring>

namespace jsonpp {

namespace internal_ {

template <typename Output>
class DumperImplement
{
public:
	DumperImplement(const DumperConfig & config, const Output & output)
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
		if(metaType->isPointer()) {
			if(value.get<void *>() == nullptr) {
				output.writeNull();
				return;
			}
			if(metaType->getUpType()->getTypeKind() == metapp::tkChar) {
				output.writeString(value.get<const char *>());
				return;
			}
		}

		const auto typeKind = metaType->getTypeKind();

		if(typeKind == metapp::tkVariant) {
			doDumpValue(value.get<metapp::Variant &>());
			return;
		}
		if(typeKind == metapp::tkStdString) {
			doDumpString(value.get<const std::string &>());
			return;
		}
		if(metaType->isArray() && metaType->getUpType()->getTypeKind() == metapp::tkChar) {
			output.writeString(value.get<char []>());
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
			output.writeNumber(value.cast<JsonReal>().template get<JsonReal>());
			return;
		}
		if(doDumpObject(value)) {
			return;
		}
		if(doDumpArray(value)) {
			return;
		}
		if(metaType->isEnum()) {
			const auto enumValue = value.cast<JsonInt>().template get<JsonInt>();
			if(config.allowNamedEnum() && metaType->hasMetaEnum()) {
				const metapp::MetaEnum * metaEnum = metaType->getMetaEnum();
				const metapp::MetaItem & metaItem = metaEnum->getByValue(enumValue);
				if(! metaItem.isEmpty()) {
					doDumpString(metaItem.getName());
					return;
				}
			}
			output.writeNumber(enumValue);
			return;
		}
	}

	void doDumpString(const std::string & s) {
		output.writeString(s.c_str(), s.size());
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
		const metapp::MetaIndexable * metaIndexable = nullptr;
		if(config.isObjectType(metaType)) {
			metaIndexable = metaType->getMetaIndexable();
		}
		const metapp::MetaClass * metaClass = nullptr;

		if(metaMappable != nullptr) {
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

		output.beginObject();

		if(as == asMap) {
			std::size_t index = 0;
			metaMappable->forEach(value, [this, &index](const metapp::Variant & key, const metapp::Variant & mapped) -> bool {
				output.beginObjectItem(key.cast<std::string>().template get<std::string>(), index++);
				doDumpValue(mapped);
				output.endObjectItem();

				return true;
			});
		}
		else if(as == asIndexable) {
			const std::size_t size = metaIndexable->getSizeInfo(value).getSize();
			for(std::size_t i = 0; i < size; ++i) {
				const metapp::Variant item = metaIndexable->get(value, i);

				auto indexable = metapp::getNonReferenceMetaType(item)->getMetaIndexable();
				if(indexable == nullptr) {
					return true;
				}
				output.beginObjectItem(indexable->get(item, 0).template cast<std::string>().template get<std::string>(), i);
				doDumpValue(indexable->get(item, 1));
				output.endObjectItem();
			}
		}
		else {
			const auto fieldView = metaClass->getAccessibleView();
			std::size_t index = 0;
			for(const auto & field : fieldView) {
				output.beginObjectItem(field.getName(), index++);
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
