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

#ifndef JSONPP_ALGORITHMS_I_H_821598293712
#define JSONPP_ALGORITHMS_I_H_821598293712

#include <type_traits>
#include <array>

namespace jsonpp {

namespace internal_ {

constexpr int intToStringBufferSize = 32;

template <typename T, typename Enalbed = void>
struct IntToString;

extern char digitPairList[];

template <typename T>
struct IntToString <T, typename std::enable_if<std::is_unsigned<T>::value>::type>
{
	static int toString(T value, char * buffer) {
		if(value == 0) {
			*buffer = '0';
			return 1;
		}
		char * p = buffer;
		while(value >= 100) {
			const auto index = (value % 100) << 1;
			value /= 100;
			*p-- = digitPairList[index + 1];
			*p-- = digitPairList[index];
		}
		const auto index = value << 1;
		*p-- = digitPairList[index + 1];
		if(value >= 10) {
			*p-- = digitPairList[index];
		}
		return static_cast<int>(buffer - p);
	}
};

template <typename T>
struct IntToString <T, typename std::enable_if<std::is_signed<T>::value>::type>
{
	static int toString(T value, char * buffer) {
		using U = typename std::make_unsigned<T>::type;
		if(value >= 0) {
			return IntToString<U>::toString(static_cast<U>(value), buffer);
		}
		const U unsignedValue = static_cast<U>(-(value + 1)) + 1;
		const int length = IntToString<U>::toString(unsignedValue, buffer);
		*(buffer - length) = '-';
		return length + 1;
	}
};


} // namespace internal_

} // namespace jsonpp

#endif
