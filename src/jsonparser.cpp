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

#include "jsonpp/jsonparser.h"
#include "metapp/allmetatypes.h"
#include "metapp/interfaces/metaclass.h"
#include "metapp/interfaces/metaindexable.h"
#include "metapp/interfaces/metamappable.h"
#include "metapp/compiler.h"

#if defined(METAPP_COMPILER_VC)
#pragma warning(push)
#pragma warning(disable: 4244 4996 4706)
#endif
#if defined(METAPP_COMPILER_GCC)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
//#pragma GCC diagnostic ignored "-Wunused-parameter"
#endif

#include "rawparser_i.inc"

#if defined(METAPP_COMPILER_GCC)
#pragma GCC diagnostic pop
#endif
#if defined(METAPP_COMPILER_VC)
#pragma warning(pop)
#endif

#include <array>

namespace jsonpp {

namespace jsonparser_internal_ {

class Implement
{
public:
	explicit Implement(const ParserConfig & config);
	~Implement();

	metapp::Variant parse(const char * jsonText, const size_t length, const metapp::MetaType * proto);

private:
	metapp::Variant doConvertValue(json_value * jsonValue, const metapp::MetaType * proto);

	metapp::Variant doConvertNull(json_value * jsonValue, const metapp::MetaType * proto);
	metapp::Variant doConvertBoolean(json_value * jsonValue, const metapp::MetaType * proto);
	metapp::Variant doConvertInteger(json_value * jsonValue, const metapp::MetaType * proto);
	metapp::Variant doConvertDouble(json_value * jsonValue, const metapp::MetaType * proto);
	metapp::Variant doConvertString(json_value * jsonValue, const metapp::MetaType * proto);
	metapp::Variant doConvertArray(json_value * jsonValue, const metapp::MetaType * proto);
	metapp::Variant doConvertObject(json_value * jsonValue, const metapp::MetaType * proto);

private:
	ParserConfig config;
	json_settings settings;
	json_value * root;
	std::array<char, json_error_max> error;
};

Implement::Implement(const ParserConfig & config)
	: config(config), settings(), root(nullptr), error()
{
	settings.settings |= json_enable_comments;
}

Implement::~Implement()
{
	if(root != nullptr) {
		json_value_free(root);
	}
}

metapp::Variant Implement::parse(const char * jsonText, const size_t length, const metapp::MetaType * proto)
{
	root = json_parse_ex(&settings, jsonText, length, error.data());
	if(error[0] != 0) {
		return metapp::Variant();
	}
	return doConvertValue(root, proto);
}

metapp::Variant Implement::doConvertValue(json_value * jsonValue, const metapp::MetaType * proto)
{
	if(proto != nullptr) {
		proto = metapp::getNonReferenceMetaType(proto);
	}

	switch(jsonValue->type) {
	case json_object:
		return doConvertObject(jsonValue, proto);

	case json_array:
		return doConvertArray(jsonValue, proto);

	case json_integer:
		return doConvertInteger(jsonValue, proto);

	case json_double:
		return doConvertDouble(jsonValue, proto);

	case json_string:
		return doConvertString(jsonValue, proto);

	case json_boolean:
		return doConvertBoolean(jsonValue, proto);

	case json_null:
		return doConvertNull(jsonValue, proto);

	default:
		break;
	}
	return metapp::Variant();
}

metapp::Variant Implement::doConvertNull(json_value * /*jsonValue*/, const metapp::MetaType * proto)
{
	const metapp::MetaType * type = proto;
	if(type == nullptr) {
		type = config.getNullType();
	}
	if(type == nullptr) {
		return metapp::Variant(nullptr);
	}
	return metapp::Variant(nullptr).cast(type);
}

metapp::Variant Implement::doConvertBoolean(json_value * jsonValue, const metapp::MetaType * proto)
{
	const metapp::MetaType * type = proto;
	if(type == nullptr) {
		type = config.getBooleanType();
	}
	if(type == nullptr) {
		return metapp::Variant((bool)(jsonValue->u.boolean));
	}
	return metapp::Variant((bool)(jsonValue->u.boolean)).cast(type);
}

metapp::Variant Implement::doConvertInteger(json_value * jsonValue, const metapp::MetaType * proto)
{
	const metapp::MetaType * type = proto;
	if(type == nullptr) {
		type = config.getIntegerType();
	}
	if(type == nullptr) {
		return metapp::Variant((long long)(jsonValue->u.integer));
	}
	return metapp::Variant((long long)(jsonValue->u.integer)).cast(type);
}

metapp::Variant Implement::doConvertDouble(json_value * jsonValue, const metapp::MetaType * proto)
{
	const metapp::MetaType * type = proto;
	if(type == nullptr) {
		type = config.getDoubleType();
	}
	if(type == nullptr) {
		return metapp::Variant((double)(jsonValue->u.dbl));
	}
	return metapp::Variant((double)(jsonValue->u.dbl)).cast(type);
}

metapp::Variant Implement::doConvertString(json_value * jsonValue, const metapp::MetaType * proto)
{
	const metapp::MetaType * type = proto;
	if(type == nullptr) {
		type = config.getStringType();
	}
	if(type == nullptr) {
		return metapp::Variant(std::string(jsonValue->u.string.ptr));
	}
	return metapp::Variant(jsonValue->u.string.ptr).cast(type);
}

metapp::Variant Implement::doConvertArray(json_value * jsonValue, const metapp::MetaType * proto)
{
	const metapp::MetaType * type = proto;
	if(type == nullptr) {
		type = config.getArrayType();
	}
	if(type == nullptr) {
		type = metapp::getMetaType<std::vector<metapp::Variant> >();
	}
	metapp::Variant result = metapp::Variant(type);
	auto metaIndexable = result.getMetaType()->getMetaIndexable();
	metaIndexable->resize(result, jsonValue->u.array.length);
	for(size_t i = 0; i < size_t(jsonValue->u.array.length); ++i) {
		const metapp::MetaType * elementProto = nullptr;
		if(proto != nullptr) {
			elementProto = metapp::getNonReferenceMetaType(metaIndexable->getValueType(result, i));
		}
		metaIndexable->set(result, i, doConvertValue(jsonValue->u.array.values[i], elementProto));
	}
	return result;
}

metapp::Variant Implement::doConvertObject(json_value * jsonValue, const metapp::MetaType * proto)
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
		type = metapp::getMetaType<std::map<std::string, metapp::Variant> >();
	}
	const metapp::MetaMappable * metaMappable = nullptr;
	const metapp::MetaIndexable * metaIndexable = nullptr;
	metapp::Variant result = metapp::Variant(type);

