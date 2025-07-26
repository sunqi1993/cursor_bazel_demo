#include <gtest/gtest.h>
#include "lib/json.h"
#include <map>
#include <unordered_map>
#include <vector>

namespace cpputil {
namespace json {
namespace {

class JsonTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 测试用的 JSON 字符串
        json_str_ = R"({
            "name": "John Doe",
            "age": 30,
            "is_student": false,
            "height": 175.5,
            "scores": [85, 92, 78],
            "address": {
                "street": "123 Main St",
                "city": "New York",
                "zip": "10001"
            },
            "hobbies": ["reading", "swimming", "coding"],
            "grades": {
                "math": 95,
                "english": 88,
                "science": 92
            },
            "nested_data": {
                "user_info": {
                    "name": "Alice",
                    "age": 25
                },
                "scores_list": [90, 85, 95]
            },
            "complex_array": [
                {"id": 1, "name": "Item1"},
                {"id": 2, "name": "Item2"}
            ]
        })";
        
        json_obj_ = std::make_unique<JsonParam>(json_str_);
    }

    std::string json_str_;
    std::unique_ptr<JsonParam> json_obj_;
};

TEST_F(JsonTest, ConstructorWithValidJson) {
    EXPECT_TRUE(json_obj_->isValid());
}

TEST_F(JsonTest, ConstructorWithInvalidJson) {
    JsonParam invalid_json("invalid json string");
    EXPECT_FALSE(invalid_json.isValid());
}

TEST_F(JsonTest, GetStringValue) {
    std::string result = json_obj_->get({"name"}, std::string("Default"));
    EXPECT_EQ(result, "John Doe");
}

TEST_F(JsonTest, GetIntValue) {
    int result = json_obj_->get({"age"}, 0);
    EXPECT_EQ(result, 30);
}

TEST_F(JsonTest, GetDoubleValue) {
    double result = json_obj_->get({"height"}, 0.0);
    EXPECT_DOUBLE_EQ(result, 175.5);
}

TEST_F(JsonTest, GetBoolValue) {
    bool result = json_obj_->get({"is_student"}, true);
    EXPECT_FALSE(result);
}

TEST_F(JsonTest, GetArrayElement) {
    int result = json_obj_->get({"scores", size_t(0)}, 0);
    EXPECT_EQ(result, 85);
}

TEST_F(JsonTest, GetNestedObjectValue) {
    std::string result = json_obj_->get({"address", "city"}, std::string("Unknown"));
    EXPECT_EQ(result, "New York");
}

TEST_F(JsonTest, GetComplexPath) {
    int result = json_obj_->get({"grades", "math"}, 0);
    EXPECT_EQ(result, 95);
}

TEST_F(JsonTest, GetArrayStringElement) {
    std::string result = json_obj_->get({"hobbies", size_t(1)}, std::string("Unknown"));
    EXPECT_EQ(result, "swimming");
}

TEST_F(JsonTest, GetNonexistentPath) {
    std::string result = json_obj_->get({"nonexistent"}, std::string("Default Value"));
    EXPECT_EQ(result, "Default Value");
}

TEST_F(JsonTest, GetNonexistentNestedPath) {
    std::string result = json_obj_->get({"address", "nonexistent"}, std::string("Default"));
    EXPECT_EQ(result, "Default");
}

TEST_F(JsonTest, GetArrayOutOfBounds) {
    int result = json_obj_->get({"scores", size_t(10)}, -1);
    EXPECT_EQ(result, -1);
}

TEST_F(JsonTest, GetInvalidArrayIndex) {
    std::string result = json_obj_->get({"address", size_t(0)}, std::string("Default")); // address is object, not array
    EXPECT_EQ(result, "Default");
}

TEST_F(JsonTest, HasExistingPath) {
    JsonPath path({"name"});
    EXPECT_TRUE(json_obj_->has(path));
}

TEST_F(JsonTest, HasNonexistentPath) {
    JsonPath path({"nonexistent"});
    EXPECT_FALSE(json_obj_->has(path));
}

TEST_F(JsonTest, HasNestedPath) {
    JsonPath path({"address", "street"});
    EXPECT_TRUE(json_obj_->has(path));
}

TEST_F(JsonTest, HasArrayElement) {
    JsonPath path({"scores", size_t(0)});
    EXPECT_TRUE(json_obj_->has(path));
}

TEST_F(JsonTest, HasArrayOutOfBounds) {
    JsonPath path({"scores", size_t(10)});
    EXPECT_FALSE(json_obj_->has(path));
}

TEST_F(JsonTest, ToString) {
    std::string result = json_obj_->toString();
    EXPECT_FALSE(result.empty());
    EXPECT_NE(result, "");
}

