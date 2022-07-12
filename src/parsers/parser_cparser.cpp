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

#if JSONPP_BACKEND_CPARSER

#if defined(METAPP_COMPILER_VC)
#pragma warning(push)
#pragma warning(disable: 4244 4996 4706)
#endif
#if defined(METAPP_COMPILER_GCC)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
//#pragma GCC diagnostic ignored "-Wunused-parameter"
#endif

#include "../thirdparty/json_parser/json.h"

#if defined(METAPP_COMPILER_GCC)
#pragma GCC diagnostic pop
#endif
#if defined(METAPP_COMPILER_VC)
#pragma warning(pop)
#endif

#include <array>

namespace jsonpp {

namespace internal_ {

struct CParserImplement
{
	using ArrayValue = json_value *;
	using ObjectValue = json_value *;
	using Array = json_value *;
	using Object = json_value *;

	static constexpr auto typeNull = json_type::json_null;
	static constexpr auto typeBoolean = json_type::json_boolean;
	static constexpr auto typeInteger = json_type::json_integer;
	static constexpr auto typeUnsignedInteger = json_type::json_none;
	static constexpr auto typeDouble = json_type::json_double;
	static constexpr auto typeString = json_type::json_string;
	static constexpr auto typeArray = json_type::json_array;
	static constexpr auto typeObject = json_type::json_object;

	json_type getNodeType(json_value * node) const {
		return node->type;
	}

	bool getBoolean(json_value * node) const {
		return node->u.boolean;
	}

	int64_t getInteger(json_value * node) const {
		return node->u.integer;
	}

	uint64_t getUnsignedInteger(json_value * node) const {
		return node->u.integer;
	}

	double getDouble(json_value * node) const {
		return node->u.dbl;
	}

	std::string getString(json_value * node) const {
		return std::string(node->u.string.ptr, node->u.string.length);
	}

	json_value * getArray(json_value * node) const {
		return node;
	}

	json_value * getObject(json_value * node) const {
		return node;
	}

	std::size_t getArraySize(json_value * node) const {
		return node->u.array.length;
	}

	template <typename Callback>
	void iterateArray(json_value * node, const Callback & callback) const {
		for(std::size_t i = 0; i < std::size_t(node->u.array.length); ++i) {
			callback(i, node->u.array.values[i]);
		}
	}

	std::size_t getObjectSize(json_value * node) const {
		return node->u.object.length;
	}

	template <typename Callback>
	void iterateObject(json_value * node, const Callback & callback) const {
		for(std::size_t i = 0; i < std::size_t(node->u.object.length); ++i) {
			const auto & objectValue = node->u.object.values[i];
			callback(objectValue.name, objectValue.value);
		}
	}

};

class BackendCParser : public ParserBackend
{
public:
	explicit BackendCParser(const ParserConfig & config);
	~BackendCParser();

	ParserBackendResult parse(const ParserSource & source, const metapp::MetaType * prototype) override;

private:
	ParserConfig config;

	json_settings settings;
	json_value * root;
};

BackendCParser::BackendCParser(const ParserConfig & config)
	: config(config), settings(), root(nullptr)
{
	if(config.allowComment()) {
		settings.settings |= json_enable_comments;
	}
}

BackendCParser::~BackendCParser()
{
	if(root != nullptr) {
		json_value_free(root);
	}
}

ParserBackendResult BackendCParser::parse(const ParserSource & source, const metapp::MetaType * prototype)
{
	std::array<char, json_error_max> error;
	error[0] = 0;

	root = json_parse_ex(&settings, source.getText(), source.getTextLength(), error.data());
	if(error[0] != 0) {
		return { metapp::Variant(), error.data() };
	}
	return { GeneralParser<CParserImplement>(config, CParserImplement()).parse(root, prototype), std::string() };
}

std::unique_ptr<ParserBackend> createBackend_cparser(const ParserConfig & config)
{
	return std::unique_ptr<ParserBackend>(new BackendCParser(config));
}


} // namespace internal_

} // namespace jsonpp

#endif
