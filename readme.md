[//]: # (Auto generated file, don't modify this file.)

# jsonpp -- easy to use C++ JSON parser and stringify library
<!--begintoc-->
- [Features](#mdtoc_bfc0dc13)
- [Basic information](#mdtoc_abc52c05)
  - [License](#mdtoc_5768f419)
  - [Version 0.1.0](#mdtoc_c3ac4dae)
  - [Source code](#mdtoc_6b8a2c23)
  - [Supported compilers](#mdtoc_215a5bea)
- [Quick start](#mdtoc_ea7b0a9)
  - [Namespace](#mdtoc_33e16b56)
  - [Build and install, use jsonpp in your project](#mdtoc_3155c9b5)
- [Example code](#mdtoc_3bb166c4)
  - [Use Variant](#mdtoc_cee017b6)
  - [Inspect MetaType](#mdtoc_3aaa429b)
  - [Reflect a class (declare meta type)](#mdtoc_19cc9779)
  - [Runtime generic algorithm on STL container](#mdtoc_afc73315)
  - [Use reference with Variant](#mdtoc_b8048b76)
  - [Work with unreflected types](#mdtoc_5095655f)
- [Documentations](#mdtoc_aa76f386)
- [Build the test code](#mdtoc_460948ff)
- [Known compiler related quirks in MSVC](#mdtoc_2f938cf5)
- [Motivations](#mdtoc_c55531bd)
<!--endtoc-->

jsonpp is a cross platform C++ json library, focusing on easy to use.  

![C++](https://img.shields.io/badge/C%2B%2B-11-blue)
![Compilers](https://img.shields.io/badge/Compilers-GCC%2FMSVC%2FClang-blue)
![License](https://img.shields.io/badge/License-Apache--2.0-blue)
![CI](https://github.com/wqking/jsonpp/workflows/CI/badge.svg)

<a id="mdtoc_bfc0dc13"></a>
## Features

- **Easy to use, very easy to use, very very ... easy to use**. That means,
  - You can parse and stringify any C++ data structures, include STL containers, classes, etc.
  - For C++ native data types and STL containers, you don't need to provide any meta data to use them, because metapp already provides them.
  - Reflecting meta data for class is very easy. 

- **Reusable meta data**. jsonpp uses meta data from [C++ reflection library metapp](https://github.com/wqking/metapp),
which is not only not special to jsonpp, but also general enough to use for other purpose such as serialization, script binding, etc.

- **Multiple parser backends**. jsonpp uses existing matured and well tested JSON parser libraries as the parser backend, such as
[json-parser](https://github.com/json-parser/json-parser) and [simdjson 2.2.0](https://github.com/simdjson/simdjson). That means you can
choose the best backend to achieve your goals.  

- **Decent performance**. Performance is not jsonpp strength and it's not close to high performance JSON libraries such as
simdjson. However, the performance is better than some existing popular JSON libraries which also focus on usability, thanks to
the high performance simdjson and well optimized metapp.

- Cross platforms, cross compilers.

- Written in standard and portable C++, only require C++11, and support later C++ standard

<a id="mdtoc_abc52c05"></a>
## Basic information

<a id="mdtoc_5768f419"></a>
### License

Apache License, Version 2.0  

<a id="mdtoc_c3ac4dae"></a>
### Version 0.1.0

The project is under working in progress.  
The first stable release will be v1.0.0. 

To put the library to first release, we need to,   
1. Add more tests.
2. Complete the documentations.

You are welcome to try the project and give feedback. Your participation will help to make the development faster and better.

<a id="mdtoc_6b8a2c23"></a>
### Source code

[https://github.com/wqking/jsonpp](https://github.com/wqking/jsonpp)

<a id="mdtoc_215a5bea"></a>
### Supported compilers

jsonpp requires C++ compiler that supports C++11 standard.  
The library is tested with MSVC 2022, 2019, MinGW (Msys) GCC 8.3 and 11.3.0, Clang (carried by MSVC).  
In brief, MSVC, GCC, Clang that has well support for C++11, or released after 2019, should be able to compile the library.

<a id="mdtoc_ea7b0a9"></a>
## Quick start

<a id="mdtoc_33e16b56"></a>
### Namespace

`jsonpp`

<a id="mdtoc_3155c9b5"></a>
### Build and install, use jsonpp in your project

There are various methods to use jsonpp

1, Install using CMake and use it in CMake

If you are going to use jsonpp in CMake managed project, you can install jsonpp then use it in CMake.  
In jsonpp root folder, run the commands,  
```
mkdir build
cd build
cmake ..
sudo make install
```

Then in the project CMakeLists.txt,   
```
# the project target is mytest, just for example
add_executable(mytest test.cpp)

find_package(jsonpp)
if(jsonpp)
target_link_libraries(mytest jsonpp::jsonpp)
else(jsonpp)
message(FATAL_ERROR "jsonpp library is not found")
endif()
```

Note: when using this method with MinGW on Windows, by default CMake will install jsonpp in system folder which is not writable.
You should specify another folder to install.
To do so, replace `cmake ..` with `cmake .. -DCMAKE_INSTALL_PREFIX="YOUR_NEW_LIB_FOLDER"`.

<a id="mdtoc_3bb166c4"></a>
## Example code

Here are some simple code pieces. There are comprehensive tutorials in the documentations.

<a id="mdtoc_cee017b6"></a>
### Use Variant
Header for Variant

```c++
#include "metapp/variant.h"
```

To use all declared meta types, include this header

```c++
#include "metapp/allmetatypes.h"
```

v contains int.

```c++
metapp::Variant v { 5 };
// Get the value
ASSERT(v.get<int>() == 5);
```

cast v to double

```c++
metapp::Variant casted = v.cast<double>();
ASSERT(casted.get<double>() == 5.0);
```

Now v contains char array.

```c++
v = "hello";
ASSERT(strcmp(v.get<char []>(), "hello") == 0);
```

Cast to std::string.

```c++
casted = v.cast<std::string>();
// Get as reference to avoid copy.
ASSERT(casted.get<const std::string &>() == "hello");
```

<a id="mdtoc_3aaa429b"></a>
### Inspect MetaType
Let's inspect the type `const std::map<const int, std::string> * volatile *`

```c++
const metapp::MetaType * metaType = metapp::getMetaType<
  const std::map<const int, std::string> * volatile *>();
ASSERT(metaType->isPointer()); // The type is pointer

// Second level pointer
const metapp::MetaType * secondLevelPointer = metaType->getUpType();
ASSERT(secondLevelPointer->isPointer());
ASSERT(secondLevelPointer->isVolatile()); //second level pointer is volatile

// The pointed type (const std::map<const int, std::string>).
const metapp::MetaType * pointed = secondLevelPointer->getUpType();
ASSERT(pointed->isConst());
ASSERT(pointed->getTypeKind() == metapp::tkStdMap);
// Key type
ASSERT(pointed->getUpType(0)->getTypeKind() == metapp::tkInt);
ASSERT(pointed->getUpType(0)->isConst()); //key is const
// Mapped type.
ASSERT(pointed->getUpType(1)->getTypeKind() == metapp::tkStdString);
```

<a id="mdtoc_19cc9779"></a>
### Reflect a class (declare meta type)
Here is the class we are going to reflect for.

```c++
class MyPet
{
public:
  MyPet() : name(), age() {}
  MyPet(const std::string & name, const int age) : name(name), age(age) {}

  int getAge() const { return age; }
  void setAge(const int newAge) { age = newAge; }

  std::string bark() const { return "Bow-wow, " + name; }
  int calculate(const int a, const int b) const { return a + b; }

  std::string name; // I don't like public field in non-POD, here is only for demo
private:
  int age;
};

// We can use factory function as constructor.
MyPet * createMyPet(const std::string & name, const int birthYear, const int nowYear)
{
  return new MyPet(name, nowYear - birthYear);
}
```

Now let's `DeclareMetaType` for MyPet. We `DeclareMetaType` for all kinds of types,
not only classes, but also enumerators, templates, etc.

```c++
template <>
struct metapp::DeclareMetaType<MyPet> : metapp::DeclareMetaTypeBase<MyPet>
{
  // Reflect the class information via MetaClass.
  static const metapp::MetaClass * getMetaClass() {
    static const metapp::MetaClass metaClass(
      metapp::getMetaType<MyPet>(),
      [](metapp::MetaClass & mc) {
        // Register constructors
        mc.registerConstructor(metapp::Constructor<MyPet ()>());
        mc.registerConstructor(metapp::Constructor<MyPet (const std::string &, int)>());
        // Factory function as constructor
        mc.registerConstructor(&createMyPet);

        // Register field with getter/setter function
        mc.registerAccessible("age",
          metapp::createAccessor(&MyPet::getAge, &MyPet::setAge));
        // Register another field
        mc.registerAccessible("name", &MyPet::name);

        // Register member functions
        mc.registerCallable("bark", &MyPet::bark);
        mc.registerCallable("calculate", &MyPet::calculate);
      }
    );
    return &metaClass;
  }
};
```

Now let's use the reflected meta class.  
Obtain the meta type for MyPet, then get the meta class. If we've registered the meta type of MyPet
to MetaRepo, we can get it at runtime instead of depending on the compile time `getMetaType`.

```c++
const metapp::MetaType * metaType = metapp::getMetaType<MyPet>();
const metapp::MetaClass * metaClass = metaType->getMetaClass();
```

`getConstructor`, then invoke the constructor as if it's a normal callable, with proper arguments.
Then obtain the MyPet instance pointer from the returned Variant and store it in a `std::shared_ptr`.  
The constructor is an overloaded callable since there are three constructors registered,
`metapp::callableInvoke` will choose the proper callable to invoke.

```c++
std::shared_ptr<MyPet> myPet(metapp::callableInvoke(metaClass->getConstructor(), nullptr,
  "Lovely", 3).get<MyPet *>());
// Verify the object is constructed properly.
ASSERT(myPet->name == "Lovely");
ASSERT(myPet->getAge() == 3);
// Call the factory function, the result is same as myPet with name == "Lovely" and age == 3.
std::shared_ptr<MyPet> myPetFromFactory(metapp::callableInvoke(metaClass->getConstructor(), nullptr,
  "Lovely", 2019, 2022).get<MyPet *>());
ASSERT(myPetFromFactory->name == "Lovely");
ASSERT(myPetFromFactory->getAge() == 3);
```

Get field by name then get the value.

```c++
const auto & propertyName = metaClass->getAccessible("name");
ASSERT(metapp::accessibleGet(propertyName, myPet).get<const std::string &>() == "Lovely");
const auto & propertyAge = metaClass->getAccessible("age");
ASSERT(metapp::accessibleGet(propertyAge, myPet).get<int>() == 3);
```

Set field `name` with new value.

```c++
metapp::accessibleSet(propertyName, myPet, "Cute");
ASSERT(metapp::accessibleGet(propertyName, myPet).get<const std::string &>() == "Cute");
```

Get member function then invoke it.

```c++
const auto & methodBark = metaClass->getCallable("bark");
ASSERT(metapp::callableInvoke(methodBark, myPet).get<const std::string &>() == "Bow-wow, Cute");

const auto & methodCalculate = metaClass->getCallable("calculate");
// Pass arguments 2 and 3 to `calculate`, the result is 2+3=5.
ASSERT(metapp::callableInvoke(methodCalculate, myPet, 2, 3).get<int>() == 5);
```

<a id="mdtoc_afc73315"></a>
### Runtime generic algorithm on STL container
Let's define a `concat` function that processes any Variant that implements meta interface MetaIterable

```c++
std::string concat(const metapp::Variant & container)
{
  // `container` may contains a pointer such as T *. We use `metapp::depointer` to convert it to equivalent
  // non-pointer such as T &, that eases the algorithm because we don't care pointer any more.
  const metapp::Variant nonPointer = metapp::depointer(container);
  const metapp::MetaIterable * metaIterable
    = metapp::getNonReferenceMetaType(nonPointer)->getMetaIterable();
  if(metaIterable == nullptr) {
    return "";
  }
  std::stringstream stream;
  metaIterable->forEach(nonPointer, [&stream](const metapp::Variant & item) {
    stream << item;
    return true;
  });
  return stream.str();
}
```

A std::vector of int.

```c++
std::vector<int> container1 { 1, 5, 9, 6, 7 };
```

Construct a Variant with the vector. To avoid container1 being copied, we move the container1 into Variant.

```c++
metapp::Variant v1 = std::move(container1);
ASSERT(container1.empty()); // container1 was moved
```

Concat the items in the vector.

```c++
ASSERT(concat(v1) == "15967");
```

We can also use std::list. Any value can convert to Variant implicitly, so we can pass the container std::list on the fly.

```c++
ASSERT(concat(std::list<std::string>{ "Hello", "World", "Good" }) == "HelloWorldGood");
```

std::tuple is supported too, and we can use heterogeneous types.

```c++
ASSERT(concat(std::make_tuple("A", 1, "B", 2)) == "A1B2");
```

Isn't it cool we can use std::pair as a container?

```c++
ASSERT(concat(std::make_pair("Number", 1)) == "Number1");
```

We can even pass a pointer to container to `concat`.

```c++
std::deque<int> container2 { 1, 2, 3 };
ASSERT(concat(&container2) == "123");
```

<a id="mdtoc_b8048b76"></a>
### Use reference with Variant
Declare a value to be referred to.

```c++
int n = 9;
```

rn holds a reference to n.
C++ equivalence is `int & rn = n;`

```c++
metapp::Variant rn = metapp::Variant::reference(n);
ASSERT(rn.get<int>() == 9);
```

Assign to rn with new value. C++ equivalence is `rn = (int)38.1;` where rn is `int &`.
Here we can't use `rn = 38.1;` where rn is `Variant`, that's different meaning.
See Variant document for details.

```c++
rn.assign(38.1); // different with rn = 38.1, `rn = 38.1` won't modify n
```

rn gets new value.

```c++
ASSERT(rn.get<int>() == 38);
```

n is modified too.

```c++
ASSERT(n == 38);
```

We can use reference to modify container elements as well.  
vs holds a `std::vector<std::string>`.

```c++
metapp::Variant vs(std::vector<std::string> { "Hello", "world" });
ASSERT(vs.get<const std::vector<std::string> &>()[0] == "Hello");
```

Get the first element. The element is returned as a reference.

```c++
metapp::Variant item = metapp::indexableGet(vs, 0);
```

assign to item with new value.

```c++
item.assign("Good");
ASSERT(vs.get<const std::vector<std::string> &>()[0] == "Good");
```

<a id="mdtoc_5095655f"></a>
### Work with unreflected types
Assume we have two types that we don't reflect for.

```c++
struct UnreflectedFoo { int f; };
enum class UnreflectedBar { one, two };
```

Surely we can't get any member function or property information for the classes since they are not reflected.
But at least we can,  
1, Construct the object using default constructor.

```c++
std::unique_ptr<UnreflectedFoo> foo(static_cast<UnreflectedFoo *>(
  metapp::getMetaType<UnreflectedFoo>()->construct()));
ASSERT(foo->f == 0);
```

2, Copy construct the object.

```c++
foo->f = 38; // Change member value to prove the value is copied.
std::unique_ptr<UnreflectedFoo> copiedFoo(static_cast<UnreflectedFoo *>(
  metapp::getMetaType<UnreflectedFoo>()->copyConstruct(foo.get())));
ASSERT(copiedFoo->f == 38);
```

3, Construct the object inplace.

```c++
UnreflectedFoo foo2 { 9 };
ASSERT(foo2.f == 9);
metapp::getMetaType<UnreflectedFoo>()->placementConstruct(&foo2);
ASSERT(foo2.f == 0);
```

4, Copy construct the object inplace.

```c++
foo2.f = 38;
UnreflectedFoo foo3 { 19 };
ASSERT(foo3.f == 19);
metapp::getMetaType<UnreflectedFoo>()->placementCopyConstruct(&foo3, &foo2);
ASSERT(foo3.f == 38);
```

5, Destroy the object, if we dont' use RAII

```c++
void * foo5 = metapp::getMetaType<UnreflectedFoo>()->construct();
// `destroy` knows the type of foo5 and can destroy the `void *` properly.
metapp::getMetaType<UnreflectedFoo>()->destroy(foo5);
```

6, Identify the meta type

```c++
const UnreflectedFoo * fooPtr = &foo3;
metapp::Variant varFoo = fooPtr;
metapp::Variant varBar = UnreflectedBar();
ASSERT(varBar.getMetaType()->equal(metapp::getMetaType<UnreflectedBar>()));
ASSERT(varFoo.getMetaType()->isPointer());
ASSERT(varFoo.getMetaType()->getUpType()->isConst());
ASSERT(varFoo.getMetaType()->equal(metapp::getMetaType<const UnreflectedFoo *>()));
ASSERT(varFoo.getMetaType()->getUpType()->equal(metapp::getMetaType<const UnreflectedFoo>()));
```

<a id="mdtoc_aa76f386"></a>
## Documentations
Below are tutorials and documents. Don't feel upset if you find issues or missing stuff in the documents, I'm not
native English speaker and it's not that exciting to write document. Any way, the code quality is always much better
than the document, for ever.  
If you want to contribute to the documents, be sure to read [How to generate documentations](doc/about_document.md).  

- Tutorials
  - [Use Variant](doc/tutorial/tutorial_variant.md)
  - [Use MetaType](doc/tutorial/tutorial_metatype.md)
  - [Use MetaClass and meta data for class members](doc/tutorial/tutorial_metaclass.md)
  - [Use MetaCallable, function, member function, etc](doc/tutorial/tutorial_callable.md)
  - [Use MetaRepo to retrieve meta data at running time](doc/tutorial/tutorial_metarepo.md)
  - [Use classes inheritance](doc/tutorial/tutorial_metaclass_inheritance.md)

- Core concepts and classes
  - [Core concepts - type kind, meta type, up type, meta interface](doc/core_concepts.md)
  - [Class Variant reference](doc/variant.md)
  - [Class MetaType reference](doc/metatype.md)

- Build and use meta data
  - [Reflect meta type at compile time using DeclareMetaType](doc/declaremetatype.md)
  - [Register and use meta type at running time using MetaRepo](doc/metarepo.md)
  - [MetaItem](doc/metaitem.md)
  - [MetaItemView and BaseView](doc/views.md)

- Meta interfaces
  - [Overview and implement meta interface](doc/meta_interface_overview.md)
  - [MetaClass](doc/interfaces/metaclass.md)
  - [MetaCallable](doc/interfaces/metacallable.md)
  - [MetaAccessible](doc/interfaces/metaaccessible.md)
  - [MetaEnum](doc/interfaces/metaenum.md)
  - [MetaIndexable](doc/interfaces/metaindexable.md)
  - [MetaIterable](doc/interfaces/metaiterable.md)
  - [MetaStreamable](doc/interfaces/metastreamable.md)
  - [MetaMappable](doc/interfaces/metamappable.md)
  - [MetaPointerWrapper](doc/interfaces/metapointerwrapper.md)
  - [User defined meta interface](doc/interfaces/metauser.md)

- Built-in meta types
  - [Overview](doc/metatypes/overview_metatypes.md)
  - [List of all built-in meta types](doc/metatypes/list_all.md)
  - [Constructor](doc/metatypes/constructor.md)
  - [Overloaded function](doc/metatypes/overloaded_function.md)
  - [Default arguments function](doc/metatypes/default_args_function.md)
  - [Variadic function](doc/metatypes/variadic_function.md)
  - [Accessor](doc/metatypes/accessor.md)

- Utilities
  - [utility.h](doc/utilities/utility.md)
  - [TypeList reference](doc/utilities/typelist.md)

- Miscellaneous
  - [Use metapp in dynamic library](doc/dynamic_library.md)
  - [Performance and benchmark](doc/benchmark.md)
  - [Exception and thread safety](doc/exception_thread_safety.md)
  - [Infrequently Asked Questions](doc/faq.md)
  - [About documentations](doc/about_document.md)


<a id="mdtoc_460948ff"></a>
## Build the test code

There are several parts of code to test the library,

- unittests: tests the library.
- docsrc: documentation source code, and sample code to demonstrate how to use the library. 
- benchmark: measure the performance.

All parts are in the `tests` folder.

All parts require CMake to build, and there is a makefile to ease the building.  
Go to folder `tests/build`, then run `make` with different target.
- `make vc19` #generate solution files for Microsoft Visual Studio 2019, then open metapptest.sln in folder project_vc19
- `make vc17` #generate solution files for Microsoft Visual Studio 2017, then open metapptest.sln in folder project_vc17
- `make vc15` #generate solution files for Microsoft Visual Studio 2015, then open metapptest.sln in folder project_vc15
- `make mingw` #build using MinGW
- `make linux` #build on Linux
- `make mingw_coverage` #build using MinGW and generate code coverage report

<a id="mdtoc_2f938cf5"></a>
## Known compiler related quirks in MSVC

MSVC 2022 and 2019, can build the CMake generated test projects and the tests run correctly in Debug and RelWithDebugInfo
configurations. But some tests fail in Release mode when incremental linking is disabled.  
Those failed tests should not fail, because they work correct in MSVC debug mode and in GCC/Clang.
Adding /Debug option to linking which generates debug information makes the tests success.  
Without /Debug option, but enabling incremental linking, will cause the tests success too.  
So if metapp shows weird behavior in MSVC, try to enable incremental linking.

<a id="mdtoc_c55531bd"></a>
## Motivations

I (wqking) developed `cpgf` library since more than 12 years ago. `cpgf` works, but it has several serious problems.
The first problem is it was written in C++03, the code is verbose and difficult to write. The second problem is that
it includes too many features in a single library -- reflection, serialization, script binding (Lua, Python, JavaScript).
`cpgf` became unmanageable and can't be developed in an elegant way.  
`metapp` is a fresh library that only focuses on reflection. Other features, such as serialization,
script binding, will be in separated projects, if they are developed.