TEST_F(JsonTest, JsonPathListInitialization) {
    // 测试列表初始化
    JsonPath path1 = {"name"};
    JsonPath path2 = {"scores", size_t(0)};
    JsonPath path3 = {"address", "city"};
    
    EXPECT_EQ(path1.size(), 1);
    EXPECT_EQ(path2.size(), 2);
    EXPECT_EQ(path3.size(), 2);
    
    // 验证路径元素
    EXPECT_TRUE(std::holds_alternative<std::string>(path1.elements()[0]));
    EXPECT_EQ(std::get<std::string>(path1.elements()[0]), "name");
    
    EXPECT_TRUE(std::holds_alternative<std::string>(path2.elements()[0]));
    EXPECT_TRUE(std::holds_alternative<size_t>(path2.elements()[1]));
    EXPECT_EQ(std::get<std::string>(path2.elements()[0]), "scores");
    EXPECT_EQ(std::get<size_t>(path2.elements()[1]), 0);
}

TEST_F(JsonTest, JsonPathEmpty) {
    JsonPath empty_path;
    EXPECT_TRUE(empty_path.empty());
    EXPECT_EQ(empty_path.size(), 0);
}

TEST_F(JsonTest, JsonPathAddElements) {
    JsonPath path;
    path.add("test");
    path.add(1);
    
    EXPECT_EQ(path.size(), 2);
    EXPECT_TRUE(std::holds_alternative<std::string>(path.elements()[0]));
    EXPECT_TRUE(std::holds_alternative<size_t>(path.elements()[1]));
}

TEST_F(JsonTest, SimplifiedGetInterface) {
    // 测试简化的 get 接口
    JsonPath path = {"test", size_t(0), "nested"};
    EXPECT_EQ(path.size(), 3);
    EXPECT_EQ(std::get<std::string>(path.elements()[0]), "test");
    EXPECT_EQ(std::get<size_t>(path.elements()[1]), 0);
    EXPECT_EQ(std::get<std::string>(path.elements()[2]), "nested");
}

// Map 相关测试
TEST_F(JsonTest, GetMapStringString) {
    std::map<std::string, std::string> result = json_obj_->get({"address"}, std::map<std::string, std::string>{});
    
    EXPECT_EQ(result.size(), 3);
    EXPECT_EQ(result["street"], "123 Main St");
    EXPECT_EQ(result["city"], "New York");
    EXPECT_EQ(result["zip"], "10001");
}

TEST_F(JsonTest, GetUnorderedMapStringString) {
    std::unordered_map<std::string, std::string> result = json_obj_->get({"address"}, std::unordered_map<std::string, std::string>{});
    
    EXPECT_EQ(result.size(), 3);
    EXPECT_EQ(result["street"], "123 Main St");
    EXPECT_EQ(result["city"], "New York");
    EXPECT_EQ(result["zip"], "10001");
}

TEST_F(JsonTest, GetMapStringInt) {
    std::map<std::string, int> result = json_obj_->get({"grades"}, std::map<std::string, int>{});
    
    EXPECT_EQ(result.size(), 3);
    EXPECT_EQ(result["math"], 95);
    EXPECT_EQ(result["english"], 88);
    EXPECT_EQ(result["science"], 92);
}

TEST_F(JsonTest, GetUnorderedMapStringInt) {
    std::unordered_map<std::string, int> result = json_obj_->get({"grades"}, std::unordered_map<std::string, int>{});
    
    EXPECT_EQ(result.size(), 3);
    EXPECT_EQ(result["math"], 95);
    EXPECT_EQ(result["english"], 88);
    EXPECT_EQ(result["science"], 92);
}

TEST_F(JsonTest, GetMapStringDouble) {
    // 创建一个包含 double 值的 JSON 对象
    std::string double_json_str = R"({
        "measurements": {
            "height": 175.5,
            "weight": 70.2,
            "bmi": 22.8
        }
    })";
    JsonParam double_json_obj(double_json_str);
    
    std::map<std::string, double> result = double_json_obj.get({"measurements"}, std::map<std::string, double>{});
    
    EXPECT_EQ(result.size(), 3);
    EXPECT_DOUBLE_EQ(result["height"], 175.5);
    EXPECT_DOUBLE_EQ(result["weight"], 70.2);
    EXPECT_DOUBLE_EQ(result["bmi"], 22.8);
}

TEST_F(JsonTest, GetMapFromNonexistentPath) {
    std::map<std::string, std::string> default_map = {{"default", "value"}};
    std::map<std::string, std::string> result = json_obj_->get({"nonexistent"}, default_map);
    
    EXPECT_EQ(result, default_map);
}

TEST_F(JsonTest, GetMapFromNonObjectPath) {
    std::map<std::string, std::string> default_map = {{"default", "value"}};
    std::map<std::string, std::string> result = json_obj_->get({"name"}, default_map); // name is string, not object
    
    EXPECT_EQ(result, default_map);
}

// 新增：Vector 类型测试
TEST_F(JsonTest, GetVectorString) {
    std::vector<std::string> result = json_obj_->get({"hobbies"}, std::vector<std::string>{});
    
    EXPECT_EQ(result.size(), 3);
    EXPECT_EQ(result[0], "reading");
    EXPECT_EQ(result[1], "swimming");
    EXPECT_EQ(result[2], "coding");
}

