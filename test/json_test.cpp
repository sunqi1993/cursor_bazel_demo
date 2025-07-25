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