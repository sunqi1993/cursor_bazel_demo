# clangd 编译数据库配置详解

## 🔍 `CompilationDatabase` 配置说明

### 基本概念

`CompilationDatabase` 用于指定 clangd 查找 `compile_commands.json` 文件的位置。

### 配置方式对比

#### 1. 通过 `.clangd` 文件配置

```yaml
# .clangd
CompileFlags:
  CompilationDatabase: .    # 在当前目录查找
  # 或者
  CompilationDatabase: build/  # 在 build/ 目录查找
```

**路径说明**：
- `.` = 项目根目录
- `build/` = 项目根目录下的 build 子目录
- `/absolute/path/` = 绝对路径
- `../parent/` = 相对路径

#### 2. 通过 VS Code 启动参数配置

```json
// .vscode/settings.json
{
    "clangd.arguments": [
        "--compile-commands-dir=${workspaceFolder}",
        // 等价于 CompilationDatabase: .
    ]
}
```

#### 3. 通过环境变量配置

```bash
export CLANGD_COMPILE_COMMANDS_DIR=/path/to/compile/commands
```

### ⚖️ 优先级顺序

clangd 按以下优先级查找编译数据库：

1. **命令行参数** (`--compile-commands-dir`)
2. **`.clangd` 配置文件** (`CompilationDatabase`)
3. **环境变量** (`CLANGD_COMPILE_COMMANDS_DIR`)
4. **自动搜索** (从当前文件开始向上查找)

### 🎯 我们的项目配置

#### 当前配置策略

```json
// .vscode/settings.json - 最高优先级
"clangd.arguments": [
    "--compile-commands-dir=${workspaceFolder}"
]
```

```yaml
# .clangd - 已注释掉，避免重复
# CompilationDatabase: .  # 已通过 VS Code settings.json 指定
```

#### 为什么这样配置？

1. **避免重复**：VS Code 参数已经指定了位置
2. **明确性**：启动参数优先级最高，更明确
3. **灵活性**：VS Code 可以动态替换 `${workspaceFolder}`

### 🔧 不同场景的配置建议

#### 场景 1：只使用 VS Code

**推荐配置**：
```json
// .vscode/settings.json
"clangd.arguments": ["--compile-commands-dir=${workspaceFolder}"]
```

```yaml
# .clangd - 不需要 CompilationDatabase
CompileFlags:
  Add: [-std=c++17]
```

#### 场景 2：多 IDE 支持（VS Code, Vim, Emacs）

**推荐配置**：
```yaml
# .clangd - 通用配置
CompileFlags:
  CompilationDatabase: .
  Add: [-std=c++17]
```

```json
// .vscode/settings.json - 移除 compile-commands-dir
"clangd.arguments": ["--background-index", "--clang-tidy"]
```

#### 场景 3：编译数据库在其他目录

**如果 `compile_commands.json` 在 `build/` 目录**：

```yaml
# .clangd
CompileFlags:
  CompilationDatabase: build/
```

或者

```json
// .vscode/settings.json
"clangd.arguments": ["--compile-commands-dir=${workspaceFolder}/build"]
```

### 🐛 常见问题

#### 问题 1：clangd 找不到编译数据库

**症状**：没有代码补全，错误提示不准确

**解决方案**：
1. 确认 `compile_commands.json` 存在：
   ```bash
   ls -la compile_commands.json
   ```

2. 检查 clangd 日志：
   ```
   VS Code: Ctrl+Shift+P → "clangd: Open log"
   ```

3. 验证配置：
   ```bash
   clangd --check=lib/hello.cpp
   ```

#### 问题 2：配置冲突

**症状**：clangd 行为不一致

**解决方案**：
- 确保只在一个地方配置 CompilationDatabase
- 优先使用 VS Code 启动参数（最高优先级）

#### 问题 3：相对路径问题

**症状**：某些文件找不到头文件

**解决方案**：
- 使用绝对路径或确保工作目录正确
- 在 VS Code 中，`${workspaceFolder}` 会自动展开为项目根目录

### 📝 最佳实践

1. **单一真相源**：只在一个地方配置编译数据库路径
2. **VS Code 优先**：如果主要使用 VS Code，在 settings.json 中配置
3. **通用性考虑**：如果需要支持多个编辑器，在 .clangd 中配置
4. **路径明确**：避免使用复杂的相对路径
5. **验证配置**：使用验证脚本检查配置是否正确

### 🔍 调试技巧

```bash
# 检查 clangd 如何解析配置
clangd --check=lib/hello.cpp 2>&1 | grep -i "compilation database"

# 查看 clangd 日志中的编译数据库加载信息
# VS Code: Ctrl+Shift+P → "clangd: Open log"
# 搜索 "compile_commands" 或 "compilation database"
``` 