TEST_F(JsonTest, GetVectorInt) {
    std::vector<int> result = json_obj_->get({"scores"}, std::vector<int>{});
    
    EXPECT_EQ(result.size(), 3);
    EXPECT_EQ(result[0], 85);
    EXPECT_EQ(result[1], 92);
    EXPECT_EQ(result[2], 78);
}

TEST_F(JsonTest, GetVectorDouble) {
    std::vector<double> result = json_obj_->get({"scores"}, std::vector<double>{});
    
    EXPECT_EQ(result.size(), 3);
    EXPECT_DOUBLE_EQ(result[0], 85.0);
    EXPECT_DOUBLE_EQ(result[1], 92.0);
    EXPECT_DOUBLE_EQ(result[2], 78.0);
}

// 新增：递归类型测试
TEST_F(JsonTest, GetNestedMap) {
    std::map<std::string, std::map<std::string, std::string>> result = 
        json_obj_->get({"nested_data"}, std::map<std::string, std::map<std::string, std::string>>{});
    
    EXPECT_EQ(result.size(), 2);
    EXPECT_EQ(result["user_info"]["name"], "Alice");
    EXPECT_EQ(result["user_info"]["age"], ""); // age 是 int 类型，转换为 string 时返回默认值
}

TEST_F(JsonTest, GetMapWithVector) {
    std::map<std::string, std::vector<int>> result = 
        json_obj_->get({"nested_data"}, std::map<std::string, std::vector<int>>{});
    
    EXPECT_EQ(result.size(), 2);
    EXPECT_EQ(result["scores_list"].size(), 3);
    EXPECT_EQ(result["scores_list"][0], 90);
    EXPECT_EQ(result["scores_list"][1], 85);
    EXPECT_EQ(result["scores_list"][2], 95);
}

TEST_F(JsonTest, GetVectorOfMaps) {
    std::vector<std::map<std::string, std::string>> result = 
        json_obj_->get({"complex_array"}, std::vector<std::map<std::string, std::string>>{});
    
    EXPECT_EQ(result.size(), 2);
    EXPECT_EQ(result[0]["id"], ""); // id 是 int 类型，转换为 string 时返回默认值
    EXPECT_EQ(result[0]["name"], "Item1");
    EXPECT_EQ(result[1]["id"], ""); // id 是 int 类型，转换为 string 时返回默认值
    EXPECT_EQ(result[1]["name"], "Item2");
}

TEST_F(JsonTest, GetVectorOfMapsWithInt) {
    std::vector<std::map<std::string, int>> result = 
        json_obj_->get({"complex_array"}, std::vector<std::map<std::string, int>>{});
    
    EXPECT_EQ(result.size(), 2);
    EXPECT_EQ(result[0]["id"], 1);
    EXPECT_EQ(result[0]["name"], 0); // name 不是 int，返回默认值
    EXPECT_EQ(result[1]["id"], 2);
    EXPECT_EQ(result[1]["name"], 0);
}

// Set 方法测试
TEST_F(JsonTest, SetStringValue) {
    bool success = json_obj_->set({"new_field"}, std::string("new_value"));
    EXPECT_TRUE(success);
    
    std::string result = json_obj_->get({"new_field"}, std::string(""));
    EXPECT_EQ(result, "new_value");
}

TEST_F(JsonTest, SetIntValue) {
    bool success = json_obj_->set({"new_int"}, 42);
    EXPECT_TRUE(success);
    
    int result = json_obj_->get({"new_int"}, 0);
    EXPECT_EQ(result, 42);
}

TEST_F(JsonTest, SetDoubleValue) {
    bool success = json_obj_->set({"new_double"}, 3.14);
    EXPECT_TRUE(success);
    
    double result = json_obj_->get({"new_double"}, 0.0);
    EXPECT_DOUBLE_EQ(result, 3.14);
}

TEST_F(JsonTest, SetBoolValue) {
    bool success = json_obj_->set({"new_bool"}, true);
    EXPECT_TRUE(success);
    
    bool result = json_obj_->get({"new_bool"}, false);
    EXPECT_TRUE(result);
}

TEST_F(JsonTest, SetVectorValue) {
    std::vector<int> new_vector = {1, 2, 3, 4, 5};
    bool success = json_obj_->set({"new_vector"}, new_vector);
    EXPECT_TRUE(success);
    
    std::vector<int> result = json_obj_->get({"new_vector"}, std::vector<int>{});
    EXPECT_EQ(result, new_vector);
}

TEST_F(JsonTest, SetMapValue) {
    std::map<std::string, std::string> new_map = {{"key1", "value1"}, {"key2", "value2"}};
    bool success = json_obj_->set({"new_map"}, new_map);
    EXPECT_TRUE(success);
    
    std::map<std::string, std::string> result = json_obj_->get({"new_map"}, std::map<std::string, std::string>{});
    EXPECT_EQ(result, new_map);
}

TEST_F(JsonTest, SetNestedPath) {
    bool success = json_obj_->set({"nested", "deep", "value"}, std::string("deep_value"));
    EXPECT_TRUE(success);
    
    std::string result = json_obj_->get({"nested", "deep", "value"}, std::string(""));
    EXPECT_EQ(result, "deep_value");
}

