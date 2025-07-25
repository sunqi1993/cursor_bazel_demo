#pragma once

#include <string>
#include <vector>
#include <initializer_list>
#include <memory>
#include <map>
#include <unordered_map>
#include <vector>
#include <string>
#include <type_traits>
#include <rapidjson/document.h>

namespace cpputil {
namespace json {

// JSON 路径类，支持列表初始化
class JsonPath {
public:
    using PathElement = std::variant<std::string, size_t>;
    
    // 默认构造函数
    JsonPath() = default;
    
    // 列表初始化构造函数
    JsonPath(std::initializer_list<PathElement> elements) 
        : path_(elements) {}
    
    // 添加路径元素
    void add(const std::string& key) { path_.emplace_back(key); }
    void add(size_t index) { path_.emplace_back(index); }
    
    // 获取路径元素
    const std::vector<PathElement>& elements() const { return path_; }
    
    // 清空路径
    void clear() { path_.clear(); }
    
    // 路径是否为空
    bool empty() const { return path_.empty(); }
    
    // 路径大小
    size_t size() const { return path_.size(); }

private:
    std::vector<PathElement> path_;
};

// JSON 类，基于 RapidJSON 封装
class JsonParam {
public:
    // 构造函数，接受 JSON 字符串
    explicit JsonParam(const std::string& json_str);
    
    // 默认构造函数
    JsonParam() = default;
    
    // 移动构造函数
    JsonParam(JsonParam&& other) noexcept = default;
    
    // 移动赋值运算符
    JsonParam& operator=(JsonParam&& other) noexcept = default;
    
    // 禁用拷贝（避免性能问题）
    JsonParam(const JsonParam&) = delete;
    JsonParam& operator=(const JsonParam&) = delete;
    
    // 析构函数
    ~JsonParam() = default;
    
    // 获取值的模板方法 - 支持递归类型解析
    template<typename T>
    T get(const JsonPath& path, const T& default_value = T{}) const {
        const rapidjson::Value* value = getValueByPath(path);
        if (!value) {
            return default_value;
        }
        return parseValue(value, default_value);
    }
    
    // 简化接口：直接接受列表初始化
    template<typename T>
    T get(std::initializer_list<JsonPath::PathElement> path_elements, const T& default_value = T{}) const {
        JsonPath path(path_elements);
        return get(path, default_value);
    }
    
    // 设置值的模板方法 - 支持递归类型设置
    template<typename T>
    bool set(const JsonPath& path, const T& value);
    
    // 简化接口：直接接受列表初始化
    template<typename T>
    bool set(std::initializer_list<JsonPath::PathElement> path_elements, const T& value) {
        JsonPath path(path_elements);
        return set(path, value);
    }
    
    // 检查路径是否存在
    bool has(const JsonPath& path) const;
    
    // 转换为字符串
    std::string toString() const;
    
    // 检查 JSON 是否有效
    bool isValid() const;

private:
    // 类型特征检测
    template<typename T>
    struct is_vector : std::false_type {};
    
    template<typename V>
    struct is_vector<std::vector<V>> : std::true_type {};
    
    template<typename T>
    struct is_map : std::false_type {};
    
    template<typename K, typename V>
    struct is_map<std::map<K, V>> : std::true_type {};
    
    template<typename K, typename V>
    struct is_map<std::unordered_map<K, V>> : std::true_type {};
    
    // 递归解析辅助函数
    template<typename T>
    T parseValue(const rapidjson::Value* value, const T& default_value) const;
    
    template<typename V>
    std::vector<V> parseVector(const rapidjson::Value* value, const std::vector<V>& default_value) const;
    
    template<typename MapType>
    MapType parseMap(const rapidjson::Value* value, const MapType& default_value) const;
    
    // 递归设置辅助函数
    template<typename T>
    bool setValue(rapidjson::Value* value, const T& new_value);
    
    template<typename V>
    bool setVector(rapidjson::Value* value, const std::vector<V>& new_value);
    
    template<typename MapType>
    bool setMap(rapidjson::Value* value, const MapType& new_value);

private:
    std::unique_ptr<rapidjson::Document> doc_;
    
    // 根据路径获取 RapidJSON 值
    const rapidjson::Value* getValueByPath(const JsonPath& path) const;
    
    // 根据路径获取可修改的 RapidJSON 值，如果路径不存在则创建
    rapidjson::Value* getOrCreateValueByPath(const JsonPath& path);
    
    // 从 RapidJSON 值转换为目标类型
    template<typename T>
    T convertValue(const rapidjson::Value* value, const T& default_value) const;
};

// 移除 make_path 函数，使用简化的 get 接口

} // namespace json
} // namespace cpputil 