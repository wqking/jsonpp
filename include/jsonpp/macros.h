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

#ifndef JSONPP_MACROS_H_821598293712
#define JSONPP_MACROS_H_821598293712

#include "metapp/variant.h"
#include "metapp/allmetatypes.h"
#include "metapp/interfaces/metaclass.h"
#include "metapp/interfaces/metaenum.h"

#define JSONPP_BEGIN_DECLARE_CLASS(cls) \
	template <> \
	struct metapp::DeclareMetaType <cls> : metapp::DeclareMetaTypeBase <cls> { \
		static const metapp::MetaClass * getMetaClass() { \
			static const metapp::MetaClass metaClass( \
				metapp::getMetaType<cls>(), \
				[](metapp::MetaClass & metaClass) { \
					using CurrentClass_ = cls;
#define JSONPP_REGISTER_CLASS_FIELD(field) metaClass.registerAccessible(# field, &CurrentClass_::field);
#define JSONPP_END_DECLARE_CLASS() \
				} \
			); \
			return &metaClass; \
		} \
	};

#define JSONPP_BEGIN_DECLARE_ENUM(e) \
	template <> \
	struct metapp::DeclareMetaType <e> : metapp::DeclareMetaTypeBase <e> { \
		static const metapp::MetaEnum * getMetaEnum() { \
			static const metapp::MetaEnum metaEnum([](metapp::MetaEnum & metaEnum) { \
				using CurrentEnum_ = e;

#define JSONPP_REGISTER_ENUM_VALUE(value) metaEnum.registerValue(# value, CurrentEnum_::value);

#define JSONPP_END_DECLARE_ENUM() \
				} \
			); \
			return &metaEnum; \
		} \
	};

#endif
