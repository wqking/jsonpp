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

#ifndef JSONPP_PARSER_H_821598293712
#define JSONPP_PARSER_H_821598293712

#include "jsonpp/common.h"

#include <memory>
#include <string>
#include <vector>
#include <map>

namespace jsonpp {

enum class ParserBackendType
{
	simdjson,
	cparser,
};

class ParserBackend;
class ParserConfig;

namespace internal_ {

using BackendCreator = std::unique_ptr<ParserBackend> (*)(const ParserConfig & config);

std::unique_ptr<ParserBackend> createBackend_cparser(const ParserConfig & config);
std::unique_ptr<ParserBackend> createBackend_simdjsonDom(const ParserConfig & config);

template <ParserBackendType type>
struct BackendCreatorGetter;

template <>
struct BackendCreatorGetter <ParserBackendType::simdjson>
{
	static BackendCreator getCreator() {
		return &createBackend_simdjsonDom;
	}
};

template <>
struct BackendCreatorGetter <ParserBackendType::cparser>
{
	static BackendCreator getCreator() {
		return &createBackend_cparser;
	}
};

struct ScopedInvoke
{
	using Callback = std::function<void ()>;

	explicit ScopedInvoke(const Callback & callback) : callback(callback) {
	}

	~ScopedInvoke() {
		callback();
	}

	Callback callback;
};

} // namespace internal_

class ParserBackend;

class ParserConfig
{
public:
	ParserConfig();
	~ParserConfig();

	ParserBackendType getBackendType() const {
		return backendType;
	}

	template <ParserBackendType type>
	ParserConfig & setBackendType() {
		backendType = type;
		backendCreator = internal_::BackendCreatorGetter<type>::getCreator();
		return *this;
	}

	internal_::BackendCreator getBackendCreator() const {
		return backendCreator;
	}

	bool allowComment() const {
		return comment;
	}

	ParserConfig & enableComment(const bool enable) {
		comment = enable;
		return *this;
	}

	const metapp::MetaType * getArrayType() const {
		return arrayType;
	}

	template <typename T>
	ParserConfig & setArrayType() {
		return setArrayType(metapp::getMetaType<T>());
	}

	ParserConfig & setArrayType(const metapp::MetaType * arrayType_) {
		assert(metapp::getNonReferenceMetaType(arrayType_)->hasMetaIndexable());

		arrayType = arrayType_;
		return *this;
	}

	const metapp::MetaType * getObjectType() const {
		return objectType;
	}

	template <typename T>
	ParserConfig & setObjectType() {
		return setObjectType(metapp::getMetaType<T>());
	}

	ParserConfig & setObjectType(const metapp::MetaType * objectType_) {
		assert(
			metapp::getNonReferenceMetaType(objectType_)->hasMetaMappable()
			|| metapp::getNonReferenceMetaType(objectType_)->hasMetaIndexable()
		);

		objectType = objectType_;
		return *this;
	}

private:
	ParserBackendType backendType;
	internal_::BackendCreator backendCreator;
	bool comment;
	const metapp::MetaType * arrayType;
	const metapp::MetaType * objectType;
};

class ParserSource
{
private:
	enum class StorageType {
		cstr,
		ref,
		string
	};

public:
	ParserSource();
	ParserSource(const char * cstr, const std::size_t cstrLength);
	explicit ParserSource(const std::string & str);
	explicit ParserSource(std::string && str);

	const char * getText() const;
	std::size_t getTextLength() const;
	std::size_t getCapacity() const;

	void pad(const std::size_t size) const;

private:
	bool hasPrepared() const {
		return prepared;
	}

	void setAsPrepared() const {
		prepared = true;
	}

private:
	mutable bool prepared;
	mutable StorageType storageType;
	mutable const char * cstr;
	mutable std::size_t cstrLength;
	mutable const std::string * ref;
	mutable std::string str;

	friend class Parser;
};

class Parser
{
private:
	template <typename T>
	struct IsValidType
	{
		static constexpr bool value = 
			! std::is_reference<T>::value
			&& ! std::is_array<T>::value
			&& ! std::is_const<T>::value
			&& ! std::is_volatile<T>::value
		;
	};

public:
	Parser();
	explicit Parser(const ParserConfig & config);
	~Parser();

	bool hasError() const;
	const std::string & getError() const;

	metapp::Variant parse(const char * jsonText, const std::size_t length, const metapp::MetaType * prototype = nullptr);
	metapp::Variant parse(const std::string & jsonText, const metapp::MetaType * prototype = nullptr);
	metapp::Variant parse(const ParserSource & source, const metapp::MetaType * prototype = nullptr);

	template <typename T>
	T parse(const char * jsonText, const std::size_t length) {
		static_assert(IsValidType<T>::value, "Type must be raw type without CV, reference, and array");

		const metapp::Variant result = parse(jsonText, length, metapp::getMetaType<T>());
		if(hasError()) {
			return T();
		}
		return result.get<const T &>();
	}

	template <typename T>
	T parse(const std::string & jsonText) {
		static_assert(IsValidType<T>::value, "Type must be raw type without CV, reference, and array");

		const metapp::Variant result = parse(jsonText, metapp::getMetaType<T>());
		if(hasError()) {
			return T();
		}
		return result.get<const T &>();
	}

	template <typename T>
	T parse(const ParserSource & source) {
		static_assert(IsValidType<T>::value, "Type must be raw type without CV, reference, and array");

		const metapp::Variant result = parse(source, metapp::getMetaType<T>());
		if(hasError()) {
			return T();
		}
		return result.get<const T &>();
	}

private:
	std::unique_ptr<ParserBackend> backend;
	std::string errorMessage;
};

std::string getParserBackendName(const ParserBackendType type);

} // namespace jsonpp

#endif
