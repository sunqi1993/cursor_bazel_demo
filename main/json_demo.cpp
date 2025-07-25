#include <iostream>
#include <string>
#include <vector>
#include <map>
#include "lib/json.h"

using namespace cpputil::json;

int main() {
    std::cout << "=== cpputil::json 演示程序 ===" << std::endl;
    
    // 创建初始 JSON
    std::string json_str = R"({
        "user": {
            "name": "Alice",
            "age": 25,
            "scores": [85, 92, 78],
            "profile": {
                "email": "alice@example.com"
            }
        }
    })";
    
    json j(json_str);
    
    std::cout << "\n1. 初始 JSON:" << std::endl;
    std::cout << j.toString() << std::endl;
    
    // 演示 get 功能
    std::cout << "\n2. Get 操作演示:" << std::endl;
    std::string name = j.get({"user", "name"}, std::string(""));
    int age = j.get({"user", "age"}, 0);
    std::vector<int> scores = j.get({"user", "scores"}, std::vector<int>{});
    
    std::cout << "   name: " << name << std::endl;
    std::cout << "   age: " << age << std::endl;
    std::cout << "   scores: ";
    for (int score : scores) {
        std::cout << score << " ";
    }
    std::cout << std::endl;
    
    // 演示 set 功能
    std::cout << "\n3. Set 操作演示:" << std::endl;
    
    // 设置基本类型
    j.set({"user", "age"}, 26);
    j.set({"user", "height"}, 175.5);
    j.set({"user", "active"}, true);
    
    // 设置数组
    j.set({"user", "scores"}, std::vector<int>{95, 88, 92});
    
    // 设置嵌套对象
    j.set({"user", "profile", "phone"}, std::string("123-456-7890"));
    
    // 设置复杂嵌套结构
    std::vector<std::map<std::string, int>> friends = {
        {{"id", 1}, {"score", 100}},
        {{"id", 2}, {"score", 200}}
    };
    j.set({"user", "friends"}, friends);
    
    // 设置数组元素
    j.set({"user", "tags", size_t(0)}, std::string("dev"));
    j.set({"user", "tags", size_t(1)}, std::string("cpp"));
    j.set({"user", "tags", size_t(2)}, std::string("json"));
    
    std::cout << "   设置完成！" << std::endl;
    
    // 验证设置结果
    std::cout << "\n4. 验证设置结果:" << std::endl;
    std::cout << "   new age: " << j.get({"user", "age"}, 0) << std::endl;
    std::cout << "   height: " << j.get({"user", "height"}, 0.0) << std::endl;
    std::cout << "   active: " << (j.get({"user", "active"}, false) ? "true" : "false") << std::endl;
    
    auto new_scores = j.get({"user", "scores"}, std::vector<int>{});
    std::cout << "   new scores: ";
    for (int score : new_scores) {
        std::cout << score << " ";
    }
    std::cout << std::endl;
    
    std::cout << "   phone: " << j.get({"user", "profile", "phone"}, std::string("")) << std::endl;
    
    auto tags = j.get({"user", "tags"}, std::vector<std::string>{});
    std::cout << "   tags: ";
    for (const auto& tag : tags) {
        std::cout << tag << " ";
    }
    std::cout << std::endl;
    
    auto friends_result = j.get({"user", "friends"}, std::vector<std::map<std::string, int>>{});
    std::cout << "   friends count: " << friends_result.size() << std::endl;
    for (size_t i = 0; i < friends_result.size(); ++i) {
        std::cout << "     friend " << i << ": id=" << friends_result[i]["id"] 
                  << ", score=" << friends_result[i]["score"] << std::endl;
    }
    
    std::cout << "\n5. 最终 JSON:" << std::endl;
    std::cout << j.toString() << std::endl;
    
    return 0;
} 