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

#ifndef JSONPP_PARSERBACKEND_H_821598293712
#define JSONPP_PARSERBACKEND_H_821598293712

#include "metapp/variant.h"
#include "metapp/allmetatypes.h"
#include "metapp/interfaces/metaclass.h"
#include "metapp/interfaces/metaindexable.h"
#include "metapp/interfaces/metamappable.h"
#include "metapp/compiler.h"

#include <memory>
#include <ostream>

namespace jsonpp {

class ParserBackend
{
public:
	virtual ~ParserBackend() {}

	virtual bool hasError() const = 0;
	virtual std::string getError() const = 0;

	virtual metapp::Variant parse(const JsonParserSource & source, const metapp::MetaType * proto) = 0;

	virtual void prepareSource(const JsonParserSource & /*source*/) const {
	}

private:
};

template <typename Implement>
class GeneralParser
{
private:
	using ArrayValue = typename Implement::ArrayValue;
	using ObjectValue = typename Implement::ObjectValue;
	using Array = typename Implement::Array;
	using Object = typename Implement::Object;

public:
	GeneralParser(const ParserConfig & config, const Implement & implement)
		: config(config), implement(implement)
	{}

	template <typename T>
	metapp::Variant parse(T && node, const metapp::MetaType * proto)
	{
		if(proto != nullptr) {
			proto = metapp::getNonReferenceMetaType(proto);
			if(proto->getTypeKind() == metapp::tkVariant) {
				proto = nullptr;
			}
		}

		switch(implement.getNodeType(std::forward<T>(node))) {
		case Implement::typeNull:
			return metapp::Variant(nullptr);

		case Implement::typeBoolean:
			return (JsonBool)(implement.getBoolean(std::forward<T>(node)));

		case Implement::typeInteger:
			return (JsonInt)(implement.getInteger(std::forward<T>(node)));

		case Implement::typeUnsignedInteger:
			return (JsonInt)(implement.getUnsignedInteger(std::forward<T>(node)));

		case Implement::typeDouble:
			return (JsonReal)(implement.getDouble(std::forward<T>(node)));

		case Implement::typeString:
			return JsonString(implement.getString(std::forward<T>(node)));

		case Implement::typeArray:
			return doConvertArray(std::forward<T>(node), proto);

		case Implement::typeObject:
			return doConvertObject(std::forward<T>(node), proto);

		default:
			break;
		}
		return metapp::Variant();
	}

private:
	template <typename T>
	metapp::Variant doConvertArray(T && node, const metapp::MetaType * proto)
	{
		const metapp::MetaType * type = proto;
		if(type == nullptr) {
			type = config.getArrayType();
		}
		if(type == nullptr) {
			type = metapp::getMetaType<JsonArray>();
		}
		metapp::Variant result = metapp::Variant(type, nullptr);
		auto metaIndexable = metapp::getNonReferenceMetaType(result)->getMetaIndexable();
		Array array = implement.getArray(std::forward<T>(node));
		metaIndexable->resize(result, implement.getArraySize(array));
		implement.iterateArray(
			array,
			[this, metaIndexable, proto, &result](const std::size_t index, ArrayValue arrayValue) -> void {
				const metapp::MetaType * elementProto = nullptr;
				if(proto != nullptr) {
					elementProto = metapp::getNonReferenceMetaType(metaIndexable->getValueType(result, index));
				}
				metaIndexable->set(result, index, parse(arrayValue, elementProto));
			}
		);
		return result;
	}

	template <typename T>
	metapp::Variant doConvertObject(T && node, const metapp::MetaType * proto)
	{
		const metapp::MetaType * type = proto;
		const metapp::MetaClass * metaClass = nullptr;
		if(proto != nullptr) {
			metaClass = proto->getMetaClass();
		}
		if(type == nullptr) {
			type = config.getObjectType();
		}
		if(type == nullptr) {
			type = metapp::getMetaType<JsonObject>();
		}
		const metapp::MetaMappable * metaMappable = type->getMetaMappable();
		const metapp::MetaIndexable * metaIndexable = type->getMetaIndexable();
		metapp::Variant result = metapp::Variant(type, nullptr);
		Object object = implement.getObject(std::forward<T>(node));

		if(metaMappable != nullptr) {
			auto valueType = metaMappable->getValueType(result);
			implement.iterateObject(
				object,
				[this, &result, metaMappable, valueType](const std::string & key, ObjectValue objectValue) -> void {
					const metapp::Variant value(parse(objectValue, valueType->getUpType(1)));
					metaMappable->set(result, key, value);
				}
			);
		}
		else if(metaIndexable != nullptr) {
			metaIndexable->resize(result, implement.getObjectSize(object));
			std::size_t index = 0;
			implement.iterateObject(
				object,
				[this, &index, &result, metaIndexable](const std::string & key, ObjectValue objectValue) -> void {
					const auto value = metaIndexable->get(result, index);
					auto valueIndexable = metapp::getNonReferenceMetaType(value)->getMetaIndexable();
					if(valueIndexable != nullptr) {
						valueIndexable->resize(value, 2);
						valueIndexable->set(value, 0, key);
						const metapp::Variant convertedValue(parse(objectValue, valueIndexable->getValueType(value, 1)));
						valueIndexable->set(convertedValue, 1, value);
					}
					++index;
				}
			);
		}
		else if(metaClass != nullptr) {
			implement.iterateObject(
				object,
				[this, &result, metaClass](const std::string & key, ObjectValue objectValue) -> void {
					auto field = metaClass->getAccessible(key);
					if(! field.isEmpty()) {
						const metapp::Variant value(parse(objectValue, metapp::accessibleGetValueType(field)));
						metapp::accessibleSet(field, result.getAddress(), value);
					}
				}
			);
		}
		return result;
	}

private:
	const ParserConfig & config;
	Implement implement;
};


} // namespace jsonpp

#endif
