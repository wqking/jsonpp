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

#include "jsonpp/jsondumper.h"
#include "jsonpp/textwriter.h"

#include "metapp/allmetatypes.h"
#include "metapp/interfaces/metaclass.h"
#include "metapp/interfaces/metaindexable.h"
#include "metapp/interfaces/metamappable.h"
#include "metapp/compiler.h"

#include <array>
#include <limits>

namespace jsonpp {

namespace internal_ {

char digitPairList[] = {
	"00010203040506070809"
	"10111213141516171819"
	"20212223242526272829"
	"30313233343536373839"
	"40414243444546474849"
	"50515253545556575859"
	"60616263646566676869"
	"70717273747576777879"
	"80818283848586878889"
	"90919293949596979899"
};

} // namespace internal_

const std::array<EscapeItem, escapeItemListSize> escapeItemList {{
	{ "\\0", 2 }, // 0
	{ "\\u0001", 6 }, // 1
	{ "\\u0002", 6 }, // 2
	{ "\\u0003", 6 }, // 3
	{ "\\u0004", 6 }, // 4
	{ "\\u0005", 6 }, // 5
	{ "\\u0006", 6 }, // 6
	{ "\\u0007", 6 }, // 7
	{ "\\b", 2 }, // 8 \b
	{ "\\t", 2 }, // 9 \t
	{ "\\n", 2 }, // 10 \n
	{ "\\u000b", 6 }, // 11
	{ "\\f", 2 }, // 12 \f
	{ "\\r", 2 }, // 13 \r
	{ "\\u000e", 6 }, // 14
	{ "\\u000f", 6 }, // 15
	{ "\\u0010", 6 }, // 16
	{ "\\u0011", 6 }, // 17
	{ "\\u0012", 6 }, // 18
	{ "\\u0013", 6 }, // 19
	{ "\\u0014", 6 }, // 20
	{ "\\u0015", 6 }, // 21
	{ "\\u0016", 6 }, // 22
	{ "\\u0017", 6 }, // 23
	{ "\\u0018", 6 }, // 24
	{ "\\u0019", 6 }, // 25
	{ "\\u001a", 6 }, // 26
	{ "\\u001b", 6 }, // 27
	{ "\\u001c", 6 }, // 28
	{ "\\u001d", 6 }, // 29
	{ "\\u001e", 6 }, // 30
	{ "\\u001f", 6 }, // 31
}};
const EscapeItem escapeItemQuoteMark = { "\\\"", 2 };
const EscapeItem escapeItemBackSlash = { "\\\\", 2 };

JsonDumper::JsonDumper()
	: JsonDumper(DumperConfig())
{
}

JsonDumper::JsonDumper(const DumperConfig & config)
	: config(config)
{
}

JsonDumper::~JsonDumper()
{
}

std::string JsonDumper::dump(const metapp::Variant & value)
{
	StringOutputter outputter;
	dump(value, TextWriter<StringOutputter>(config, outputter));
	return outputter.getString();
}

} // namespace jsonpp

