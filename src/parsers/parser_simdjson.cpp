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

#if JSONPP_BACKEND_SIMDJSON

#include "metapp/allmetatypes.h"
#include "metapp/interfaces/metaclass.h"
#include "metapp/interfaces/metaindexable.h"
#include "metapp/interfaces/metamappable.h"
#include "metapp/compiler.h"

#if defined(METAPP_COMPILER_VC)
#pragma warning(push)
#pragma warning(disable: 4245 4100 4459)
#endif
#if defined(METAPP_COMPILER_GCC)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
//#pragma GCC diagnostic ignored "-Wunused-parameter"
#endif

#include "simdjson/simdjson.h"

#if defined(METAPP_COMPILER_GCC)
#pragma GCC diagnostic pop
#endif
#if defined(METAPP_COMPILER_VC)
#pragma warning(pop)
#endif

#include <array>

namespace jsonpp {

namespace internal_ {

class BackendSimdjson : public ParserBackend
{
public:
	void prepareSource(const JsonParserSource & source) const override {
		source.pad(simdjson::SIMDJSON_PADDING);
	}

};
class BackendSimdjsonDom : public BackendSimdjson
{
public:
	explicit BackendSimdjsonDom(const ParserConfig & config);
	~BackendSimdjsonDom();

	bool hasError() const override;
	std::string getError() const override;

	metapp::Variant parse(const JsonParserSource & source, const metapp::MetaType * proto) override;

private:
	metapp::Variant doConvertValue(const simdjson::dom::element & element, const metapp::MetaType * proto);
	metapp::Variant doConvertArray(const simdjson::dom::element & element, const metapp::MetaType * proto);
	metapp::Variant doConvertObject(const simdjson::dom::element & element, const metapp::MetaType * proto);

private:
	ParserConfig config;
	simdjson::dom::parser parser;

	std::string errorString;
};

BackendSimdjsonDom::BackendSimdjsonDom(const ParserConfig & config)
	: config(config), parser()
{
}

BackendSimdjsonDom::~BackendSimdjsonDom()
{
}

bool BackendSimdjsonDom::hasError() const
{
	return ! errorString.empty();
}

std::string BackendSimdjsonDom::getError() const
{
	return errorString;
}

metapp::Variant BackendSimdjsonDom::parse(const JsonParserSource & source, const metapp::MetaType * proto)
{
	errorString.clear();

	simdjson::dom::element element;
	auto r = parser.parse(source.getText(), source.getTextLength(), false).get(element);
	if(r != simdjson::SUCCESS) {
		errorString = simdjson::error_message(r);
		return metapp::Variant();
	}
	return doConvertValue(element, proto);
}

metapp::Variant BackendSimdjsonDom::doConvertValue(const simdjson::dom::element & element, const metapp::MetaType * proto)
{
	if(proto != nullptr) {
		proto = metapp::getNonReferenceMetaType(proto);
		if(proto->getTypeKind() == metapp::tkVariant) {
			proto = nullptr;
		}
	}

	switch(element.type()) {
	case simdjson::dom::element_type::NULL_VALUE:
		return metapp::Variant(nullptr);

	case simdjson::dom::element_type::BOOL:
		return metapp::Variant((JsonBool)(element.get<bool>()));

	case simdjson::dom::element_type::INT64:
		return metapp::Variant((JsonInt)(element.get<int64_t>()));

	case simdjson::dom::element_type::UINT64:
		return metapp::Variant((JsonInt)(element.get<uint64_t>()));

	case simdjson::dom::element_type::DOUBLE:
		return metapp::Variant((JsonReal)(element.get<double>()));

	case simdjson::dom::element_type::STRING:
		return metapp::Variant((JsonString)(element.get<const char *>().value()));

	case simdjson::dom::element_type::ARRAY:
		return doConvertArray(element, proto);

	case simdjson::dom::element_type::OBJECT:
		return doConvertObject(element, proto);

	}

	return metapp::Variant();
}

metapp::Variant BackendSimdjsonDom::doConvertArray(const simdjson::dom::element & element, const metapp::MetaType * proto)
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
	simdjson::dom::array array = element.get<simdjson::dom::array>();
	metaIndexable->resize(result, array.size());
	int index = 0;
	for(const auto & item : array) {
		const metapp::MetaType * elementProto = nullptr;
		if(proto != nullptr) {
			elementProto = metapp::getNonReferenceMetaType(metaIndexable->getValueType(result, index));
		}
		metaIndexable->set(result, index, doConvertValue(item, elementProto));
		++index;
	}
	return result;
}

metapp::Variant BackendSimdjsonDom::doConvertObject(const simdjson::dom::element & element, const metapp::MetaType * proto)
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
	simdjson::dom::object object = element.get<simdjson::dom::object>();

