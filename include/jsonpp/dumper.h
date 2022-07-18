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

#ifndef JSONPP_DUMPER_H_821598293712
#define JSONPP_DUMPER_H_821598293712

#include "jsonpp/common.h"

#include "implement/algorithms_i.h"

#include <memory>
#include <vector>
#include <ostream>
#include <sstream>
#include <vector>

namespace jsonpp {

namespace internal_ {

template <typename Output>
class DumperImplement;

} // namespace internal_

class DumperConfig
{
public:
	DumperConfig()
		:
			beautify(false),
			indent("    "),
			namedEnum(false),
			objectTypeList(),
			arrayTypeList()
	{
	}

	bool allowBeautify() const {
		return beautify;
	}

	DumperConfig & enableBeautify(const bool enable) {
		beautify = enable;
		return *this;
	}

	bool allowNamedEnum() const {
		return namedEnum;
	}

	DumperConfig & enableNamedEnum(const bool enable) {
		namedEnum = enable;
		return *this;
	}

	DumperConfig & setIndent(const std::string & indent_) {
		indent = indent_;
		return *this;
	}

	const std::string & getIndent() const {
		return indent;
	}

	bool isObjectType(const metapp::MetaType * metaType) const {
		return ! objectTypeList.empty()
			&& std::binary_search(objectTypeList.begin(), objectTypeList.end(), metaType)
		;
	}

	template <typename T>
	DumperConfig & addObjectType() {
		return addObjectType(metapp::getMetaType<T>());
	}

	DumperConfig & addObjectType(const metapp::MetaType * metaType) {
		addToObjectArrayTypeList(objectTypeList, metaType);
		return *this;
	}

	bool isArrayType(const metapp::MetaType * metaType) const {
		return ! arrayTypeList.empty()
			&& std::binary_search(arrayTypeList.begin(), arrayTypeList.end(), metaType)
		;
	}

	template <typename T>
	DumperConfig & addArrayType() {
		return addArrayType(metapp::getMetaType<T>());
	}

	DumperConfig & addArrayType(const metapp::MetaType * metaType) {
		addToObjectArrayTypeList(arrayTypeList, metaType);
		return *this;
	}

private:
	void addToObjectArrayTypeList(std::vector<const metapp::MetaType *> & typeList, const metapp::MetaType * metaType) {
		typeList.push_back(metaType);
		std::sort(typeList.begin(), typeList.end());
	}

private:
	bool beautify;
	std::string indent;
	bool namedEnum;
	std::vector<const metapp::MetaType *> objectTypeList;
	std::vector<const metapp::MetaType *> arrayTypeList;
};

class Dumper
{
public:
	Dumper();
	explicit Dumper(const DumperConfig & config);
	~Dumper();

	std::string dump(const metapp::Variant & value);

	template <typename Output>
	void dump(const metapp::Variant & value, const Output & output) {
		internal_::DumperImplement<Output>(config, output).dump(value);
	}

private:
	DumperConfig config;
};


} // namespace jsonpp

#include "implement/dumper_impl.h"

#endif
