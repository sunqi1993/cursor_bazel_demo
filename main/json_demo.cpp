#include <iostream>
#include <string>
#include <vector>
#include <map>
#include "lib/json.h"

using namespace cpputil::json;

int main() {
    std::cout << "=== cpputil::json 演示程序 ===\n";
    
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
    
    JsonParam js(json_str);

    auto a = js.get({"user", "age"}, 0);
    auto b =js.get<std::vector<int>>({"user","scores"});
    auto c =js.get<std::map<std::string,std::string>>({"user","profile"});
    return 0;
} 