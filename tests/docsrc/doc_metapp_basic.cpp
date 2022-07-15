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

#include "testutil.h"

#include "jsonpp/macros.h"
#include "jsonpp/dumper.h"
#include "jsonpp/textoutput.h"

#include "metapp/allmetatypes.h"
#include "metapp/interfaces/metaclass.h"
#include "metapp/interfaces/metaenum.h"

#include <cstring>
#include <sstream>
#include <iostream>

/*desc
# Use metapp library, use classes and enumerators in JSON

## Overview

`jsonpp` uses the powerful [C++ reflection library metapp](https://github.com/wqking/metapp) that's developed by the same
developer (wqking) of jsonpp. Meta data created by `metapp` can not only be used by jsonpp, but also can be used for
many other purposes, such as serialization, script binding, property editor, etc.  
I highly suggest you learn metapp to see its potential. But if you don't have interesting in it, this document helps you
to grasp core metapp technology that's used in jsonpp so you can use jsonpp without dig into metapp.  

## Reflect class and enumerator to be used in jsonpp

To allow jsonpp to stringify or parse JSON object as customized class objects or enumerators, the class and enumerator
must be reflected using metapp.

### Data structure we will use later

Assume we have below data structure for fictional vendor and product.
desc*/

//code
enum ProductDepartment
{
	electronic,
	computer,
	book,
	vehicle,
	software
};

struct Product
{
	std::string name;
	ProductDepartment department;
	int price;

	// Getter and setter for price, to demonstrate how to use getter/setter functions
	int getPrice() const {
		return price;
	}

	void setPrice(const int newPrice) {
		price = newPrice;
	}
};

enum class Country
{
	usa,
	japan,
	germany,
	singapore
};

struct Vendor
{
	std::string name;
	Country country;
	std::vector<Product> products;
	bool publicCompany;
};

// Define compare operator to ease the demon, they are not required if you don't use them in your code.
bool operator == (const Product & a, const Product & b)
{
	return a.name == b.name
		&& a.department == b.department
		&& a.price == b.price
	;
}
bool operator == (const Vendor & a, const Vendor & b)
{
	return a.name == b.name
		&& a.country == b.country
		&& a.products == b.products
		&& a.publicCompany == b.publicCompany
	;
}

//desc ### Declare meta types for the data structure

//desc We want to read and write Vendor from/to JSON document. First step we need to declare meta type for related data structures
//desc so jsonpp can recognize them.

//desc Firstly we need to include proper header. Either `jsonpp/parser.h` or `jsonpp/dumper.h` includes all we need.

#include "jsonpp/parser.h"

//desc Now let's declare meta type for enum ProductDepartment.  
//desc The specialization of `metapp::DeclareMetaType` must be in global namespace.  
//desc To declare meta type for your enumerator, just copy/paste below code, replace `ProductDepartment` with your enum name,
//desc and replace the enum values.

template <>
struct metapp::DeclareMetaType <ProductDepartment> : metapp::DeclareMetaTypeBase <ProductDepartment>
{
	static const metapp::MetaEnum * getMetaEnum() {
		static const metapp::MetaEnum metaEnum([](metapp::MetaEnum & me) {
			me.registerValue("electronic", ProductDepartment::electronic);
			me.registerValue("computer", ProductDepartment::computer);
			me.registerValue("book", ProductDepartment::book);
			me.registerValue("vehicle", ProductDepartment::vehicle);
			me.registerValue("software", ProductDepartment::software);
		});
		return &metaEnum;
	}
};

//desc Now we declare meta type for struct Product. metapp supports many aspects to be registered, such as method, nested classes,
//desc etc. But in jsonpp we only need `registerAccessible`.

template <>
struct metapp::DeclareMetaType <Product> : metapp::DeclareMetaTypeBase <Product>
{
	static const metapp::MetaClass * getMetaClass() {
		static const metapp::MetaClass metaClass(
			metapp::getMetaType<Product>(),
			[](metapp::MetaClass & mc) {
				mc.registerAccessible("name", &Product::name);
				mc.registerAccessible("department", &Product::department);
				mc.registerAccessible("price", metapp::createAccessor(&Product::getPrice, &Product::setPrice));
			}
		);
		return &metaClass;
	}
};

