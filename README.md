# HGUI - 轻量级Windows GUI库

HGUI是一个基于Windows API的轻量级GUI库，旨在简化Windows平台下的图形界面开发。它提供了一套简洁的API，支持常见的GUI控件和事件处理，适合快速开发小型Windows应用程序。

## 特性

- 轻量级设计，无额外依赖，仅依赖Windows系统库
- 支持多种常用控件：窗口、标签、按钮、输入框、列表框、单选框、复选框、菜单栏等
- 简单易用的API，统一的命名空间设计
- 完善的事件处理机制（点击、双击、状态变化等）
- 支持控件的动态创建、删除、显示和隐藏
- 内存管理完善，确保资源正确释放

## 快速开始

### 环境要求

- Windows操作系统
- MinGW或MSVC等C编译器
- 支持C99标准的开发环境

### 编译方法

1. 将`hgui.h`和`hgui.c`添加到你的项目中
2. 编译时链接必要的Windows系统库

使用MinGW编译：
```bash
gcc your_program.c hgui.c -o your_program.exe -lgdi32 -luser32
```

使用MSVC编译：
```bash
cl your_program.c hgui.c gdi32.lib user32.lib
```

### 简单示例

```c
#include "hgui.h"
#include <stdio.h>

// 按钮点击事件回调
void on_button_click(const char* id) {
    printf("按钮 %s 被点击了!\n", id);
    hgui.setText("status_label", "按钮已被点击");
}

// 复选框状态变化回调
void on_checkbox_change(const char* id) {
    bool checked = hgui.getCheck(id);
    if (checked) {
        hgui.setText("status_label", "复选框已选中");
    } else {
        hgui.setText("status_label", "复选框已取消");
    }
}

int main() {
    // 初始化HGUI库
    hgui.init();
    
    // 创建主窗口
    hgui.create.window("main_window", "HGUI示例程序", 100, 100, 400, 300);
    
    // 创建标签
    hgui.create.label("title_label", "main_window", "欢迎使用HGUI库", 20, 20, 300, 20);
    hgui.create.label("status_label", "main_window", "请操作控件...", 20, 260, 300, 20);
    
    // 创建按钮
    hgui.create.button("test_button", "main_window", "点击我", 20, 60, 100, 30);
    hgui.bind("test_button", "click", on_button_click);
    
    // 创建复选框
    hgui.create.checkbox("test_checkbox", "main_window", "选择我", 20, 110, 150, 20);
    hgui.bind("test_checkbox", "change", on_checkbox_change);
    
    // 创建单选框组
    hgui.create.radio("radio1", "main_window", "选项一", 20, 150, 150, 20, true);
    hgui.create.radio("radio2", "main_window", "选项二", 20, 180, 150, 20, false);
    
    // 创建输入框
    hgui.create.input("input_box", "main_window", 200, 60, 150, 25);
    
    // 创建列表框并添加项目
    hgui.create.listbox("list_box", "main_window", 200, 110, 150, 100);
    hgui.addItem("list_box", "列表项 1");
    hgui.addItem("list_box", "列表项 2");
    hgui.addItem("list_box", "列表项 3");
    
    // 运行消息循环
    int result = hgui.run();
    
    // 清理资源
    hgui.cleanup();
    
    return result;
}
```

## 支持的控件

| 控件类型 | 创建方法 | 说明 |
|---------|---------|------|
| 窗口 | `hgui.create.window()` | 主窗口控件 |
| 标签 | `hgui.create.label()` | 文本显示控件 |
| 按钮 | `hgui.create.button()` | 点击按钮控件 |
| 输入框 | `hgui.create.input()` | 文本输入控件 |
| 列表框 | `hgui.create.listbox()` | 列表选择控件 |
| 单选框 | `hgui.create.radio()` | 单选按钮控件 |
| 复选框 | `hgui.create.checkbox()` | 复选框控件 |
| 菜单栏 | `hgui.create.menubar()` | 菜单容器控件 |
| 菜单项 | `hgui.create.addMenuItem()` | 菜单选项控件 |

## 常用API

### 核心功能
- `hgui.init()`: 初始化库
- `hgui.run()`: 运行消息循环
- `hgui.cleanup()`: 清理资源

### 控件操作
- `hgui.show(id)`: 显示控件
- `hgui.hide(id)`: 隐藏控件
- `hgui.remove(id)`: 移除控件
- `hgui.bind(id, event, callback)`: 绑定事件回调
- `hgui.setText(id, text)`: 设置控件文本
- `hgui.getText(id, buffer, size)`: 获取控件文本

## 常见问题

### 编译错误：undefined reference to `__imp_xxx`
这类错误通常是由于缺少必要的系统库链接。确保编译时添加了`-lgdi32 -luser32`参数（MinGW）或链接了相应的库（MSVC）。

### 编译错误：cannot find -lgdi32
这表明编译器无法找到gdi32库，通常是因为MinGW安装不完整。请重新安装完整的MinGW版本，并确保环境变量配置正确。

## 许可证
本项目采用MIT许可证，详情请见LICENSE文件。

## 贡献
欢迎提交issue和pull request，帮助改进这个库。在提交代码前，请确保代码符合项目的编码规范。
