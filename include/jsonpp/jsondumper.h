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

#ifndef JSONPP_JSONDUMPER_H_821598293712
#define JSONPP_JSONDUMPER_H_821598293712

#include "implement/algorithms_i.h"
#include "drachennest/dragonbox.h"

#include "metapp/variant.h"
#include "metapp/allmetatypes.h"
#include "metapp/interfaces/metaclass.h"
#include "metapp/interfaces/metaindexable.h"
#include "metapp/interfaces/metamappable.h"
#include "metapp/utilities/utility.h"
#include "metapp/compiler.h"

#include <memory>
#include <vector>
#include <ostream>
#include <sstream>

namespace jsonpp {

namespace internal_ {

template <typename Output>
class JsonDumperImplement;

} // namespace internal_

class DumperConfig
{
public:
	DumperConfig()
		:
			beautify(false)
	{
	}

	DumperConfig & setBeautify(const bool beautify_) {
		beautify = beautify_;
		return *this;
	}

	bool allowBeautify() const {
		return beautify;
	}

	bool isObjectType(const metapp::MetaType *) const {
		return false;
	}

	bool isArrayType(const metapp::MetaType *) const {
		return false;
	}

private:
	bool beautify;
};

class JsonDumper
{
public:
	JsonDumper();
	explicit JsonDumper(const DumperConfig & config);
	~JsonDumper();

	std::string dump(const metapp::Variant & value);

	template <typename Writer>
	void dump(const metapp::Variant & value, const Writer & writer) {
		internal_::JsonDumperImplement<Writer>(config, writer).dump(value);
	}

private:
	DumperConfig config;
};


} // namespace jsonpp

#include "implement/jsondumper_impl.h"

#endif
