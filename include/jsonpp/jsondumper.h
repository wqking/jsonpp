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

namespace jsondumper_internal_ {

template <typename Outputter>
class JsonDumperImplement;

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

struct StreamOutputter
{
	StreamOutputter(std::ostream & stream)
		: stream(stream)
	{
	}

	void operator() (const char c) const {
		stream.put(c);
	}

	void operator() (const char * s, const std::size_t length) const {
		stream.write(s, static_cast<std::streamsize>(length));
	}

	std::ostream & stream;
};

class JsonDumper
{
public:
	JsonDumper()
		: JsonDumper(DumperConfig())
	{
	}

	explicit JsonDumper(const DumperConfig & config)
		: config(config)
	{
	}

	~JsonDumper() {
	}

	//void dump(const metapp::Variant & value, std::ostream & stream);
	
	std::string dump(const metapp::Variant & value) {
		std::stringstream ss;
		StreamOutputter outputter(ss);
		dump(value, outputter);
		return ss.str();
	}

	template <typename Outputter>
	void dump(const metapp::Variant & value, const Outputter & outputter) {
		jsondumper_internal_::JsonDumperImplement<Outputter>(config, outputter).dump(value);
	}

private:
	DumperConfig config;
};

} // namespace jsonpp

#include "implement/jsondumper_impl.h"

#endif
