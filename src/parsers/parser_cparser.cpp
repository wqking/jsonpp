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
#include "../parser.h"

#if JSONPP_BACKEND_CPARSER

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

#include "jsonparser/json.h"

#if defined(METAPP_COMPILER_GCC)
#pragma GCC diagnostic pop
#endif
#if defined(METAPP_COMPILER_VC)
#pragma warning(pop)
#endif

#include <array>

namespace jsonpp {

namespace internal_ {

class BackendCParser : public ParserBackend
{
public:
	explicit BackendCParser(const ParserConfig & config);
	~BackendCParser();

	bool hasError() const override;
	std::string getError() const override;

	metapp::Variant parse(const char * jsonText, const size_t length, const metapp::MetaType * proto) override;

private:
	metapp::Variant doConvertValue(json_value * jsonValue, const metapp::MetaType * proto);
	metapp::Variant doConvertArray(json_value * jsonValue, const metapp::MetaType * proto);
	metapp::Variant doConvertObject(json_value * jsonValue, const metapp::MetaType * proto);

private:
	ParserConfig config;

	json_settings settings;
	json_value * root;
	std::array<char, json_error_max> error;
};

BackendCParser::BackendCParser(const ParserConfig & config)
	: config(config), settings(), root(nullptr), error()
{
	settings.settings |= json_enable_comments;
}

BackendCParser::~BackendCParser()
{
	if(root != nullptr) {
		json_value_free(root);
	}
}

bool BackendCParser::hasError() const
{
	return error[0] != 0;
}

std::string BackendCParser::getError() const
{
	return error.data();
}

metapp::Variant BackendCParser::parse(const char * jsonText, const size_t length, const metapp::MetaType * proto)
{
	root = json_parse_ex(&settings, jsonText, length, error.data());
	if(error[0] != 0) {
		return metapp::Variant();
	}
	return doConvertValue(root, proto);
}

metapp::Variant BackendCParser::doConvertValue(json_value * jsonValue, const metapp::MetaType * proto)
{
	if(proto != nullptr) {
		proto = metapp::getNonReferenceMetaType(proto);
		if(proto->getTypeKind() == metapp::tkVariant) {
			proto = nullptr;
		}
	}

	switch(jsonValue->type) {
	case json_null:
		return metapp::Variant(nullptr);

	case json_boolean:
		return metapp::Variant((JsonBool)(jsonValue->u.boolean));

	case json_integer:
		return metapp::Variant((JsonInt)(jsonValue->u.integer));

	case json_double:
		return metapp::Variant((JsonReal)(jsonValue->u.dbl));

	case json_string:
		return metapp::Variant(JsonString(jsonValue->u.string.ptr));

	case json_array:
		return doConvertArray(jsonValue, proto);

	case json_object:
		return doConvertObject(jsonValue, proto);

	default:
		break;
	}
	return metapp::Variant();
}

metapp::Variant BackendCParser::doConvertArray(json_value * jsonValue, const metapp::MetaType * proto)
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

metapp::Variant BackendCParser::doConvertObject(json_value * jsonValue, const metapp::MetaType * proto)
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

	if(metaMappable != nullptr) {
		auto valueType = metaMappable->getValueType(result);
		for(size_t i = 0; i < size_t(jsonValue->u.object.length); ++i) {
			const auto & objectValue = jsonValue->u.object.values[i];
			const metapp::Variant value(doConvertValue(objectValue.value, valueType->getUpType(1)));
			metaMappable->set(result, objectValue.name, value);
		}
	}
	else if(metaIndexable != nullptr) {
		metaIndexable->resize(result, jsonValue->u.object.length);
		for(size_t i = 0; i < size_t(jsonValue->u.object.length); ++i) {
			const auto & objectValue = jsonValue->u.object.values[i];
			const auto value = metaIndexable->get(result, i);
			auto valueIndexable = metapp::getNonReferenceMetaType(value)->getMetaIndexable();
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

std::unique_ptr<ParserBackend> createBackend_cparser(const ParserConfig & config)
{
	return std::unique_ptr<ParserBackend>(new BackendCParser(config));
}


} // namespace internal_

} // namespace jsonpp

#endif