TEST_F(JsonTest, SetArrayElement) {
    bool success = json_obj_->set({"new_array", size_t(0)}, std::string("first"));
    EXPECT_TRUE(success);
    
    success = json_obj_->set({"new_array", size_t(1)}, std::string("second"));
    EXPECT_TRUE(success);
    
    std::string result1 = json_obj_->get({"new_array", size_t(0)}, std::string(""));
    std::string result2 = json_obj_->get({"new_array", size_t(1)}, std::string(""));
    EXPECT_EQ(result1, "first");
    EXPECT_EQ(result2, "second");
}

TEST_F(JsonTest, SetComplexNestedStructure) {
    // 设置复杂的嵌套结构
    std::vector<std::map<std::string, int>> complex_data = {
        {{"id", 1}, {"score", 100}},
        {{"id", 2}, {"score", 200}}
    };
    
    bool success = json_obj_->set({"complex_data"}, complex_data);
    EXPECT_TRUE(success);
    
    auto result = json_obj_->get({"complex_data"}, std::vector<std::map<std::string, int>>{});
    EXPECT_EQ(result, complex_data);
}

TEST_F(JsonTest, SetMapWithVector) {
    std::map<std::string, std::vector<int>> map_with_vector = {
        {"scores", {90, 85, 95}},
        {"grades", {88, 92, 87}}
    };
    
    bool success = json_obj_->set({"map_with_vector"}, map_with_vector);
    EXPECT_TRUE(success);
    
    auto result = json_obj_->get({"map_with_vector"}, std::map<std::string, std::vector<int>>{});
    EXPECT_EQ(result, map_with_vector);
}

TEST_F(JsonTest, SetExistingValue) {
    // 设置已存在的值
    bool success = json_obj_->set({"name"}, std::string("New Name"));
    EXPECT_TRUE(success);
    
    std::string result = json_obj_->get({"name"}, std::string(""));
    EXPECT_EQ(result, "New Name");
}

TEST_F(JsonTest, SetInvalidPath) {
    // 测试无效路径（空路径）
    bool success = json_obj_->set({}, std::string("value"));
    EXPECT_FALSE(success);
}

TEST_F(JsonTest, SetWithSimplifiedInterface) {
    // 测试简化的 set 接口
    bool success = json_obj_->set({"simplified", "test"}, std::string("works"));
    EXPECT_TRUE(success);
    
    std::string result = json_obj_->get({"simplified", "test"}, std::string(""));
    EXPECT_EQ(result, "works");
}

} // namespace
} // namespace json
} // namespace cpputil 

// 新增 update 方法的测试用例
TEST(JsonParamTest, UpdateBasicMerge) {
    std::string json1 = R"({
        "name": "Alice",
        "age": 25
    })";
    
    std::string json2 = R"({
        "age": 26,
        "city": "New York"
    })";
    
    cpputil::json::JsonParam js1(json1);
    cpputil::json::JsonParam js2(json2);
    
    EXPECT_TRUE(js1.update(js2));
    
    // 验证合并结果
    EXPECT_EQ(js1.get({"name"}, std::string("")), "Alice");  // 保持原值
    EXPECT_EQ(js1.get({"age"}, 0), 26);                     // 被覆盖
    EXPECT_EQ(js1.get({"city"}, std::string("")), "New York"); // 新增
}

TEST(JsonParamTest, UpdateNestedObject) {
    std::string json1 = R"({
        "user": {
            "name": "Alice",
            "profile": {
                "email": "alice@old.com"
            }
        }
    })";
    
    std::string json2 = R"({
        "user": {
            "age": 25,
            "profile": {
                "phone": "123-456-7890",
                "email": "alice@new.com"
            }
        }
    })";
    
    cpputil::json::JsonParam js1(json1);
    cpputil::json::JsonParam js2(json2);
    
    EXPECT_TRUE(js1.update(js2));
    
    // 验证嵌套合并结果
    EXPECT_EQ(js1.get({"user", "name"}, std::string("")), "Alice");
    EXPECT_EQ(js1.get({"user", "age"}, 0), 25);
    EXPECT_EQ(js1.get({"user", "profile", "email"}, std::string("")), "alice@new.com");
    EXPECT_EQ(js1.get({"user", "profile", "phone"}, std::string("")), "123-456-7890");
}

TEST(JsonParamTest, UpdateArray) {
    std::string json1 = R"({
        "scores": [85, 92, 78]
    })";
    
    std::string json2 = R"({
        "scores": [95, 88]
    })";
    
    cpputil::json::JsonParam js1(json1);
    cpputil::json::JsonParam js2(json2);
    
    EXPECT_TRUE(js1.update(js2));
    
    // 验证数组合并结果（数组会被追加）
    auto scores = js1.get<std::vector<int>>({"scores"});
    EXPECT_EQ(scores.size(), 5);  // 原来3个 + 新增2个
    EXPECT_EQ(scores[0], 85);     // 原来的元素
    EXPECT_EQ(scores[1], 92);
    EXPECT_EQ(scores[2], 78);
    EXPECT_EQ(scores[3], 95);     // 新增的元素
    EXPECT_EQ(scores[4], 88);
}

