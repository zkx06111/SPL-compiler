# how to use `sem`

## Include

in C++:

```c++
#include "TypeUtil.h"
```

in CMake (need C++ 17):

```cmake
target_link_libraries(${TARGET_NAME}
	PUBLIC sem
)
```

## Classes

* `Type`: 记录类型的类型、层次、和一个下标。要数组等复杂类型的信息，需要调用 `GetArray`、`GetRecord` 等函数。
* `Enum`：记录该枚举定义的每个符合。
* `Array`：记录下标类型和元素类型。
* `Subrange`：记录元素类型和上下界。
* `Record`：记录成员名及其类型。
* `Const`：记录常数的类型和值。（枚举定义的被记做是类型为枚举类型的 int 常量）
* `Func`：函数/过程的返回值（过程的为 void）、各个参数的类型。

## Usage

### New Type

新建一个类型，返回值是 `std::pair<Type, std::string>`。成功返回相应类型，失败返回 void 类型，string 存错误信息。

```c++
sem::sym_t.NewEnum(names); // name: vector<string>
sem::sym_t.NewIntSubrange(l, r);
sem::sym_t.NewCharSubrange(l, r);
sem::sym_t.NewBoolSubrange(l, r);
sem::sym_t.NewEnumSubrange(l, r); // l, r: string
sem::sym_t.NewArray(ind_type, ele_type); // ind_type, ele_type: Type
sem::sym_t.NewRecord(data); // data: vector<pair<string, Type>>
```

命名类型（即 `type xxx = blabla`），返回值是 `std::string`，记录失败信息，成功为 `""`。

```c++
sem::sym_t.NameType(name, type); // name: string, type: Type
```

### Scope

新建或结束一个作用域，结束作用域时，最新的作用域内的内容会消失。

应该是不需要主动调用 `NewScope()` 的。。。

```c++
sem::sym_t.NewScope();
sem::sym_t.EndScope();
```

### New Constant

新建常量，返回值是 `std::string`。

```c++
sem::sym_t.NewConst(name, val); // name: string, val: int/double/bool/char/string
```

### New Variable

新建变量，返回值是 `std::string`。

```c++
sem::sym_t.NewVarible(name, type); // name: string, type: Type
```

### New Function/Procedure

新建函数，返回值是 `std::string`。会新建作用域。

```c++
sem::sym_t.NewFunc(name, ret, args); // name: string, ret: Type, args: vector<pair<string, Type>>
sem::sym_t.NewProc(name, args); // name: string, args: vector<pair<string, Type>>
```

### Check XXX

检查名称为 blabla 的 Id/Type/Enum（指枚举定义的那些符号）/Var/Const/Func 是否存在。

### Get XXX

获取名称为 blabla 的 xxx 的类型，返回值为 Type。Enum（指枚举定义的那些符号）

```c++
sem::sym_t.GetType(name); // name: string
sem::sym_t.GetEnumType(name); // name: string
sem::sym_t.GetVarType(name); // name: string
sem::sym_t.GetConstType(name); // name: string
sem::sym_t.GetFunc(name); // name: string
```

获取常数值。枚举算 int。

```c++
int i = sem::sym_t.GetConstValI(name); // name: string
double d = sem::sym_t.GetConstValR(name); // name: string
char c = sem::sym_t.GetConstValC(name); // name: string
bool b = sem::sym_t.GetConstValB(name); // name: string
std::string s = sem::sym_t.GetConstValS(name); // name: string
```

根据 Type 获取数组等复杂类型的具体内容。

```c++
Enum enm = sem::sym_t.GetEnum(type) // type: Type
Subrange = sem::sym_t.GetSubrange(type) // type: Type
Array = sem::sym_t.GetArray(type) // type: Type
Record = sem::sym_t.GetRecord(type) // type: Type
```

## Type Utilities

类型相等判断：

用 `==` 判的是严格的相等（使用 `type name = T` 定义的类型别名与原类型相等）。

用 `IsAlmostSame(a, b)` 比 `==` 松一点，会从 subrange 中取出类型比较，比如 int 的 subrange 和 int 相等。

`RemoveSubrange(type)` 用于取出 Subrange 的类型，对其他类型无影响。

## TODO

* [ ] add system function to table
* [ ] label
* [ ] maybe others...