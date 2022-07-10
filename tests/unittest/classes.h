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

#ifndef CLASSES_H
#define CLASSES_H

#include "metapp/allmetatypes.h"
#include "metapp/interfaces/metaclass.h"
#include "metapp/interfaces/metaenum.h"

enum class TestEnum1
{
	first = 1,
	second = 2,
	dog = -1000,
	cat = 101
};

struct TestClass1
{
	TestEnum1 e;
	std::string s;
	std::list<std::deque<long> > listDequeLong;
	std::tuple<int, std::string, std::vector<int>, char> tuple;
};

inline bool operator == (const TestClass1 & a, const TestClass1 & b)
{
	return a.e == b.e
		&& a.s == b.s
		&& a.listDequeLong == b.listDequeLong
		&& a.tuple == b.tuple
	;
}

inline TestClass1 makeTestClass1(const int seed)
{
	static TestClass1 objectList[] = {
		{
			TestEnum1::dog,
			"Hello",
			{ { 37065102, -976038618 }, { -1876835186, 516005336, 444334793 }, { 3872225, -1629858 } },
			{ -8, "this", { -16601837, 654321, 0, 2653196, -113703022, 3577582 }, 'w' }
		},

		{
			TestEnum1::first,
			"This is a full sentence, though it's meaningless.",
			{ { 1309825622, -170511008 }, { -3211966, -6, -7 }, { 9 } },
			{ -8, "this", { -123456, 654321, 0, 78, -9, 13579 }, 'q' }
		},

		{
			TestEnum1(1000),
			"Good",
			{ { 1309825622, -170511008 }, { -3211966, -6, -7 }, { 9 } },
			{ -8, "this", { -123456, 654321, 0, 78, -9, 13579 }, 'q' }
		},

	};

	return objectList[seed % (sizeof(objectList) / sizeof(objectList[0]))];
}

struct TestClass2
{
	TestClass2() :
		i(),
		vectorString(),
		obj1(),
		mapStringInt()
	{}

	TestClass2(
		const int i,
		const std::vector<std::string> & vectorString,
		const TestClass1 & obj1,
		const std::map<std::string, int> & mapStringInt
	) :
		i(i),
		vectorString(vectorString),
		obj1(obj1),
		mapStringInt(mapStringInt)
	{}

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

inline bool operator == (const TestClass2 & a, const TestClass2 & b)
{
	return a.i == b.i
		&& a.vectorString == b.vectorString
		&& a.obj1 == b.obj1
		&& a.getMapStringInt() == b.getMapStringInt()
	;
}

inline TestClass2 makeTestClass2(const int seed)
{
	static TestClass2 objectList[] = {
		{
			1,
			{ "hello", "world" },
			makeTestClass1(seed),
			{ { "abc", 1 }, { "def", 2 } }
		},

		{
			-921398,
			{ "this is", "a very long", "sentence, splited in", "several parts." },
			makeTestClass1(seed),
			{ { "dog", 123 }, { "cat", -5 } }
		}

	};

	return objectList[seed % (sizeof(objectList) / sizeof(objectList[0]))];
}

template <>
struct metapp::DeclareMetaType <TestEnum1> : metapp::DeclareMetaTypeBase <TestEnum1>
{
	static const metapp::MetaEnum * getMetaEnum() {
		static const metapp::MetaEnum metaEnum([](metapp::MetaEnum & me) {
			me.registerValue("first", TestEnum1::first);
			me.registerValue("second", TestEnum1::second);
			me.registerValue("dog", TestEnum1::dog);
			me.registerValue("cat", TestEnum1::cat);
			});
		return &metaEnum;
	}
};

template <>
struct metapp::DeclareMetaType <TestClass1> : metapp::DeclareMetaTypeBase <TestClass1>
{
	static const metapp::MetaClass * getMetaClass() {
		static const metapp::MetaClass metaClass(
			metapp::getMetaType<TestClass1>(),
			[](metapp::MetaClass & mc) {
				mc.registerAccessible("e", &TestClass1::e);
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


#endif
