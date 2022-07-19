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
#include "metapp/interfaces/metaenum.h"
#include "metapp/compiler.h"

#include <memory>
#include <ostream>

namespace jsonpp {

struct ParserBackendResult
{
	metapp::Variant value;
	std::string errorMessage;
};

class ParserBackend
{
public:
	virtual ~ParserBackend() {}

	virtual ParserBackendResult parse(const ParserSource & source, const metapp::MetaType * prototype) = 0;

	virtual void prepareSource(const ParserSource & /*source*/) const {
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
	metapp::Variant parse(T && node, const metapp::MetaType * prototype)
	{
		if(prototype != nullptr) {
			prototype = metapp::getNonReferenceMetaType(prototype);
			if(prototype->getTypeKind() == metapp::tkVariant) {
				prototype = nullptr;
			}
		}

		switch(implement.getNodeType(std::forward<T>(node))) {
		case Implement::typeNull:
			if(prototype != nullptr) {
				return metapp::Variant(nullptr).cast(prototype);
			}
			return metapp::Variant(nullptr);

		case Implement::typeBoolean:
			if(prototype != nullptr) {
				return metapp::Variant((JsonBool)(implement.getBoolean(std::forward<T>(node)))).cast(prototype);
			}
			return (JsonBool)(implement.getBoolean(std::forward<T>(node)));

		case Implement::typeInteger:
			if(prototype != nullptr) {
				return metapp::Variant((JsonInt)(implement.getInteger(std::forward<T>(node)))).cast(prototype);
			}
			return (JsonInt)(implement.getInteger(std::forward<T>(node)));

		case Implement::typeUnsignedInteger:
			if(prototype != nullptr) {
				return metapp::Variant((JsonUnsignedInt)(implement.getUnsignedInteger(std::forward<T>(node)))).cast(prototype);
			}
			return (JsonUnsignedInt)(implement.getUnsignedInteger(std::forward<T>(node)));

		case Implement::typeDouble:
			if(prototype != nullptr) {
				return metapp::Variant((JsonReal)(implement.getDouble(std::forward<T>(node)))).cast(prototype);
			}
			return (JsonReal)(implement.getDouble(std::forward<T>(node)));

		case Implement::typeString: {
			if(prototype != nullptr) {
				if(prototype->isEnum()) {
					const auto metaEnum = prototype->getMetaEnum();
					if(metaEnum != nullptr) {
						const auto & metaItem = metaEnum->getByName(JsonString(implement.getString(std::forward<T>(node))));
						metapp::Variant enumValue = 0;
						if(! metaItem.isEmpty()) {
							enumValue = metaItem.asEnumValue().cast<long long>();
						}
						// There is a dedicated item in metapp FAQ for this conversion.
						return enumValue.cast(prototype);
					}
					return metapp::Variant(prototype, nullptr);
				}
				return metapp::Variant(JsonString(implement.getString(std::forward<T>(node)))).cast(prototype);
			}
			return JsonString(implement.getString(std::forward<T>(node)));
		}

		case Implement::typeArray:
			return doConvertArray(std::forward<T>(node), prototype);

		case Implement::typeObject:
			return doConvertObject(std::forward<T>(node), prototype);

		default:
			break;
		}
		return metapp::Variant();
	}

private:
	template <typename T>
	metapp::Variant doConvertArray(T && node, const metapp::MetaType * prototype)
	{
		const metapp::MetaType * type = prototype;
		if(type == nullptr) {
			type = config.getArrayType();
		}
		if(type == nullptr) {
			// If there is no prototype, we parse array as the default JsonArray. Since we know the native data structure,
			// we operate on the data structure directly instead of using meta interface. This increases performance significantly.
			// Parsing canada.json speeds up more than 30%, citm_catalog.json and twitter.json more than 60%~70%.
			// The same for doConvertObject.
			Array array = implement.getArray(std::forward<T>(node));
			JsonArray result(implement.getArraySize(array));
			implement.iterateArray(
				array,
				[this, &result](const std::size_t index, ArrayValue arrayValue) -> void {
					result[index] = parse(arrayValue, nullptr);
				}
			);
			return result;
		}
		else {
			metapp::Variant result = metapp::Variant(type, nullptr);
			auto metaIndexable = metapp::getNonReferenceMetaType(result)->getMetaIndexable();
			Array array = implement.getArray(std::forward<T>(node));
			metaIndexable->resize(result, implement.getArraySize(array));
			implement.iterateArray(
				array,
				[this, metaIndexable, prototype, &result](const std::size_t index, ArrayValue arrayValue) -> void {
					const metapp::MetaType * elementProto = nullptr;
					if(prototype != nullptr) {
						elementProto = metapp::getNonReferenceMetaType(metaIndexable->getValueType(result, index));
					}
					metaIndexable->set(result, index, parse(arrayValue, elementProto));
				}
			);
			return result;
		}
	}

	template <typename T>
	metapp::Variant doConvertObject(T && node, const metapp::MetaType * prototype)
	{
		const metapp::MetaType * type = prototype;
		const metapp::MetaClass * metaClass = nullptr;
		if(prototype != nullptr) {
			metaClass = prototype->getMetaClass();
		}
		if(type == nullptr) {
			type = config.getObjectType();
		}
		if(type == nullptr) {
			Object object = implement.getObject(std::forward<T>(node));
			JsonObject result;
			implement.iterateObject(
				object,
				[this, &result](const std::string & key, ObjectValue objectValue) -> void {
					result.insert(std::make_pair(key, parse(objectValue, nullptr)));
				}
			);
			return result;
		}
		else {
			const metapp::MetaMappable * metaMappable = type->getMetaMappable();
			const metapp::MetaIndexable * metaIndexable = type->getMetaIndexable();
			metapp::Variant result = metapp::Variant(type, nullptr);
			Object object = implement.getObject(std::forward<T>(node));

			if(metaMappable != nullptr) {
				auto valueType = metaMappable->getValueType(result);
				implement.iterateObject(
					object,
					[this, &result, metaMappable, valueType](const std::string & key, ObjectValue objectValue) -> void {
						metaMappable->set(
							result,
							key,
							parse(objectValue, valueType->getUpType(1))
						);
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
							valueIndexable->set(
								value,
								1,
								parse(objectValue, valueIndexable->getValueType(value, 1))
							);
						}
						++index;
					}
				);
			}
			else if(metaClass != nullptr) {
				implement.iterateObject(
					object,
					[this, &result, metaClass](const std::string & key, ObjectValue objectValue) -> void {
						const auto & field = metaClass->getAccessible(key);
						if(! field.isEmpty()) {
							metapp::accessibleSet(
								field,
								result.getAddress(),
								parse(objectValue, metapp::accessibleGetValueType(field))
							);
						}
					}
				);
			}
			return result;
		}
	}

private:
	const ParserConfig & config;
	Implement implement;
};


} // namespace jsonpp

#endif