TEST(JsonParamTest, UpdateArrayOverwrite) {
    std::string json1 = R"({
        "config": {
            "values": [1, 2, 3]
        }
    })";
    
    std::string json2 = R"({
        "config": {
            "values": [4, 5],
            "enabled": true
        }
    })";
    
    cpputil::json::JsonParam js1(json1);
    cpputil::json::JsonParam js2(json2);
    
    EXPECT_TRUE(js1.update(js2));
    
    // 验证嵌套对象中的数组合并
    auto values = js1.get<std::vector<int>>({"config", "values"});
    EXPECT_EQ(values.size(), 5);  // 3 + 2
    EXPECT_EQ(js1.get({"config", "enabled"}, false), true);
}

TEST(JsonParamTest, UpdateInvalidJson) {
    std::string json1 = R"({"name": "Alice"})";
    cpputil::json::JsonParam js1(json1);
    
    // 创建一个无效的 JsonParam
    cpputil::json::JsonParam invalid_js;
    
    // 用无效的 JSON 更新应该失败
    EXPECT_FALSE(js1.update(invalid_js));
    
    // 原始数据应该保持不变
    EXPECT_EQ(js1.get({"name"}, std::string("")), "Alice");
}

TEST(JsonParamTest, UpdateToInvalidJson) {
    std::string json2 = R"({"city": "New York"})";
    cpputil::json::JsonParam js2(json2);
    
    // 创建一个无效的 JsonParam
    cpputil::json::JsonParam invalid_js;
    
    // 无效的 JSON 更新有效的 JSON 应该成功（相当于复制）
    EXPECT_TRUE(invalid_js.update(js2));
    
    // 验证数据被正确复制
    EXPECT_EQ(invalid_js.get({"city"}, std::string("")), "New York");
}

TEST(JsonParamTest, UpdateComplexStructure) {
    std::string json1 = R"({
        "users": [
            {"id": 1, "name": "Alice"},
            {"id": 2, "name": "Bob"}
        ],
        "settings": {
            "theme": "light",
            "features": {
                "notifications": true
            }
        }
    })";
    
    std::string json2 = R"({
        "users": [
            {"id": 3, "name": "Charlie"}
        ],
        "settings": {
            "language": "en",
            "features": {
                "darkMode": true
            }
        },
        "metadata": {
            "version": "1.0"
        }
    })";
    
    cpputil::json::JsonParam js1(json1);
    cpputil::json::JsonParam js2(json2);
    
    EXPECT_TRUE(js1.update(js2));
    
    // 验证复杂结构合并结果
    auto users = js1.get<std::vector<std::map<std::string, std::string>>>({"users"});
    EXPECT_EQ(users.size(), 3);  // 2 + 1
    
    EXPECT_EQ(js1.get({"settings", "theme"}, std::string("")), "light");      // 保持原值
    EXPECT_EQ(js1.get({"settings", "language"}, std::string("")), "en");      // 新增
    EXPECT_EQ(js1.get({"settings", "features", "notifications"}, false), true);  // 保持原值
    EXPECT_EQ(js1.get({"settings", "features", "darkMode"}, false), true);       // 新增
    EXPECT_EQ(js1.get({"metadata", "version"}, std::string("")), "1.0");     // 新增
} 

// 测试数组合并的详细行为
TEST(JsonParamTest, UpdateArrayAppendBehavior) {
    // 测试同级数组的追加行为
    std::string json1 = R"({
        "numbers": [1, 2, 3],
        "strings": ["a", "b"]
    })";
    
    std::string json2 = R"({
        "numbers": [4, 5],
        "strings": ["c", "d", "e"]
    })";
    
    cpputil::json::JsonParam js1(json1);
    cpputil::json::JsonParam js2(json2);
    
    EXPECT_TRUE(js1.update(js2));
    
    // 验证数组追加行为
    auto numbers = js1.get<std::vector<int>>({"numbers"});
    EXPECT_EQ(numbers.size(), 5);  // 3 + 2
    EXPECT_EQ(numbers[0], 1);
    EXPECT_EQ(numbers[1], 2);
    EXPECT_EQ(numbers[2], 3);
    EXPECT_EQ(numbers[3], 4);  // 追加的元素
    EXPECT_EQ(numbers[4], 5);
    
    auto strings = js1.get<std::vector<std::string>>({"strings"});
    EXPECT_EQ(strings.size(), 5);  // 2 + 3
    EXPECT_EQ(strings[0], "a");
    EXPECT_EQ(strings[1], "b");
    EXPECT_EQ(strings[2], "c");  // 追加的元素
    EXPECT_EQ(strings[3], "d");
    EXPECT_EQ(strings[4], "e");
}

