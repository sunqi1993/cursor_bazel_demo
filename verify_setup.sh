#!/bin/bash

echo "🔍 验证 VS Code + clangd 配置"
echo "================================"

# 检查 clangd 是否安装
echo "1. 检查 clangd 安装..."
if command -v clangd &> /dev/null; then
    echo "✅ clangd 已安装: $(clangd --version | head -1)"
else
    echo "❌ clangd 未安装，请先安装 clangd"
    exit 1
fi

# 检查 compile_commands.json
echo ""
echo "2. 检查 compile_commands.json..."
if [ -f "compile_commands.json" ]; then
    size=$(du -h compile_commands.json | cut -f1)
    echo "✅ compile_commands.json 存在 (大小: $size)"
else
    echo "❌ compile_commands.json 不存在，请运行: bazel run //:refresh_compile_commands"
fi

# 检查 .clangd 配置
echo ""
echo "3. 检查 .clangd 配置..."
if [ -f ".clangd" ]; then
    echo "✅ .clangd 配置文件存在"
else
    echo "❌ .clangd 配置文件不存在"
fi

# 检查 VS Code 配置
echo ""
echo "4. 检查 VS Code 配置..."
if [ -f ".vscode/settings.json" ]; then
    echo "✅ VS Code settings.json 存在"
    if grep -q "clangd" .vscode/settings.json; then
        echo "✅ clangd 配置已添加到 settings.json"
    else
        echo "❌ settings.json 中未找到 clangd 配置"
    fi
else
    echo "❌ VS Code settings.json 不存在"
fi

# 检查扩展推荐
echo ""
echo "5. 检查扩展推荐..."
if [ -f ".vscode/extensions.json" ]; then
    echo "✅ extensions.json 存在"
    if grep -q "llvm-vs-code-extensions.vscode-clangd" .vscode/extensions.json; then
        echo "✅ clangd 扩展已添加到推荐列表"
    else
        echo "❌ clangd 扩展未在推荐列表中"
    fi
else
    echo "❌ extensions.json 不存在"
fi

echo ""
echo "🎉 配置验证完成！"
echo ""
echo "📝 下一步："
echo "1. 在 VS Code 中安装 clangd 扩展"
echo "2. 禁用 C/C++ IntelliSense"
echo "3. 重启 VS Code"
echo "4. 打开 C++ 文件验证语法补全功能"