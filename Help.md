# HGUI 库功能使用手册

## 核心功能

### 初始化与清理
```c
// 初始化HGUI库
hgui.init();

// 运行消息循环（程序主循环）
int result = hgui.run();  // 返回值为程序退出代码

// 清理资源
hgui.cleanup();
```

## 控件操作通用方法

### 显示/隐藏控件
```c
// 显示指定ID的控件
hgui.show("control_id");

// 隐藏指定ID的控件
hgui.hide("control_id");
```

### 移除控件
```c
// 从界面中移除并销毁指定ID的控件
hgui.remove("control_id");
```

### 事件绑定
```c
// 绑定事件回调函数
// 支持的事件: "click", "dblclick", "change"
hgui.bind("control_id", "event_name", callback_function);

// 示例: 绑定按钮点击事件
void on_click(const char* id) {
    // 处理点击事件
}
hgui.bind("my_button", "click", on_click);
```

## 窗口控件 (Window)

### 创建窗口
```c
hgui.create.window(
    const char* id,          // 窗口唯一ID
    const char* title,       // 窗口标题
    int x,                   // 窗口左上角X坐标
    int y,                   // 窗口左上角Y坐标
    int width,               // 窗口宽度
    int height               // 窗口高度
);

// 示例
hgui.create.window("main_win", "我的应用", 100, 100, 800, 600);
```

### 窗口操作
```c
// 设置窗口标题
hgui.setText("main_win", "新标题");

// 获取窗口标题
char title[256];
hgui.getText("main_win", title, sizeof(title));
```

## 标签控件 (Label)

### 创建标签
```c
hgui.create.label(
    const char* id,          // 标签唯一ID
    const char* parent_id,   // 父控件ID（通常是窗口）
    const char* text,        // 标签文本
    int x,                   // 位置X坐标
    int y,                   // 位置Y坐标
    int width,               // 宽度
    int height               // 高度
);

// 示例
hgui.create.label("info_label", "main_win", "欢迎使用", 20, 20, 200, 20);
```

### 标签操作
```c
// 设置标签文本
hgui.setText("info_label", "新的文本内容");

// 获取标签文本
char text[256];
hgui.getText("info_label", text, sizeof(text));
```

## 按钮控件 (Button)

### 创建按钮
```c
hgui.create.button(
    const char* id,          // 按钮唯一ID
    const char* parent_id,   // 父控件ID
    const char* text,        // 按钮文本
    int x,                   // 位置X坐标
    int y,                   // 位置Y坐标
    int width,               // 宽度
    int height               // 高度
);

// 示例
hgui.create.button("submit_btn", "main_win", "提交", 20, 60, 100, 30);
```

### 按钮操作
```c
// 设置按钮文本
hgui.setText("submit_btn", "确认");

// 获取按钮文本
char btn_text[256];
hgui.getText("submit_btn", btn_text, sizeof(btn_text));

// 绑定点击事件
void on_submit(const char* id) {
    // 处理提交逻辑
}
hgui.bind("submit_btn", "click", on_submit);
```

## 输入框控件 (Input)

### 创建输入框
```c
hgui.create.input(
    const char* id,          // 输入框唯一ID
    const char* parent_id,   // 父控件ID
    int x,                   // 位置X坐标
    int y,                   // 位置Y坐标
    int width,               // 宽度
    int height               // 高度
);

// 示例
hgui.create.input("name_input", "main_win", 140, 60, 200, 30);
```

### 输入框操作
```c
// 设置输入框内容
hgui.setText("name_input", "默认文本");

// 获取输入框内容
char input_text[256];
hgui.getText("name_input", input_text, sizeof(input_text));

// 绑定内容变化事件
void on_input_change(const char* id) {
    // 处理输入变化
}
hgui.bind("name_input", "change", on_input_change);
```

## 列表框控件 (ListBox)

### 创建列表框
```c
hgui.create.listbox(
    const char* id,          // 列表框唯一ID
    const char* parent_id,   // 父控件ID
    int x,                   // 位置X坐标
    int y,                   // 位置Y坐标
    int width,               // 宽度
    int height               // 高度
);

// 示例
hgui.create.listbox("items_list", "main_win", 20, 120, 320, 150);
```

### 列表框操作
```c
// 添加列表项
hgui.addItem("items_list", "项目1");
hgui.addItem("items_list", "项目2");

// 移除指定索引的列表项
hgui.removeItem("items_list", 0);  // 移除第一个项目

// 清空列表
hgui.clearList("items_list");

// 获取选中项索引 (-1表示无选中项)
int selected = hgui.getSelectedIndex("items_list");

// 获取指定索引的列表项文本
char item_text[256];
hgui.getListItem("items_list", 0, item_text, sizeof(item_text));

// 绑定双击事件
void on_item_dblclick(const char* id) {
    int idx = hgui.getSelectedIndex(id);
    // 处理双击逻辑
}
hgui.bind("items_list", "dblclick", on_item_dblclick);
```