//desc I don't encourage to use macros and I don't provide macros in metapp library.
//desc But for jsonpp users that don't want to dig into metapp and only want to the jsonpp features,
//desc jsonpp provides macros to ease the meta type declaration.
//desc Note: the macros are not required by jsonpp. The code can be rewritten without macros, as how Skill is declared above.  
//desc To use the macros, we need to include "jsonpp/macros.h" explicitly.

#include "jsonpp/macros.h"

//desc Now let's use macros to declare meta type for enum Country
// The macros can't end with ';'
JSONPP_BEGIN_DECLARE_ENUM(Country)
	JSONPP_REGISTER_ENUM_VALUE(usa)
	JSONPP_REGISTER_ENUM_VALUE(japan)
	JSONPP_REGISTER_ENUM_VALUE(germany)
	// The macros expose `metaEnum` which we can use for non-macro code.
	metaEnum.registerValue("singapore", Country::singapore);
JSONPP_END_DECLARE_ENUM()

//desc Now let's use macros to declare meta type for class Vendor
// The macros can't end with ';'
JSONPP_BEGIN_DECLARE_CLASS(Vendor)
	JSONPP_REGISTER_CLASS_FIELD(name)
	JSONPP_REGISTER_CLASS_FIELD(country)
	JSONPP_REGISTER_CLASS_FIELD(products)
	// The macros expose `metaClass` which we can use for non-macro code.
	metaClass.registerAccessible("publicCompany", &Vendor::publicCompany);
JSONPP_END_DECLARE_CLASS()
//code

//desc ### Stringify and parse a single object

//desc Now let's see how to stringify and parse class Vendor

ExampleFunc
{
	//code
	//desc Define a vendor. The data is fake, you know.
	const Vendor vendor {
		"Siemens",
		Country::germany,
		{
			{
				"Fridge",
				ProductDepartment::electronic,
				1158
			},
			{
				"Rail",
				ProductDepartment::vehicle,
				12345678
			},
		},
		true
	};
	//desc Dump the object to JSON, the enum is dumped as name (enableNamedEnum(true)).
	//desc The JSON text looks like,
	//desc {"name":"Siemens","country":"germany","products":[{"name":"Fridge","department":"electronic","price":1158},{"name":"Rail","department":"vehicle","price":12345678}],"publicCompany":true}
	jsonpp::Dumper dumper(jsonpp::DumperConfig().enableNamedEnum(true));
	const std::string jsonText = dumper.dump(vendor);
	//std::cout << std::endl << jsonText << std::endl;
	
	//desc Now let's parse the JSON document back to Vendor
	jsonpp::Parser parser;
	const Vendor parsedVendor = parser.parse<Vendor>(jsonText);
	ASSERT(parsedVendor == vendor);
	//code
}

//desc ### Stringify and parse complicated data struct

//desc We don't stop at single object, we can stringify/parse arbitrary complicated STL data structures.

ExampleFunc
{
	//code
	// ComplicatedType is not complicated enough, you can extend it infinitely.
	using ComplicatedType = std::map<std::string, std::vector<Vendor> >;
	const ComplicatedType vendorListMap {
		{
			"Europe",
			{
				{
					"Siemens",
					Country::germany,
					{
						{
							"Fridge",
							ProductDepartment::electronic,
							1158
						},
						{
							"Rail",
							ProductDepartment::vehicle,
							12345678
						},
					},
					true
				}
			}
		},

		{
			"Asia",
			{
				{
					"Nintendo",
					Country::japan,
					{
						{
							"Super Mario Bros",
							ProductDepartment::software,
							60
						},
						{
							"Donkey Kong",
							ProductDepartment::software,
							50
						},
					},
					true
				}
			}
		},
	};
	//desc Dump the object to JSON, the enum is dumped as name (enableNamedEnum(true)).
	jsonpp::Dumper dumper(jsonpp::DumperConfig().enableNamedEnum(true));
	const std::string jsonText = dumper.dump(vendorListMap);
	//std::cout << std::endl << jsonText << std::endl;
	
	//desc Now let's parse the JSON document back to Vendor
	jsonpp::Parser parser;
	const ComplicatedType parsedVendorListMap = parser.parse<ComplicatedType>(jsonText);
	ASSERT(parsedVendorListMap == vendorListMap);
	//code
}