	if(metaMappable != nullptr) {
		auto valueType = metaMappable->getValueType(result);
		for(auto it = object.begin(); it != object.end(); ++it) {
			const metapp::Variant value(doConvertValue(it.value(), valueType->getUpType(1)));
			metaMappable->set(result, it.key_c_str(), value);
		}
	}
	else if(metaIndexable != nullptr) {
		metaIndexable->resize(result, object.size());
		int index = 0;
		for(auto it = object.begin(); it != object.end(); ++it) {
			const auto value = metaIndexable->get(result, index);
			auto valueIndexable = metapp::getNonReferenceMetaType(value)->getMetaIndexable();
			if(valueIndexable != nullptr) {
				valueIndexable->resize(value, 2);
				valueIndexable->set(value, 0, it.key_c_str());
				const metapp::Variant convertedValue(doConvertValue(it.value(), valueIndexable->getValueType(value, 1)));
				valueIndexable->set(convertedValue, 1, value);
			}
			++index;
		}
	}
	else if(metaClass != nullptr) {
		for(auto it = object.begin(); it != object.end(); ++it) {
			std::string name(it.key_c_str());
			auto field = metaClass->getAccessible(name);
			if(! field.isEmpty()) {
				const metapp::Variant value(doConvertValue(it.value(), metapp::accessibleGetValueType(field)));
				metapp::accessibleSet(field, result.getAddress(), value);
			}
		}
	}
	return result;
}

class BackendSimdjsonOnDemand : public BackendSimdjson
{
public:
	explicit BackendSimdjsonOnDemand(const ParserConfig & config);
	~BackendSimdjsonOnDemand();

	bool hasError() const override;
	std::string getError() const override;

	metapp::Variant parse(const JsonParserSource & source, const metapp::MetaType * proto) override;

private:
	template <typename T>
	metapp::Variant doConvertValue(T && element, const metapp::MetaType * proto);
	metapp::Variant doConvertArray(simdjson::ondemand::value element, const metapp::MetaType * proto);
	metapp::Variant doConvertObject(simdjson::ondemand::value element, const metapp::MetaType * proto);

private:
	ParserConfig config;
	simdjson::ondemand::parser parser;

	std::string errorString;
};

BackendSimdjsonOnDemand::BackendSimdjsonOnDemand(const ParserConfig & config)
	: config(config), parser()
{
}

BackendSimdjsonOnDemand::~BackendSimdjsonOnDemand()
{
}

bool BackendSimdjsonOnDemand::hasError() const
{
	return ! errorString.empty();
}

std::string BackendSimdjsonOnDemand::getError() const
{
	return errorString;
}

metapp::Variant BackendSimdjsonOnDemand::parse(const JsonParserSource & source, const metapp::MetaType * proto)
{
	errorString.clear();

	try {
		simdjson::ondemand::document document = parser.iterate(source.getText(), source.getTextLength(), source.getCapacity());
		return doConvertValue(std::move(document), proto);
	}
	catch(const simdjson::simdjson_error & e) {
		errorString = e.what();
	}
	catch(const std::exception & e) {
		errorString = e.what();
	}
	return metapp::Variant();
}

simdjson::ondemand::value getValue(simdjson::ondemand::document && v) {
	return v.get_value();
}

simdjson::ondemand::value getValue(simdjson::ondemand::value v) {
	return v;
}

template <typename T>
std::string toString(T && s)
{
	auto view = s.value();
	return std::string(view.data(), view.size());
}

