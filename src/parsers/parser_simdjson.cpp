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

#include "jsonpp/parser.h"
#include "jsonpp/parserbackend.h"

#if JSONPP_BACKEND_SIMDJSON

#if defined(METAPP_COMPILER_VC)
#pragma warning(push)
#pragma warning(disable: 4245 4100 4459)
#endif
#if defined(METAPP_COMPILER_GCC)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
//#pragma GCC diagnostic ignored "-Wunused-parameter"
#endif

#include "../thirdparty/simdjson/simdjson.h"

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
	void prepareSource(const ParserSource & source) const override {
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

	metapp::Variant parse(const ParserSource & source, const metapp::MetaType * proto) override;

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

struct SimdjsonDomImplement
{
	using ArrayValue = const simdjson::dom::element &;
	using ObjectValue = const simdjson::dom::element &;
	using Array = simdjson::dom::array;
	using Object = simdjson::dom::object;

	static constexpr auto typeNull = simdjson::dom::element_type::NULL_VALUE;
	static constexpr auto typeBoolean = simdjson::dom::element_type::BOOL;
	static constexpr auto typeInteger = simdjson::dom::element_type::INT64;
	static constexpr auto typeUnsignedInteger = simdjson::dom::element_type::UINT64;
	static constexpr auto typeDouble = simdjson::dom::element_type::DOUBLE;
	static constexpr auto typeString = simdjson::dom::element_type::STRING;
	static constexpr auto typeArray = simdjson::dom::element_type::ARRAY;
	static constexpr auto typeObject = simdjson::dom::element_type::OBJECT;

	simdjson::dom::element_type getNodeType(const simdjson::dom::element & node) const {
		return node.type();
	}

	bool getBoolean(const simdjson::dom::element & node) const {
		return node.get<bool>();
	}

	int64_t getInteger(const simdjson::dom::element & node) const {
		return node.get<int64_t>();
	}

	uint64_t getUnsignedInteger(const simdjson::dom::element & node) const {
		return node.get<uint64_t>();
	}

	double getDouble(const simdjson::dom::element & node) const {
		return node.get<double>();
	}

	std::string getString(const simdjson::dom::element & node) const {
		return node.get<const char *>().value();
	}

	simdjson::dom::array getArray(const simdjson::dom::element & node) const {
		return node.get_array();
	}

	simdjson::dom::object getObject(const simdjson::dom::element & node) const {
		return node.get_object();
	}

	std::size_t getArraySize(const simdjson::dom::array & node) const {
		return node.size();
	}

	template <typename Callback>
	void iterateArray(const simdjson::dom::array & node, const Callback & callback) const {
		std::size_t index = 0;
		for(const auto & item : node) {
			callback(index, item);
			++index;
		}
	}

	std::size_t getObjectSize(const simdjson::dom::object & node) const {
		return node.size();
	}

	template <typename Callback>
	void iterateObject(const simdjson::dom::object & node, const Callback & callback) const {
		for(auto it = node.begin(); it != node.end(); ++it) {
			callback(it.key_c_str(), it.value());
		}
	}

};

metapp::Variant BackendSimdjsonDom::parse(const ParserSource & source, const metapp::MetaType * proto)
{
	errorString.clear();

	simdjson::dom::element element;
	auto r = parser.parse(source.getText(), source.getTextLength(), false).get(element);
	if(r != simdjson::SUCCESS) {
		errorString = simdjson::error_message(r);
		return metapp::Variant();
	}
	return GeneralParser<SimdjsonDomImplement>(config, SimdjsonDomImplement()).parse(element, proto);
}

std::unique_ptr<ParserBackend> createBackend_simdjsonDom(const ParserConfig & config)
{
	return std::unique_ptr<ParserBackend>(new BackendSimdjsonDom(config));
}

#if 0
/*
Dropped support for simdjson::ondemand, because,
1, it doesn't gain much performance than simdjson::dom for jsonpp.
2, it doesn't treat "5, 6" as error.
3, it throws error on "true", while the node has boolean type but get_bool doesn't agree.
No need to waste time on it.
*/
class BackendSimdjsonOnDemand : public BackendSimdjson
{
public:
	explicit BackendSimdjsonOnDemand(const ParserConfig & config);
	~BackendSimdjsonOnDemand();

	bool hasError() const override;
	std::string getError() const override;

	metapp::Variant parse(const ParserSource & source, const metapp::MetaType * proto) override;

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

struct SimdjsonOnDemandImplement
{
	using ArrayValue = simdjson::ondemand::value;
	using ObjectValue = simdjson::ondemand::value;
	using Array = simdjson::ondemand::array;
	using Object = simdjson::ondemand::object;

	static constexpr int typeNull = (int)simdjson::ondemand::json_type::null;
	static constexpr int typeBoolean = (int)simdjson::ondemand::json_type::boolean;
	static constexpr int typeInteger = (int)simdjson::ondemand::number_type::signed_integer + 1000;
	static constexpr int typeUnsignedInteger = (int)simdjson::ondemand::number_type::unsigned_integer + 1000;
	static constexpr int typeDouble = (int)simdjson::ondemand::number_type::floating_point_number + 1000;
	static constexpr int typeString = (int)simdjson::ondemand::json_type::string;
	static constexpr int typeArray = (int)simdjson::ondemand::json_type::array;
	static constexpr int typeObject = (int)simdjson::ondemand::json_type::object;

	template <typename T>
	int getNodeType(T && node) const {
		auto type = node.type().value();
		if(type == simdjson::ondemand::json_type::number) {
			return (int)node.get_number_type().value() + 1000;
		}
		return (int)type;
	}

	template <typename T>
	bool getBoolean(T && node) const {
		return node.get_bool();
	}

	template <typename T>
	int64_t getInteger(T && node) const {
		return node.get_int64();
	}

	template <typename T>
	uint64_t getUnsignedInteger(T && node) const {
		return node.get_uint64();
	}

	template <typename T>
	double getDouble(T && node) const {
		return node.get_double();
	}

	template <typename T>
	std::string getString(T && node) const {
		return toString(node.get_string());
	}

	template <typename T>
	simdjson::ondemand::array getArray(T && node) const {
		return node.get_array();
	}

	template <typename T>
	simdjson::ondemand::object getObject(T && node) const {
		return node.get_object();
	}

	std::size_t getArraySize(simdjson::ondemand::array node) const {
		return node.count_elements();
	}

	template <typename Callback>
	void iterateArray(simdjson::ondemand::array node, const Callback & callback) const {
		int index = 0;
		for(auto item : node) {
			callback(index, item.value());
			++index;
		}
	}

	std::size_t getObjectSize(simdjson::ondemand::object node) const {
		return node.count_fields();
	}

	template <typename Callback>
	void iterateObject(simdjson::ondemand::object node, const Callback & callback) const {
		for(auto item : node) {
			callback(toString(item.unescaped_key()), item.value());
		}
	}

};

metapp::Variant BackendSimdjsonOnDemand::parse(const ParserSource & source, const metapp::MetaType * proto)
{
	errorString.clear();

	try {
		simdjson::ondemand::document document = parser.iterate(source.getText(), source.getTextLength(), source.getCapacity());
		return GeneralParser<SimdjsonOnDemandImplement>(config, SimdjsonOnDemandImplement()).parse(std::move(document), proto);
	}
	catch(const simdjson::simdjson_error & e) {
		errorString = e.what();
	}
	catch(const std::exception & e) {
		errorString = e.what();
	}
	return metapp::Variant();
}

std::unique_ptr<ParserBackend> createBackend_simdjsonOnDemand(const ParserConfig & config)
{
	return std::unique_ptr<ParserBackend>(new BackendSimdjsonOnDemand(config));
}
#endif


} // namespace internal_

} // namespace jsonpp


#endif