TEST(JsonParamTest, UpdateMixedArrayTypes) {
    // 测试混合类型的数组合并
    std::string json1 = R"({
        "mixed": [1, "hello", true]
    })";
    
    std::string json2 = R"({
        "mixed": [2, "world", false, 3.14]
    })";
    
    cpputil::json::JsonParam js1(json1);
    cpputil::json::JsonParam js2(json2);
    
    EXPECT_TRUE(js1.update(js2));
    
    // 由于是混合类型，我们无法直接用模板get，但可以验证数组大小和toString
    std::string result = js1.toString();
    // 结果应该包含所有元素
    EXPECT_TRUE(result.find("1") != std::string::npos);
    EXPECT_TRUE(result.find("hello") != std::string::npos);
    EXPECT_TRUE(result.find("true") != std::string::npos);
    EXPECT_TRUE(result.find("2") != std::string::npos);
    EXPECT_TRUE(result.find("world") != std::string::npos);
    EXPECT_TRUE(result.find("false") != std::string::npos);
    EXPECT_TRUE(result.find("3.14") != std::string::npos);
}

TEST(JsonParamTest, UpdateNestedArrays) {
    // 测试嵌套对象中的数组合并
    std::string json1 = R"({
        "data": {
            "items": [{"id": 1}, {"id": 2}],
            "tags": ["important", "urgent"]
        }
    })";
    
    std::string json2 = R"({
        "data": {
            "items": [{"id": 3}, {"id": 4}],
            "tags": ["new"],
            "category": "test"
        }
    })";
    
    cpputil::json::JsonParam js1(json1);
    cpputil::json::JsonParam js2(json2);
    
    EXPECT_TRUE(js1.update(js2));
    
    // 验证嵌套数组合并
    auto tags = js1.get<std::vector<std::string>>({"data", "tags"});
    EXPECT_EQ(tags.size(), 3);  // 2 + 1
    EXPECT_EQ(tags[0], "important");
    EXPECT_EQ(tags[1], "urgent");
    EXPECT_EQ(tags[2], "new");
    
    // 验证新增的属性
    EXPECT_EQ(js1.get({"data", "category"}, std::string("")), "test");
}

TEST(JsonParamTest, UpdateArrayVsNonArray) {
    // 测试数组与非数组类型的合并
    std::string json1 = R"({
        "value": [1, 2, 3]
    })";
    
    std::string json2 = R"({
        "value": "string_value"
    })";
    
    cpputil::json::JsonParam js1(json1);
    cpputil::json::JsonParam js2(json2);
    
    EXPECT_TRUE(js1.update(js2));
    
    // 当类型不匹配时，应该覆盖而不是合并
    EXPECT_EQ(js1.get({"value"}, std::string("")), "string_value");
}

TEST(JsonParamTest, UpdateNonArrayVsArray) {
    // 测试非数组与数组类型的合并
    std::string json1 = R"({
        "value": "string_value"
    })";
    
    std::string json2 = R"({
        "value": [1, 2, 3]
    })";
    
    cpputil::json::JsonParam js1(json1);
    cpputil::json::JsonParam js2(json2);
    
    EXPECT_TRUE(js1.update(js2));
    
    // 当类型不匹配时，应该覆盖而不是合并
    auto result = js1.get<std::vector<int>>({"value"});
    EXPECT_EQ(result.size(), 3);
    EXPECT_EQ(result[0], 1);
    EXPECT_EQ(result[1], 2);
    EXPECT_EQ(result[2], 3);
}

TEST(JsonParamTest, UpdateEmptyArrays) {
    // 测试空数组的合并
    std::string json1 = R"({
        "empty1": [],
        "data": [1, 2]
    })";
    
    std::string json2 = R"({
        "empty1": [3, 4],
        "empty2": []
    })";
    
    cpputil::json::JsonParam js1(json1);
    cpputil::json::JsonParam js2(json2);
    
    EXPECT_TRUE(js1.update(js2));
    
    // 空数组 + 非空数组 = 非空数组
    auto empty1 = js1.get<std::vector<int>>({"empty1"});
    EXPECT_EQ(empty1.size(), 2);
    EXPECT_EQ(empty1[0], 3);
    EXPECT_EQ(empty1[1], 4);
    
    // 非空数组保持不变，因为empty2是空的，但仍会被添加
    auto data = js1.get<std::vector<int>>({"data"});
    EXPECT_EQ(data.size(), 2);  // 原始数据保持不变，因为没有合并
    
    // 新增的空数组
    auto empty2 = js1.get<std::vector<int>>({"empty2"});
    EXPECT_EQ(empty2.size(), 0);
} 

// 测试 clone 功能
TEST(JsonParamTest, CloneEntireObject) {
    std::string json = R"({
        "user": {
            "name": "Alice",
            "age": 25,
            "scores": [85, 92, 78]
        },
        "settings": {
            "theme": "dark"
        }
    })";
    
    cpputil::json::JsonParam original(json);
    
    // 克隆整个对象
    auto cloned = original.clone();
    
    EXPECT_TRUE(cloned->isValid());
    EXPECT_EQ(cloned->get({"user", "name"}, std::string("")), "Alice");
    EXPECT_EQ(cloned->get({"user", "age"}, 0), 25);
    EXPECT_EQ(cloned->get({"settings", "theme"}, std::string("")), "dark");
    
    // 验证是独立的对象（修改克隆不影响原对象）
    cloned->set({"user", "name"}, std::string("Bob"));
    EXPECT_EQ(original.get({"user", "name"}, std::string("")), "Alice");
    EXPECT_EQ(cloned->get({"user", "name"}, std::string("")), "Bob");
}

