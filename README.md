# Bazel C++ Demo Project

这是一个使用 Bazel modules (bzlmod) 的 C++ 示例项目。

## 项目结构

```
bazel_demo/
├── MODULE.bazel        # Bazel module 定义
├── .bazelrc           # Bazel 配置文件
├── BUILD.bazel        # 根目录构建文件
├── lib/               # 库代码
│   ├── hello.h        # 头文件
│   ├── hello.cpp      # 实现文件
│   ├── json.h         # JSON 库头文件
│   ├── json.cpp       # JSON 库实现文件
│   └── BUILD.bazel    # 库构建文件
├── main/              # 主程序
│   ├── main.cpp       # 主程序入口
│   └── BUILD.bazel    # 主程序构建文件
├── test/              # 单元测试
│   ├── hello_test.cpp # 测试文件
│   ├── json_test.cpp  # JSON 库测试文件
│   └── BUILD.bazel    # 测试构建文件
└── README.md          # 项目说明
```

## 构建和运行

### 安装 Bazel

首先确保已安装 Bazel 6.0 或更高版本，支持 bzlmod。

### 构建项目

```bash
# 构建整个项目
bazel build //...

# 构建主程序
bazel build //main:main

# 构建库
bazel build //lib:hello_lib
```

### 运行程序

```bash
# 运行主程序
bazel run //main:main

# 或者使用别名
bazel run //:run
```

### 运行测试

```bash
# 运行所有测试
bazel test //...

# 运行特定测试
bazel test //test:hello_test
bazel test //test:json_test
```

### 构建配置

```bash
# 优化构建
bazel build --config=opt //main:main

# 调试构建
bazel build --config=dbg //main:main
```

## 项目特性

- 使用 Bazel modules (bzlmod) 进行依赖管理
- C++17 标准
- 包含库、可执行文件和单元测试
- 使用 Google Test 进行单元测试
- 配置了常用的构建选项
- 支持生成 `compile_commands.json` 用于 IDE 语义分析
- 基于 RapidJSON 的 JSON 库封装，支持路径访问和类型安全
- 支持递归类型解析：`int`、`double`、`string`、`vector<V>`、`map<K,V>`
- 支持嵌套类型：`map<string, vector<int>>`、`vector<map<string, string>>` 等
- 支持 `set<T>` 接口：可设置任意类型值，自动创建路径
- 简化的 API：`json_obj.get({"node1", 1, "node2"}, default_value)`、`json_obj.set({"node1", 1, "node2"}, value)`
- 命名空间：`cpputil::json::JsonParam`

## IDE 支持和语义分析

### 生成 compile_commands.json

这个项目配置了 `hedron_compile_commands` 扩展，可以为 clangd 等语言服务器生成 `compile_commands.json` 文件：

```bash
# 生成 compile_commands.json
bazel run //:refresh_compile_commands
```

该命令会：
- 分析项目中的所有 C++ 目标
- 生成包含编译选项的 `compile_commands.json` 文件
- 自动设置 `.git/info/exclude` 忽略生成的文件
- 创建 `external` 符号链接以便查看外部依赖

### IDE 配置

#### VS Code
1. 安装 clangd 扩展
2. 确保禁用 C/C++ IntelliSense（避免冲突）
3. 运行 `bazel run //:refresh_compile_commands` 生成编译数据库
4. clangd 会自动使用 `compile_commands.json` 和 `.clangd` 配置

#### 其他 IDE
大多数现代 IDE 都支持 clangd 或可以直接使用 `compile_commands.json`：
- CLion：可以直接导入 `compile_commands.json`
- Vim/Neovim：通过 LSP 插件使用 clangd
- Emacs：通过 lsp-mode 使用 clangd

### 更新编译数据库

当你添加新文件或修改构建配置时，需要重新生成 `compile_commands.json`：

```bash
bazel run //:refresh_compile_commands
```

### clangd 配置

项目包含 `.clangd` 配置文件，提供了：
- C++17 标准支持
- 严格的诊断检查
- 优化的语义分析设置
- 详细的悬停信息 