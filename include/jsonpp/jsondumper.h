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

#include "metapp/variant.h"

#include <memory>
#include <ostream>

namespace jsonpp {

namespace jsondumper_internal_ {
class Implement;
} // namespace jsondumper_internal_

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

	void dump(const metapp::Variant & value, std::ostream & stream);
	std::string dump(const metapp::Variant & value);

private:
	std::unique_ptr<jsondumper_internal_::Implement> implement;
};

} // namespace jsonpp

#endif