TEST(JsonParamTest, CloneSubObject) {
    std::string json = R"({
        "user": {
            "name": "Alice",
            "age": 25,
            "profile": {
                "email": "alice@example.com",
                "phone": "123-456-7890"
            }
        },
        "settings": {
            "theme": "dark"
        }
    })";
    
    cpputil::json::JsonParam original(json);
    
    // 克隆用户信息子对象
    auto userClone = original.clone({"user"});
    
    EXPECT_TRUE(userClone->isValid());
    EXPECT_EQ(userClone->get({"name"}, std::string("")), "Alice");
    EXPECT_EQ(userClone->get({"age"}, 0), 25);
    EXPECT_EQ(userClone->get({"profile", "email"}, std::string("")), "alice@example.com");
    
    // 验证克隆的对象不包含其他数据
    EXPECT_EQ(userClone->get({"settings", "theme"}, std::string("default")), "default");
}

TEST(JsonParamTest, CloneNestedObject) {
    std::string json = R"({
        "user": {
            "profile": {
                "email": "alice@example.com",
                "phone": "123-456-7890",
                "address": {
                    "city": "New York",
                    "zip": "10001"
                }
            }
        }
    })";
    
    cpputil::json::JsonParam original(json);
    
    // 克隆深层嵌套对象
    auto profileClone = original.clone({"user", "profile"});
    
    EXPECT_TRUE(profileClone->isValid());
    EXPECT_EQ(profileClone->get({"email"}, std::string("")), "alice@example.com");
    EXPECT_EQ(profileClone->get({"phone"}, std::string("")), "123-456-7890");
    EXPECT_EQ(profileClone->get({"address", "city"}, std::string("")), "New York");
    EXPECT_EQ(profileClone->get({"address", "zip"}, std::string("")), "10001");
}

TEST(JsonParamTest, CloneArray) {
    std::string json = R"({
        "data": {
            "scores": [85, 92, 78, 96],
            "names": ["Alice", "Bob", "Charlie"]
        }
    })";
    
    cpputil::json::JsonParam original(json);
    
    // 尝试克隆数组 - 现在应该失败，因为数组不是对象类型
    auto scoresClone = original.clone({"data", "scores"});
    EXPECT_FALSE(scoresClone->isValid());
    
    auto namesClone = original.clone({"data", "names"});
    EXPECT_FALSE(namesClone->isValid());
    
    // 但可以克隆包含数组的对象
    auto dataClone = original.clone({"data"});
    EXPECT_TRUE(dataClone->isValid());
    auto scores = dataClone->get<std::vector<int>>({"scores"});
    EXPECT_EQ(scores.size(), 4);
    EXPECT_EQ(scores[0], 85);
}

TEST(JsonParamTest, CloneArrayElement) {
    std::string json = R"({
        "users": [
            {"name": "Alice", "age": 25},
            {"name": "Bob", "age": 30},
            {"name": "Charlie", "age": 35}
        ]
    })";
    
    cpputil::json::JsonParam original(json);
    
    // 克隆数组中的单个元素
    auto userClone = original.clone({"users", static_cast<size_t>(1)});  // 克隆Bob的信息
    
    EXPECT_TRUE(userClone->isValid());
    EXPECT_EQ(userClone->get({"name"}, std::string("")), "Bob");
    EXPECT_EQ(userClone->get({"age"}, 0), 30);
}

TEST(JsonParamTest, CloneNonexistentPath) {
    std::string json = R"({
        "user": {
            "name": "Alice"
        }
    })";
    
    cpputil::json::JsonParam original(json);
    
    // 克隆不存在的路径
    auto nonexistentClone = original.clone({"user", "profile", "email"});
    
    // 应该返回无效的JsonParam
    EXPECT_FALSE(nonexistentClone->isValid());
}

TEST(JsonParamTest, CloneFromInvalidJson) {
    cpputil::json::JsonParam invalid;
    
    // 从无效JSON克隆
    auto cloned = invalid.clone();
    EXPECT_FALSE(cloned->isValid());
    
    auto clonedWithPath = invalid.clone({"user", "name"});
    EXPECT_FALSE(clonedWithPath->isValid());
}

TEST(JsonParamTest, CloneWithListInitialization) {
    std::string json = R"({
        "data": {
            "config": {
                "settings": {
                    "value": 42
                }
            }
        }
    })";
    
    cpputil::json::JsonParam original(json);
    
    // 使用列表初始化语法克隆
    auto configClone = original.clone({"data", "config"});
    
    EXPECT_TRUE(configClone->isValid());
    EXPECT_EQ(configClone->get({"settings", "value"}, 0), 42);
}

