[//]: # (Auto generated file, don't modify this file.)

# Use metapp library, use classes and enumerators in JSON
<!--begintoc-->
- [Overview](#mdtoc_e7c3d1bb)
- [Reflect class and enumerator to be used in jsonpp](#mdtoc_978feee)
  - [Data structure we will use later](#mdtoc_b975e69e)
  - [Declare meta types for the data structure](#mdtoc_83dcc370)
  - [Stringify and parse a single object](#mdtoc_a4cee20f)
  - [Stringify and parse complicated data struct](#mdtoc_4e02b90)
<!--endtoc-->

<a id="mdtoc_e7c3d1bb"></a>
## Overview

`jsonpp` uses the powerful [C++ reflection library metapp](https://github.com/wqking/metapp) that's developed by the same
developer (wqking) of jsonpp. Meta data created by `metapp` can not only be used by jsonpp, but also can be used for
many other purposes, such as serialization, script binding, property editor, etc.  
I highly suggest you learn metapp to see its potential. But if you don't have interesting in it, this document helps you
to grasp core metapp technology that's used in jsonpp so you can use jsonpp without dig into metapp.  

<a id="mdtoc_978feee"></a>
## Reflect class and enumerator to be used in jsonpp

To allow jsonpp to stringify or parse JSON object as customized class objects or enumerators, the class and enumerator
must be reflected using metapp.

<a id="mdtoc_b975e69e"></a>
### Data structure we will use later

Assume we have below data structure for fictional vendor and product.

```c++
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
```

<a id="mdtoc_83dcc370"></a>
### Declare meta types for the data structure
We want to read and write Vendor from/to JSON document. First step we need to declare meta type for related data structures
so jsonpp can recognize them.
Firstly we need to include proper header. Either `jsonpp/parser.h` or `jsonpp/dumper.h` includes all we need.

```c++
#include "jsonpp/parser.h"
```

Now let's declare meta type for enum ProductDepartment.  
The specialization of `metapp::DeclareMetaType` must be in global namespace.  
To declare meta type for your enumerator, just copy/paste below code, replace `ProductDepartment` with your enum name,
and replace the enum values.

```c++
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
```

Now we declare meta type for struct Product. metapp supports many aspects to be registered, such as method, nested classes,
etc. But in jsonpp we only need `registerAccessible`.

```c++
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
```

I don't encourage to use macros and I don't provide macros in metapp library.
But for jsonpp users that don't want to dig into metapp and only want to the jsonpp features,
jsonpp provides macros to ease the meta type declaration.
Note: the macros are not required by jsonpp. The code can be rewritten without macros, as how Skill is declared above.  
To use the macros, we need to include "jsonpp/macros.h" explicitly.

```c++
#include "jsonpp/macros.h"
```

Now let's use macros to declare meta type for enum Country

```c++
// The macros can't end with ';'
JSONPP_BEGIN_DECLARE_ENUM(Country)
  JSONPP_REGISTER_ENUM_VALUE(usa)
  JSONPP_REGISTER_ENUM_VALUE(japan)
  JSONPP_REGISTER_ENUM_VALUE(germany)
  // The macros expose `metaEnum` which we can use for non-macro code.
  metaEnum.registerValue("singapore", Country::singapore);
JSONPP_END_DECLARE_ENUM()
```

Now let's use macros to declare meta type for class Vendor

```c++
// The macros can't end with ';'
JSONPP_BEGIN_DECLARE_CLASS(Vendor)
  JSONPP_REGISTER_CLASS_FIELD(name)
  JSONPP_REGISTER_CLASS_FIELD(country)
  JSONPP_REGISTER_CLASS_FIELD(products)
  // The macros expose `metaClass` which we can use for non-macro code.
  metaClass.registerAccessible("publicCompany", &Vendor::publicCompany);
JSONPP_END_DECLARE_CLASS()
```

<a id="mdtoc_a4cee20f"></a>
### Stringify and parse a single object
Now let's see how to stringify and parse class Vendor
Define a vendor. The data is fake, you know.

```c++
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
```

Dump the object to JSON, the enum is dumped as name (enableNamedEnum(true)).
The JSON text looks like,
{"name":"Siemens","country":"germany","products":[{"name":"Fridge","department":"electronic","price":1158},{"name":"Rail","department":"vehicle","price":12345678}],"publicCompany":true}

```c++
jsonpp::Dumper dumper(jsonpp::DumperConfig().enableNamedEnum(true));
const std::string jsonText = dumper.dump(vendor);
//std::cout << std::endl << jsonText << std::endl;
```

Now let's parse the JSON document back to Vendor

```c++
jsonpp::Parser parser;
const Vendor parsedVendor = parser.parse<Vendor>(jsonText);
ASSERT(parsedVendor == vendor);
```

<a id="mdtoc_4e02b90"></a>
### Stringify and parse complicated data struct
We don't stop at single object, we can stringify/parse arbitrary complicated STL data structures.

```c++
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
```

Dump the object to JSON, the enum is dumped as name (enableNamedEnum(true)).

```c++
jsonpp::Dumper dumper(jsonpp::DumperConfig().enableNamedEnum(true));
const std::string jsonText = dumper.dump(vendorListMap);
//std::cout << std::endl << jsonText << std::endl;
```

Now let's parse the JSON document back to Vendor

```c++
jsonpp::Parser parser;
const ComplicatedType parsedVendorListMap = parser.parse<ComplicatedType>(jsonText);
ASSERT(parsedVendorListMap == vendorListMap);
```
