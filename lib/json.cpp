#include "json.h"
#include <rapidjson/document.h>
#include <rapidjson/error/en.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include <iostream>
#include <variant>
#include <vector>
#include <string>
#include <map>
#include <unordered_map>

namespace cpputil {
namespace json {

JsonParam::JsonParam(const std::string& json_str) : doc_(std::make_unique<rapidjson::Document>()) {
    if (doc_->Parse(json_str.c_str()).HasParseError()) {
        std::cerr << "JSON parse error: " << rapidjson::GetParseError_En(doc_->GetParseError()) 
                  << " at offset " << doc_->GetErrorOffset() << std::endl;
        doc_.reset();
    }
}

// 拷贝构造函数
JsonParam::JsonParam(const JsonParam& other) {
    if (other.isValid()) {
        doc_ = std::make_unique<rapidjson::Document>();
        doc_->CopyFrom(*other.doc_, doc_->GetAllocator());
    }
}

// 拷贝赋值运算符
JsonParam& JsonParam::operator=(const JsonParam& other) {
    if (this != &other) {
        if (other.isValid()) {
            if (!doc_) {
                doc_ = std::make_unique<rapidjson::Document>();
            }
            doc_->CopyFrom(*other.doc_, doc_->GetAllocator());
        } else {
            doc_.reset();
        }
    }
    return *this;
}

bool JsonParam::has(const JsonPath& path) const {
    return getValueByPath(path) != nullptr;
}

std::string JsonParam::toString() const {
    if (!isValid()) {
        return "";
    }
    
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    doc_->Accept(writer);
    return buffer.GetString();
}

bool JsonParam::isValid() const {
    return doc_ != nullptr;
}

// 更新当前 JSON 对象
bool JsonParam::update(const JsonParam& other) {
    if (!other.isValid()) {
        return false;
    }
    
    // 如果当前对象无效，直接复制
    if (!isValid()) {
        *this = other;
        return true;
    }
    
    // 深度合并两个 JSON 对象
    deepMerge(*doc_, *other.doc_, doc_->GetAllocator());
    return true;
}

// 深度合并两个 JSON 值
void JsonParam::deepMerge(rapidjson::Value& target, const rapidjson::Value& source, rapidjson::Document::AllocatorType& allocator) {
    if (source.IsObject() && target.IsObject()) {
        // 合并对象
        for (auto it = source.MemberBegin(); it != source.MemberEnd(); ++it) {
            const char* key = it->name.GetString();
            
            if (target.HasMember(key)) {
                // 如果目标也有这个键，递归合并
                deepMerge(target[key], it->value, allocator);
            } else {
                // 如果目标没有这个键，添加新成员
                rapidjson::Value key_copy(key, allocator);
                rapidjson::Value value_copy = deepCopy(it->value, allocator);
                target.AddMember(key_copy, value_copy, allocator);
            }
        }
    } else if (source.IsArray() && target.IsArray()) {
        // 对于数组，将源数组的元素追加到目标数组
        for (rapidjson::SizeType i = 0; i < source.Size(); ++i) {
            rapidjson::Value element_copy = deepCopy(source[i], allocator);
            target.PushBack(element_copy, allocator);
        }
    } else {
        // 对于其他类型，直接覆盖
        target.CopyFrom(source, allocator);
    }
}

// 深度复制 JSON 值
rapidjson::Value JsonParam::deepCopy(const rapidjson::Value& source, rapidjson::Document::AllocatorType& allocator) {
    rapidjson::Value result;
    result.CopyFrom(source, allocator);
    return result;
}

// 克隆当前 JSON 对象
JsonParamPtr JsonParam::clone() const {
    if (!isValid()) {
        return std::make_shared<JsonParam>();
    }
    
    // 创建新的 JsonParam 对象
    auto result = std::make_shared<JsonParam>();
    result->doc_ = std::make_unique<rapidjson::Document>();
    result->doc_->CopyFrom(*doc_, result->doc_->GetAllocator());
    
    return result;
}

// 克隆指定路径的 JSON 子树
JsonParamPtr JsonParam::clone(const JsonPath& path) const {
    if (!isValid()) {
        return std::make_shared<JsonParam>();
    }
    
    // 获取指定路径的值
    const rapidjson::Value* value = getValueByPath(path);
    if (!value||!value->IsObject()) {
        return std::make_shared<JsonParam>();
    }
    
    // 创建新的 JsonParam 对象，并复制指定路径的值
    auto result = std::make_shared<JsonParam>();
    result->doc_ = std::make_unique<rapidjson::Document>();
    result->doc_->CopyFrom(*value, result->doc_->GetAllocator());
    
    return result;
}

// 简化接口：直接接受列表初始化的路径
JsonParamPtr JsonParam::clone(std::initializer_list<JsonPath::PathElement> path_elements) const {
    JsonPath path(path_elements);
    return clone(path);
}

const rapidjson::Value* JsonParam::getValueByPath(const JsonPath& path) const {
    if (!isValid() || path.empty()) {
        return nullptr;
    }
    
    const rapidjson::Value* current = doc_.get();
    
    for (const auto& element : path.elements()) {
        if (!current) {
            return nullptr;
        }
        
        if (std::holds_alternative<std::string>(element)) {
            const std::string& key = std::get<std::string>(element);
            if (!current->IsObject() || !current->HasMember(key.c_str())) {
                return nullptr;
            }
            current = &(*current)[key.c_str()];
        } else if (std::holds_alternative<size_t>(element)) {
            size_t index = std::get<size_t>(element);
            if (!current->IsArray() || index >= current->Size()) {
                return nullptr;
            }
            current = &(*current)[index];
        }
    }
    
    return current;
}

rapidjson::Value* JsonParam::getOrCreateValueByPath(const JsonPath& path) {
    if (!isValid() || path.empty()) {
        return nullptr;
    }
    
    rapidjson::Value* current = doc_.get();
    
    for (size_t i = 0; i < path.elements().size(); ++i) {
        const auto& element = path.elements()[i];
        
        if (!current) {
            return nullptr;
        }
        
        if (std::holds_alternative<std::string>(element)) {
            const std::string& key = std::get<std::string>(element);
            
            // 确保当前值是对象
            if (!current->IsObject()) {
                current->SetObject();
            }
            
            // 检查成员是否存在，如果不存在则添加
            if (!current->HasMember(key.c_str())) {
                current->AddMember(
                    rapidjson::Value(key.c_str(), doc_->GetAllocator()),
                    rapidjson::Value(),
                    doc_->GetAllocator()
                );
            }
            
            // 安全地访问成员
            auto member = current->FindMember(key.c_str());
            current = &member->value;
        } else if (std::holds_alternative<size_t>(element)) {
            size_t index = std::get<size_t>(element);
            
            // 确保当前值是数组
            if (!current->IsArray()) {
                current->SetArray();
            }
            
            // 确保数组大小足够
            while (current->Size() <= index) {
                current->PushBack(rapidjson::Value(), doc_->GetAllocator());
            }
            
            current = &(*current)[index];
        }
    }
    
    return current;
}

// 通用的递归类型解析函数
template<typename T>
T JsonParam::parseValue(const rapidjson::Value* value, const T& default_value) const {
    if (!value) return default_value;
    
    // 基本类型处理
    if constexpr (std::is_same_v<T, std::string>) {
        if (value->IsString()) {
            return value->GetString();
        }
    } else if constexpr (std::is_same_v<T, int>) {
        if (value->IsInt()) {
            return value->GetInt();
        } else if (value->IsInt64()) {
            return static_cast<int>(value->GetInt64());
        }
    } else if constexpr (std::is_same_v<T, double>) {
        if (value->IsDouble()) {
            return value->GetDouble();
        } else if (value->IsInt()) {
            return static_cast<double>(value->GetInt());
        }
    } else if constexpr (std::is_same_v<T, bool>) {
        if (value->IsBool()) {
            return value->GetBool();
        }
    } else if constexpr (is_vector<T>::value) {
        // vector 类型处理
        return parseVector(value, default_value);
    } else if constexpr (is_map<T>::value) {
        // map 类型处理
        return parseMap(value, default_value);
    }
    
    return default_value;
}

// vector 类型解析
template<typename V>
std::vector<V> JsonParam::parseVector(const rapidjson::Value* value, const std::vector<V>& default_value) const {
    if (!value || !value->IsArray()) {
        return default_value;
    }
    
    std::vector<V> result;
    result.reserve(value->Size());
    
    for (rapidjson::SizeType i = 0; i < value->Size(); ++i) {
        V element = parseValue(&(*value)[i], V{});
        result.push_back(element);
    }
    
    return result;
}

// map 类型解析
template<typename MapType>
MapType JsonParam::parseMap(const rapidjson::Value* value, const MapType& default_value) const {
    if (!value || !value->IsObject()) {
        return default_value;
    }
    
    MapType result;
    using ValueType = typename MapType::mapped_type;
    
    for (auto it = value->MemberBegin(); it != value->MemberEnd(); ++it) {
        std::string key = it->name.GetString();
        ValueType converted_value = parseValue(&it->value, ValueType{});
        result[key] = converted_value;
    }
    
    return result;
}

// 通用的递归类型设置函数
template<typename T>
bool JsonParam::set(const JsonPath& path, const T& value) {
    rapidjson::Value* target = getOrCreateValueByPath(path);
    if (!target) {
        return false;
    }
    return setValue(target, value);
}

template<typename T>
bool JsonParam::setValue(rapidjson::Value* value, const T& new_value) {
    if (!value) return false;
    
    // 基本类型处理
    if constexpr (std::is_same_v<T, std::string>) {
        value->SetString(new_value.c_str(), doc_->GetAllocator());
    } else if constexpr (std::is_same_v<T, int>) {
        value->SetInt(new_value);
    } else if constexpr (std::is_same_v<T, double>) {
        value->SetDouble(new_value);
    } else if constexpr (std::is_same_v<T, bool>) {
        value->SetBool(new_value);
    } else if constexpr (is_vector<T>::value) {
        // vector 类型处理
        return setVector(value, new_value);
    } else if constexpr (is_map<T>::value) {
        // map 类型处理
        return setMap(value, new_value);
    } else {
        return false;
    }
    
    return true;
}

// vector 类型设置
template<typename V>
bool JsonParam::setVector(rapidjson::Value* value, const std::vector<V>& new_value) {
    if (!value) return false;
    
    value->SetArray();
    value->Clear();
    
    for (const auto& item : new_value) {
        rapidjson::Value item_value;
        if (setValue(&item_value, item)) {
            value->PushBack(item_value, doc_->GetAllocator());
        }
    }
    
    return true;
}

// map 类型设置
template<typename MapType>
bool JsonParam::setMap(rapidjson::Value* value, const MapType& new_value) {
    if (!value) return false;
    
    value->SetObject();
    value->RemoveAllMembers();
    
    for (const auto& pair : new_value) {
        rapidjson::Value key_value(pair.first.c_str(), doc_->GetAllocator());
        rapidjson::Value value_value;
        if (setValue(&value_value, pair.second)) {
            value->AddMember(key_value, value_value, doc_->GetAllocator());
        }
    }
    
    return true;
}

// 显式实例化常用类型
template std::string JsonParam::get(const JsonPath& path, const std::string& default_value) const;
template int JsonParam::get(const JsonPath& path, const int& default_value) const;
template double JsonParam::get(const JsonPath& path, const double& default_value) const;
template bool JsonParam::get(const JsonPath& path, const bool& default_value) const;

// 显式实例化 vector 类型
template std::vector<std::string> JsonParam::get(const JsonPath& path, const std::vector<std::string>& default_value) const;
template std::vector<int> JsonParam::get(const JsonPath& path, const std::vector<int>& default_value) const;
template std::vector<double> JsonParam::get(const JsonPath& path, const std::vector<double>& default_value) const;
template std::vector<bool> JsonParam::get(const JsonPath& path, const std::vector<bool>& default_value) const;

// 显式实例化 map 类型
template std::map<std::string, std::string> JsonParam::get(const JsonPath& path, const std::map<std::string, std::string>& default_value) const;
template std::unordered_map<std::string, std::string> JsonParam::get(const JsonPath& path, const std::unordered_map<std::string, std::string>& default_value) const;
template std::map<std::string, int> JsonParam::get(const JsonPath& path, const std::map<std::string, int>& default_value) const;
template std::unordered_map<std::string, int> JsonParam::get(const JsonPath& path, const std::unordered_map<std::string, int>& default_value) const;
template std::map<std::string, double> JsonParam::get(const JsonPath& path, const std::map<std::string, double>& default_value) const;
template std::unordered_map<std::string, double> JsonParam::get(const JsonPath& path, const std::unordered_map<std::string, double>& default_value) const;

// 递归 map 类型实例化
template std::map<std::string, std::map<std::string, std::string>> JsonParam::get(const JsonPath& path, const std::map<std::string, std::map<std::string, std::string>>& default_value) const;
template std::map<std::string, std::vector<std::string>> JsonParam::get(const JsonPath& path, const std::map<std::string, std::vector<std::string>>& default_value) const;
template std::vector<std::map<std::string, std::string>> JsonParam::get(const JsonPath& path, const std::vector<std::map<std::string, std::string>>& default_value) const;

// 新增测试用例需要的实例化
template std::map<std::string, std::vector<int>> JsonParam::get(const JsonPath& path, const std::map<std::string, std::vector<int>>& default_value) const;
template std::vector<std::map<std::string, int>> JsonParam::get(const JsonPath& path, const std::vector<std::map<std::string, int>>& default_value) const;

// set 方法的显式实例化
template bool JsonParam::set(const JsonPath& path, const std::string& value);
template bool JsonParam::set(const JsonPath& path, const int& value);
template bool JsonParam::set(const JsonPath& path, const double& value);
template bool JsonParam::set(const JsonPath& path, const bool& value);

// set vector 类型实例化
template bool JsonParam::set(const JsonPath& path, const std::vector<std::string>& value);
template bool JsonParam::set(const JsonPath& path, const std::vector<int>& value);
template bool JsonParam::set(const JsonPath& path, const std::vector<double>& value);
template bool JsonParam::set(const JsonPath& path, const std::vector<bool>& value);

// set map 类型实例化
template bool JsonParam::set(const JsonPath& path, const std::map<std::string, std::string>& value);
template bool JsonParam::set(const JsonPath& path, const std::unordered_map<std::string, std::string>& value);
template bool JsonParam::set(const JsonPath& path, const std::map<std::string, int>& value);
template bool JsonParam::set(const JsonPath& path, const std::unordered_map<std::string, int>& value);
template bool JsonParam::set(const JsonPath& path, const std::map<std::string, double>& value);
template bool JsonParam::set(const JsonPath& path, const std::unordered_map<std::string, double>& value);

// 递归 set 类型实例化
template bool JsonParam::set(const JsonPath& path, const std::map<std::string, std::vector<int>>& value);
template bool JsonParam::set(const JsonPath& path, const std::vector<std::map<std::string, int>>& value);

} // namespace json
} // namespace cpputil 