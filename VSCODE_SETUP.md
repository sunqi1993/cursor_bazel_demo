# VS Code + clangd 配置指南

本指南将帮助您在 VS Code 中配置 clangd 插件，以获得最佳的 C++ 开发体验。

## 📦 安装必需的扩展

### 1. 安装 clangd 扩展

在 VS Code 中打开扩展面板（`Cmd+Shift+X` / `Ctrl+Shift+X`），搜索并安装：

- **clangd** (llvm-vs-code-extensions.vscode-clangd)
- **Bazel** (bazelbuild.vscode-bazel)

### 2. 禁用冲突扩展

如果您之前安装了传统的 C/C++ 扩展，请禁用或卸载：

- ❌ **C/C++** (ms-vscode.cpptools)
- ❌ **C/C++ Extension Pack** (ms-vscode.cpptools-extension-pack)

这些扩展会与 clangd 产生冲突。

## 🔧 系统依赖

### macOS
```bash
# 使用 Homebrew 安装 clangd
brew install llvm
# 或者直接安装 clang
xcode-select --install
```

### Linux (Ubuntu/Debian)
```bash
# 安装 clangd
sudo apt update
sudo apt install clangd-15 clang-15
# 创建符号链接（如果需要）
sudo ln -sf /usr/bin/clangd-15 /usr/bin/clangd
```

### Windows
```powershell
# 使用 Chocolatey
choco install llvm
# 或者从官网下载 LLVM: https://releases.llvm.org/download.html
```

## 🚀 使用步骤

### 1. 生成编译数据库

在项目根目录运行：

```bash
bazel run //:refresh_compile_commands
```

这将生成 `compile_commands.json` 文件，包含所有 C++ 文件的编译信息。

### 2. 重启 VS Code

为确保所有配置生效，请重启 VS Code：

- `Cmd+Shift+P` / `Ctrl+Shift+P`
- 搜索 "Developer: Reload Window"
- 按回车

### 3. 验证配置

打开任意 C++ 文件（如 `lib/hello.cpp`），您应该看到：

- ✅ 语法高亮
- ✅ 代码补全
- ✅ 错误检查
- ✅ 跳转到定义/声明
- ✅ 符号搜索

## 🎯 快捷键和功能

### 导航
- `F12` - 跳转到定义
- `Shift+F12` - 查找所有引用
- `Ctrl+Shift+O` / `Cmd+Shift+O` - 文件内符号搜索
- `Ctrl+T` / `Cmd+T` - 全局符号搜索

### 代码操作
- `Ctrl+Space` - 触发代码补全
- `Shift+Alt+F` / `Shift+Option+F` - 格式化代码
- `Ctrl+.` / `Cmd+.` - 快速修复

### Bazel 集成
- `Ctrl+Shift+P` -> "Tasks: Run Task" -> 选择 Bazel 任务
- 或者直接使用 `F1` -> "Bazel: Build All"

## 🛠️ 任务和调试

### 可用任务

通过 `Ctrl+Shift+P` -> "Tasks: Run Task" 访问：

1. **Bazel: Build All** - 构建所有目标
2. **Bazel: Run Main** - 运行主程序
3. **Bazel: Run Tests** - 运行所有测试
4. **Refresh Compile Commands** - 刷新编译数据库
5. **Bazel: Clean** - 清理构建产物

### 调试配置

按 `F5` 或进入调试面板，可用配置：

1. **Debug Main Program** - 调试主程序
2. **Debug Tests** - 调试测试

## 🔍 故障排除

### clangd 未工作

1. 检查 clangd 是否已安装：
   ```bash
   clangd --version
   ```

2. 检查 `compile_commands.json` 是否存在：
   ```bash
   ls -la compile_commands.json
   ```

3. 查看 clangd 日志：
   - `Ctrl+Shift+P` -> "clangd: Open log"

### 补全不工作

1. 确保禁用了 C/C++ IntelliSense
2. 重新生成 compile_commands.json：
   ```bash
   bazel run //:refresh_compile_commands
   ```
3. 重启 VS Code

### 错误检查不准确

1. 检查 `.clangd` 配置文件
2. 确保项目构建成功：
   ```bash
   bazel build //...
   ```

## 📝 额外配置

### 自定义 clangd 参数

编辑 `.vscode/settings.json` 中的 `clangd.arguments`：

```json
"clangd.arguments": [
    "--log=verbose",                              // 详细日志输出
    "--pretty",                                   // 美化输出格式
    "--background-index",                         // 后台索引构建
    "--clang-tidy",                              // 启用 clang-tidy 检查
    "--completion-style=detailed",               // 详细的代码补全
    "--header-insertion=iwyu",                   // 智能头文件插入
    "--header-insertion-decorators",             // 头文件插入装饰器
    "--pch-storage=memory",                      // 预编译头存储在内存中
    "--compile-commands-dir=${workspaceFolder}", // 指定 compile_commands.json 位置
    "--enable-config",                           // 启用 .clangd 配置文件
    "--query-driver=**"                          // 查询编译器驱动获取系统头文件
]
```

#### 重要参数说明：

- `--compile-commands-dir`: 明确指定 `compile_commands.json` 的查找目录
- `--enable-config`: 启用项目根目录的 `.clangd` 配置文件支持
- `--query-driver=**`: 允许 clangd 查询编译器来获取系统头文件路径，解决系统库补全问题

### 项目特定设置

当前项目已预配置：

- 🔧 **`.clangd`** - clangd 语言服务器配置
- 🔧 **`.vscode/settings.json`** - VS Code 工作区设置
- 🔧 **`.vscode/c_cpp_properties.json`** - C++ 语言配置
- 🔧 **`.vscode/tasks.json`** - Bazel 任务配置
- 🔧 **`.vscode/launch.json`** - 调试配置

## 🎉 享受开发！

配置完成后，您将拥有：

- 🚀 快速准确的代码补全
- 🔍 智能错误检查和修复建议
- 📍 精确的跳转和搜索功能
- 🎨 语义代码高亮
- 🔧 代码格式化和重构
- �� 集成调试支持

愉快地编码吧！ 🎊 