	if(metaMappable != nullptr) {
		auto valueType = metaMappable->getValueType(result);
		for(size_t i = 0; i < size_t(jsonValue->u.object.length); ++i) {
			const auto & objectValue = jsonValue->u.object.values[i];
			const metapp::Variant value(doConvertValue(objectValue.value, valueType.second));
			metaMappable->set(result, objectValue.name, value);
		}
	}
	else if(metaIndexable != nullptr) {
		metaIndexable->resize(result, jsonValue->u.object.length);
		for(size_t i = 0; i < size_t(jsonValue->u.object.length); ++i) {
			const auto & objectValue = jsonValue->u.object.values[i];
			const auto value = metaIndexable->get(result, i);
			auto valueIndexable = metapp::getNonReferenceMetaType(value.getMetaType())->getMetaIndexable();
			if(valueIndexable != nullptr) {
				valueIndexable->resize(value, 2);
				valueIndexable->set(value, 0, objectValue.name);
				const metapp::Variant convertedValue(doConvertValue(objectValue.value, valueIndexable->getValueType(value, 1)));
				valueIndexable->set(convertedValue, 1, value);
			}
		}
	}
	else if(metaClass != nullptr) {
		for(size_t i = 0; i < size_t(jsonValue->u.object.length); ++i) {
			const auto & objectValue = jsonValue->u.object.values[i];
			std::string name(objectValue.name);
			auto field = metaClass->getAccessible(name);
			if(! field.isEmpty()) {
				const metapp::Variant value(doConvertValue(objectValue.value, metapp::accessibleGetValueType(field)));
				metapp::accessibleSet(field, result.getAddress(), value);
			}
		}
	}
	return result;
}

} // namespace jsonparser_internal_


JsonParser::JsonParser()
	: JsonParser(ParserConfig())
{
}

JsonParser::JsonParser(const ParserConfig & config)
	: implement(new jsonparser_internal_::Implement(config))
{
}

JsonParser::~JsonParser()
{
}

metapp::Variant JsonParser::parse(const char * jsonText, const size_t length, const metapp::MetaType * proto)
{
	return implement->parse(jsonText, length, proto);
}

metapp::Variant JsonParser::parse(const std::string & jsonText, const metapp::MetaType * proto)
{
	return parse(jsonText.c_str(), jsonText.size(), proto);
}


} // namespace jsonpp