template <typename T>
metapp::Variant BackendSimdjsonOnDemand::doConvertValue(T && element, const metapp::MetaType * proto)
{
	if(proto != nullptr) {
		proto = metapp::getNonReferenceMetaType(proto);
		if(proto->getTypeKind() == metapp::tkVariant) {
			proto = nullptr;
		}
	}

	switch(element.type()) {
	case simdjson::ondemand::json_type::null:
		return metapp::Variant(nullptr);

	case simdjson::ondemand::json_type::boolean:
		return metapp::Variant((JsonBool)(element.get_bool()));

	case simdjson::ondemand::json_type::number: {
		switch(element.get_number_type()) {
		case simdjson::ondemand::number_type::signed_integer:
			return metapp::Variant((JsonInt)(element.get_int64()));

		case simdjson::ondemand::number_type::unsigned_integer:
			return metapp::Variant((JsonUnsignedInt)(element.get_uint64()));

		default:
			return metapp::Variant((JsonReal)(element.get_double()));
		}
	}

	case simdjson::ondemand::json_type::string: {
		return metapp::Variant(toString(element.get_string()));
	}

	case simdjson::ondemand::json_type::array:
		return doConvertArray(getValue(std::forward<T>(element)), proto);

	case simdjson::ondemand::json_type::object:
		return doConvertObject(getValue(std::forward<T>(element)), proto);

	}

	return metapp::Variant();
}

metapp::Variant BackendSimdjsonOnDemand::doConvertArray(simdjson::ondemand::value element, const metapp::MetaType * proto)
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
	simdjson::ondemand::array array = element.get_array();
	metaIndexable->resize(result, array.count_elements());
	int index = 0;
	for(auto item : array) {
		const metapp::MetaType * elementProto = nullptr;
		if(proto != nullptr) {
			elementProto = metapp::getNonReferenceMetaType(metaIndexable->getValueType(result, index));
		}
		metaIndexable->set(result, index, doConvertValue(item.value(), elementProto));
		++index;
	}
	return result;
}

metapp::Variant BackendSimdjsonOnDemand::doConvertObject(simdjson::ondemand::value element, const metapp::MetaType * proto)
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
	simdjson::ondemand::object object = element.get_object();

	if(metaMappable != nullptr) {
		auto valueType = metaMappable->getValueType(result);
		for(auto item : object) {
			const metapp::Variant value(doConvertValue(item.value(), valueType->getUpType(1)));
			metaMappable->set(result, toString(item.unescaped_key()), value);
		}
	}
	else if(metaIndexable != nullptr) {
		metaIndexable->resize(result, object.count_fields());
		int index = 0;
		for(auto item : object) {
			const auto value = metaIndexable->get(result, index);
			auto valueIndexable = metapp::getNonReferenceMetaType(value)->getMetaIndexable();
			if(valueIndexable != nullptr) {
				valueIndexable->resize(value, 2);
				valueIndexable->set(value, 0, toString(item.unescaped_key()));
				const metapp::Variant convertedValue(doConvertValue(item.value(), valueIndexable->getValueType(value, 1)));
				valueIndexable->set(convertedValue, 1, value);
			}
			++index;
		}
	}
	else if(metaClass != nullptr) {
		for(auto item : object) {
			std::string name(toString(item.unescaped_key()));
			auto field = metaClass->getAccessible(name);
			if(! field.isEmpty()) {
				const metapp::Variant value(doConvertValue(item.value(), metapp::accessibleGetValueType(field)));
				metapp::accessibleSet(field, result.getAddress(), value);
			}
		}
	}
	return result;
}

std::unique_ptr<ParserBackend> createBackend_simdjsonDom(const ParserConfig & config)
{
	return std::unique_ptr<ParserBackend>(new BackendSimdjsonDom(config));
}

std::unique_ptr<ParserBackend> createBackend_simdjsonOnDemand(const ParserConfig & config)
{
	return std::unique_ptr<ParserBackend>(new BackendSimdjsonOnDemand(config));
}

} // namespace internal_

} // namespace jsonpp


#endif