## 单选框控件 (Radio)

### 创建单选框
```c
hgui.create.radio(
    const char* id,          // 单选框唯一ID
    const char* parent_id,   // 父控件ID
    const char* text,        // 单选框文本
    int x,                   // 位置X坐标
    int y,                   // 位置Y坐标
    int width,               // 宽度
    int height,              // 高度
    bool is_group_first      // 是否为单选组的第一个（用于分组）
);

// 示例: 创建一组单选框
hgui.create.radio("radio1", "main_win", "选项A", 20, 300, 150, 20, true);
hgui.create.radio("radio2", "main_win", "选项B", 20, 330, 150, 20, false);
hgui.create.radio("radio3", "main_win", "选项C", 20, 360, 150, 20, false);
```

### 单选框操作
```c
// 设置单选框选中状态
hgui.setCheck("radio2", true);  // 选中选项B

// 获取单选框选中状态
bool is_checked = hgui.getCheck("radio1");

// 绑定状态变化事件
void on_radio_change(const char* id) {
    // 处理单选框状态变化
}
hgui.bind("radio1", "change", on_radio_change);
hgui.bind("radio2", "change", on_radio_change);
```

## 复选框控件 (Checkbox)

### 创建复选框
```c
hgui.create.checkbox(
    const char* id,          // 复选框唯一ID
    const char* parent_id,   // 父控件ID
    const char* text,        // 复选框文本
    int x,                   // 位置X坐标
    int y,                   // 位置Y坐标
    int width,               // 宽度
    int height               // 高度
);

// 示例
hgui.create.checkbox("option1", "main_win", "同意条款", 200, 300, 150, 20);
```

### 复选框操作
```c
// 设置复选框选中状态
hgui.setCheck("option1", true);  // 选中复选框

// 获取复选框选中状态
bool is_checked = hgui.getCheck("option1");

// 绑定状态变化事件
void on_checkbox_change(const char* id) {
    bool checked = hgui.getCheck(id);
    // 处理复选框状态变化
}
hgui.bind("option1", "change", on_checkbox_change);
```

## 菜单栏与菜单项 (Menubar & MenuItem)

### 创建菜单栏
```c
hgui.create.menubar(
    const char* id,          // 菜单栏唯一ID
    const char* parent_id    // 父控件ID（通常是窗口）
);

// 示例
hgui.create.menubar("main_menu", "main_win");
```

### 添加菜单项
```c
hgui.create.addMenuItem(
    const char* parent_id,   // 父菜单ID（菜单栏或其他菜单项）
    const char* id,          // 菜单项唯一ID
    const char* text,        // 菜单项文本（空字符串表示分隔线）
    bool is_submenu          // 是否为子菜单容器
);

// 示例: 构建菜单结构
// 创建文件菜单（子菜单容器）
hgui.create.addMenuItem("main_menu", "file_menu", "文件(&F)", true);

// 向文件菜单添加项
hgui.create.addMenuItem("file_menu", "menu_new", "新建(&N)", false);
hgui.create.addMenuItem("file_menu", "menu_open", "打开(&O)", false);
hgui.create.addMenuItem("file_menu", "menu_sep1", "", false);  // 分隔线
hgui.create.addMenuItem("file_menu", "menu_exit", "退出(&X)", false);

// 创建编辑菜单
hgui.create.addMenuItem("main_menu", "edit_menu", "编辑(&E)", true);
hgui.create.addMenuItem("edit_menu", "menu_copy", "复制(&C)", false);
hgui.create.addMenuItem("edit_menu", "menu_paste", "粘贴(&V)", false);
```

### 菜单项操作
```c
// 绑定菜单项点击事件
void on_menu_exit(const char* id) {
    // 处理退出逻辑
    PostQuitMessage(0);
}
hgui.bind("menu_exit", "click", on_menu_exit);

void on_menu_new(const char* id) {
    // 处理新建逻辑
}
hgui.bind("menu_new", "click", on_menu_new);
```

## 使用注意事项

1. 所有控件ID必须唯一，否则会导致不可预期的行为
2. 子控件必须在其父控件创建之后才能创建
3. 单选框需要通过`is_group_first`参数进行分组，同组中只能有一个选中项
4. 编译时需要链接必要的系统库：`-lgdi32 -luser32`（MinGW）或`gdi32.lib user32.lib`（MSVC）
5. 所有字符串操作都应注意缓冲区大小，避免缓冲区溢出

通过以上方法，你可以创建和操作HGUI库中所有支持的控件，构建完整的Windows GUI应用程序。