// 测试 clone 方法只能克隆对象类型的限制
TEST(JsonParamTest, CloneObjectTypeOnly) {
    std::string json = R"({
        "stringValue": "hello",
        "numberValue": 42,
        "boolValue": true,
        "arrayValue": [1, 2, 3],
        "objectValue": {
            "name": "Alice",
            "age": 25
        }
    })";
    
    cpputil::json::JsonParam original(json);
    
    // 只有对象类型可以被克隆
    auto objectClone = original.clone({"objectValue"});
    EXPECT_TRUE(objectClone->isValid());
    EXPECT_EQ(objectClone->get({"name"}, std::string("")), "Alice");
    EXPECT_EQ(objectClone->get({"age"}, 0), 25);
    
    // 字符串值不能被克隆
    auto stringClone = original.clone({"stringValue"});
    EXPECT_FALSE(stringClone->isValid());
    
    // 数字值不能被克隆
    auto numberClone = original.clone({"numberValue"});
    EXPECT_FALSE(numberClone->isValid());
    
    // 布尔值不能被克隆
    auto boolClone = original.clone({"boolValue"});
    EXPECT_FALSE(boolClone->isValid());
    
    // 数组值不能被克隆
    auto arrayClone = original.clone({"arrayValue"});
    EXPECT_FALSE(arrayClone->isValid());
}

TEST(JsonParamTest, CloneNestedObjectsOnly) {
    std::string json = R"({
        "data": {
            "user": {
                "name": "Alice",
                "scores": [85, 92, 78]
            },
            "config": {
                "settings": {
                    "theme": "dark",
                    "language": "en"
                }
            },
            "simpleValue": "test"
        }
    })";
    
    cpputil::json::JsonParam original(json);
    
    // 可以克隆嵌套对象
    auto userClone = original.clone({"data", "user"});
    EXPECT_TRUE(userClone->isValid());
    EXPECT_EQ(userClone->get({"name"}, std::string("")), "Alice");
    
    auto settingsClone = original.clone({"data", "config", "settings"});
    EXPECT_TRUE(settingsClone->isValid());
    EXPECT_EQ(settingsClone->get({"theme"}, std::string("")), "dark");
    EXPECT_EQ(settingsClone->get({"language"}, std::string("")), "en");
    
    // 不能克隆对象内的数组
    auto scoresClone = original.clone({"data", "user", "scores"});
    EXPECT_FALSE(scoresClone->isValid());
    
    // 不能克隆对象内的字符串值
    auto nameClone = original.clone({"data", "user", "name"});
    EXPECT_FALSE(nameClone->isValid());
    
    // 不能克隆简单值
    auto simpleClone = original.clone({"data", "simpleValue"});
    EXPECT_FALSE(simpleClone->isValid());
}

TEST(JsonParamTest, CloneArrayElementObject) {
    std::string json = R"({
        "users": [
            {"name": "Alice", "age": 25},
            {"name": "Bob", "age": 30},
            "invalidUser"
        ],
        "mixed": [
            1,
            "string",
            {"valid": true}
        ]
    })";
    
    cpputil::json::JsonParam original(json);
    
    // 可以克隆数组中的对象元素
    auto user0Clone = original.clone({"users", static_cast<size_t>(0)});
    EXPECT_TRUE(user0Clone->isValid());
    EXPECT_EQ(user0Clone->get({"name"}, std::string("")), "Alice");
    EXPECT_EQ(user0Clone->get({"age"}, 0), 25);
    
    auto user1Clone = original.clone({"users", static_cast<size_t>(1)});
    EXPECT_TRUE(user1Clone->isValid());
    EXPECT_EQ(user1Clone->get({"name"}, std::string("")), "Bob");
    EXPECT_EQ(user1Clone->get({"age"}, 0), 30);
    
    // 不能克隆数组中的非对象元素
    auto invalidUserClone = original.clone({"users", static_cast<size_t>(2)});
    EXPECT_FALSE(invalidUserClone->isValid());
    
    // 不能克隆混合数组中的基本类型
    auto numberClone = original.clone({"mixed", static_cast<size_t>(0)});
    EXPECT_FALSE(numberClone->isValid());
    
    auto stringClone = original.clone({"mixed", static_cast<size_t>(1)});
    EXPECT_FALSE(stringClone->isValid());
    
    // 可以克隆混合数组中的对象
    auto validObjectClone = original.clone({"mixed", static_cast<size_t>(2)});
    EXPECT_TRUE(validObjectClone->isValid());
    EXPECT_EQ(validObjectClone->get({"valid"}, false), true);
}

TEST(JsonParamTest, CloneEmptyObject) {
    std::string json = R"({
        "emptyObject": {},
        "nonEmptyObject": {
            "key": "value"
        }
    })";
    
    cpputil::json::JsonParam original(json);
    
    // 可以克隆空对象
    auto emptyClone = original.clone({"emptyObject"});
    EXPECT_TRUE(emptyClone->isValid());
    EXPECT_EQ(emptyClone->toString(), "{}");
    
    // 可以克隆非空对象
    auto nonEmptyClone = original.clone({"nonEmptyObject"});
    EXPECT_TRUE(nonEmptyClone->isValid());
    EXPECT_EQ(nonEmptyClone->get({"key"}, std::string("")), "value");
} 