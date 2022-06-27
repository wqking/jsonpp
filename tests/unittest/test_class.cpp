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

#include "test.h"

#include "jsonpp/jsonparser.h"
#include "jsonpp/jsondumper.h"
#include "metapp/interfaces/metaindexable.h"
#include "metapp/interfaces/metaclass.h"
#include "metapp/allmetatypes.h"

#include <deque>
#include <unordered_map>
#include <iostream>

struct TestClass1
{
	std::string s;
	std::list<std::deque<long> > listDequeLong;
	std::tuple<int, std::string, std::vector<int>, char> tuple;
};

bool operator == (const TestClass1 & a, const TestClass1 & b)
{
	return a.s == b.s
		&& a.listDequeLong == b.listDequeLong
		&& a.tuple == b.tuple
	;
}

struct TestClass2
{
	int i;
	std::vector<std::string> vectorString;
	TestClass1 obj1;

	const std::map<std::string, int> & getMapStringInt() const {
		return mapStringInt;
	}

	void setMapStringInt(const std::map<std::string, int> & m) {
		mapStringInt = m;
	}

private:
	std::map<std::string, int> mapStringInt;
};

bool operator == (const TestClass2 & a, const TestClass2 & b)
{
	return a.i == b.i
		&& a.vectorString == b.vectorString
		&& a.obj1 == b.obj1
	;
}

template <>
struct metapp::DeclareMetaType <TestClass1> : metapp::DeclareMetaTypeBase <TestClass1>
{
	static const metapp::MetaClass * getMetaClass() {
		static const metapp::MetaClass metaClass(
			metapp::getMetaType<TestClass1>(),
			[](metapp::MetaClass & mc) {
				mc.registerAccessible("s", &TestClass1::s);
				mc.registerAccessible("listDequeLong", &TestClass1::listDequeLong);
				mc.registerAccessible("tuple", &TestClass1::tuple);
			}
		);
		return &metaClass;
	}
};

template <>
struct metapp::DeclareMetaType <TestClass2> : metapp::DeclareMetaTypeBase <TestClass2>
{
	static const metapp::MetaClass * getMetaClass() {
		static const metapp::MetaClass metaClass(
			metapp::getMetaType<TestClass2>(),
			[](metapp::MetaClass & mc) {
				mc.registerAccessible("i", &TestClass2::i);
				mc.registerAccessible("vectorString", &TestClass2::vectorString);
				mc.registerAccessible("obj1", &TestClass2::obj1);
				mc.registerAccessible("mapStringInt",
					metapp::createAccessor(&TestClass2::getMapStringInt, &TestClass2::setMapStringInt));
			}
		);
		return &metaClass;
	}
};

TEST_CASE("Test class, TestClass2")
{
	TestClass2 obj;
	obj.i = 38;
	obj.vectorString.push_back("Hello");
	obj.vectorString.push_back("world");
	obj.obj1.s = "Good";
	obj.obj1.listDequeLong.push_back({ 5, 6, 1999 });
	obj.obj1.listDequeLong.push_back({ 12356789, -38, 98765, 0, -10000 });
	obj.obj1.tuple = { 96, "This is tuple", { 9, 8, 7 }, 'w' };
	obj.setMapStringInt({ { "a", 1 }, { "b", 2 } });

	jsonpp::DumperConfig config;
	config.setBeautify(true);
	jsonpp::JsonDumper dumper(config);
	const std::string jsonText = dumper.dump(metapp::Variant::create<TestClass2 &>(obj));
//std::cout << jsonText << std::endl;
	jsonpp::JsonParser parser;
	metapp::Variant var = parser.parse(jsonText, metapp::getMetaType<TestClass2>());
	const TestClass2 & parsed = var.get<const TestClass2 &>();
	REQUIRE(obj == parsed);
}

