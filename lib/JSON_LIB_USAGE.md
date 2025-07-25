# cpputil::json::JsonParam —— JSON 解析与类型安全访问库

## 简介

`cpputil::json::JsonParam` 是基于 RapidJSON 封装的 C++ JSON 解析与类型安全访问库，支持通过路径表达式和模板类型递归安全地获取和设置 JSON 数据，适用于现代 C++ 项目。

## 主要特性
- 基于 RapidJSON，性能优异
- 支持路径访问（JsonPath），可链式访问嵌套对象/数组
- 支持类型安全的 `get<T>` 和 `set<T>`，T 可为 int/double/string/vector/map 等递归组合
- 支持默认值，类型不匹配时自动返回默认值
- 支持 `std::map`、`std::unordered_map`、`std::vector` 等 STL 容器
- 递归类型推断，支持任意嵌套 map/vector
- 自动路径创建，set 时自动创建不存在的路径
- 友好的错误处理

## 典型用法

```cpp
#include "lib/json.h"
using namespace cpputil::json;

std::string json_str = R"({
    "user": {
        "name": "Alice",
        "age": 25,
        "tags": ["dev", "cpp"],
        "scores": [90, 85, 95],
        "profile": {
            "email": "alice@example.com"
        },
        "friends": [
            {"id": 1, "name": "Bob"},
            {"id": 2, "name": "Carol"}
        ]
    }
})";

JsonParam j(json_str);

// 基本类型
std::string name = j.get({"user", "name"}, std::string("default"));
int age = j.get({"user", "age"}, 0);
bool valid = j.isValid();

// 数组
std::vector<int> scores = j.get({"user", "scores"}, std::vector<int>{});

// 嵌套对象
std::string email = j.get({"user", "profile", "email"}, std::string("none"));

// vector<map<string, string>>
auto friends = j.get({"user", "friends"}, std::vector<std::map<std::string, std::string>>{});

// map<string, vector<int>>
auto m = j.get({"user"}, std::map<std::string, std::vector<int>>{});

// 递归 map<string, map<string, string>>
auto m2 = j.get({"user"}, std::map<std::string, std::map<std::string, std::string>>{});
```

## 支持的类型说明

- `int`, `double`, `bool`, `std::string`
- `std::vector<T>`，T 可为上述类型或递归容器
- `std::map<std::string, T>`，T 可为上述类型或递归容器
- `std::unordered_map<std::string, T>`，T 可为上述类型或递归容器
- 任意嵌套组合，如 `vector<map<string, int>>`、`map<string, vector<double>>`、`vector<map<string, vector<string>>>` 等

## get<T> 行为细节

- 若类型匹配，返回实际值
- 若类型不匹配，返回 `default_value`（如用 `get<string>` 取 int 字段，返回空字符串）
- 支持递归类型推断，map/vector 可嵌套任意层
- 支持默认值参数，未找到路径或类型不符时返回
- 支持列表初始化路径：`j.get({"a", size_t(1), "b"}, ...);`

## set<T> 行为细节

- 支持设置基本类型：`string`、`int`、`double`、`bool`
- 支持设置容器类型：`vector<T>`、`map<string, T>`、`unordered_map<string, T>`
- 支持递归类型设置，如 `vector<map<string, int>>`、`map<string, vector<double>>` 等
- 自动路径创建：如果路径不存在，会自动创建中间路径
- 支持列表初始化路径：`j.set({"a", size_t(1), "b"}, value);`
- 返回 `bool` 表示操作是否成功
- 支持覆盖已存在的值

## Set 操作示例

```cpp
// 基本类型设置
j.set({"user", "new_field"}, std::string("new_value"));
j.set({"user", "age"}, 26);
j.set({"user", "height"}, 175.5);
j.set({"user", "active"}, true);

// 数组设置
j.set({"user", "scores"}, std::vector<int>{95, 88, 92});
j.set({"user", "tags"}, std::vector<std::string>{"dev", "cpp", "json"});

// 嵌套对象设置
j.set({"user", "profile", "email"}, std::string("new@example.com"));
j.set({"user", "profile", "phone"}, std::string("123-456-7890"));

// 复杂嵌套结构设置
std::vector<std::map<std::string, int>> complex_data = {
    {{"id", 1}, {"score", 100}},
    {{"id", 2}, {"score", 200}}
};
j.set({"user", "complex_data"}, complex_data);

// 数组元素设置
j.set({"user", "friends", size_t(0)}, std::map<std::string, std::string>{{"name", "David"}});
j.set({"user", "friends", size_t(1)}, std::map<std::string, std::string>{{"name", "Eve"}});
```

## JsonPath 说明

- `JsonPath` 用于描述 JSON 路径，支持字符串 key 和数组下标
- 可通过列表初始化直接传递：`{"a", size_t(1), "b"}`
- 也可手动构造：
  ```cpp
  JsonPath p;
  p.add("a");
  p.add(size_t(1));
  p.add("b");
  j.get(p, ...);
  ```

## 错误处理与默认值机制

- 路径不存在或类型不匹配时，返回 `default_value`
- 递归类型不匹配时，子节点也返回对应类型的默认值
- JSON 解析失败时，`isValid()` 返回 false，所有 `get` 返回默认值

## 扩展建议

- 如需支持自定义类型，可扩展 `parseValue` 模板
- 如需支持更多 STL 容器，可仿照 vector/map 实现
- 如需支持非 string key 的 map，可自行扩展

## 维护者

- 作者：ackongsun
- 基于 RapidJSON 封装
- 反馈/建议请联系维护者 