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

#include "metapp/variant.h"

#include <memory>
#include <ostream>

namespace jsonpp {

namespace internal_ {

class ParserBackend
{
public:
	virtual ~ParserBackend() {}

	virtual bool hasError() const = 0;
	virtual std::string getError() const = 0;

	virtual metapp::Variant parse(const char * jsonText, const std::size_t length, const metapp::MetaType * proto) = 0;

private:
};

} // namespace internal_

} // namespace jsonpp

#endif
