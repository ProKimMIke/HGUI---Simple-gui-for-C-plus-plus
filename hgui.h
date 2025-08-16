#include "base.h"

// 全局变量
static HINSTANCE hInstance;
static HGUI_Control* controls = NULL;
static HWND main_window_hwnd = NULL;
static UINT_PTR next_menu_id = 1000;  // 菜单ID计数器

// 窗口过程声明
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

// 辅助函数：查找控件（公开供演示程序使用）
HGUI_Control* find_control(const char* id) {
	HGUI_Control* current = controls;
	while (current) {
		if (strcmp(current->id, id) == 0) {
			return current;
		}
		current = current->next;
	}
	return NULL;
}

// 辅助函数：查找父窗口句柄
static HWND find_parent_hwnd(const char* parent_id) {
	if (parent_id == NULL) return NULL;
	
	HGUI_Control* parent = find_control(parent_id);
	if (parent) {
		return parent->hwnd;
	}
	return NULL;
}

// 辅助函数：通过菜单ID查找菜单项
static HGUI_Control* find_menu_item_by_id(UINT_PTR menu_id) {
	HGUI_Control* current = controls;
	while (current) {
		if (current->type == HGUI_MENUITEM && current->menu_id == menu_id) {
			return current;
		}
		current = current->next;
	}
	return NULL;
}

// 辅助函数：取消同组中其他单选框的选中状态
static void uncheck_radio_group(const char* current_id) {
	HGUI_Control* current_control = find_control(current_id);
	if (!current_control || current_control->type != HGUI_RADIO) return;
	
	// 遍历所有单选框，取消同组中其他单选框的选中状态
	HGUI_Control* control = controls;
	while (control) {
		if (control->type == HGUI_RADIO && 
			strcmp(control->parent_id, current_control->parent_id) == 0 &&
			strcmp(control->id, current_id) != 0) {
			
			hgui.setCheck(control->id, false);
		}
		control = control->next;
	}
}

// 注册窗口类
static void register_window_class(const char* class_name) {
	WNDCLASSEX wc = {0};
	wc.cbSize        = sizeof(WNDCLASSEX);
	wc.style         = CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS;
	wc.lpfnWndProc   = WndProc;
	wc.hInstance     = hInstance;
	wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszClassName = class_name;
	
	RegisterClassEx(&wc);
}

// 窗口过程实现
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
		case WM_COMMAND: {
		// 处理菜单项点击 (lParam为0表示菜单命令)
		if (lParam == 0) {
			UINT_PTR menu_id = (UINT_PTR)LOWORD(wParam);
			HGUI_Control* item = find_menu_item_by_id(menu_id);
			
			if (item && item->click_callback) {
				item->click_callback(item->id);
				return 0;
			}
		}
		// 处理控件消息
		else {
			HWND child_hwnd = (HWND)lParam;
			HGUI_Control* control = NULL;
			
			// 根据句柄查找控件
			HGUI_Control* current = controls;
			while (current) {
				if (current->hwnd == child_hwnd) {
					control = current;
					break;
				}
				current = current->next;
			}
			
			if (control) {
				// 处理按钮点击
				if (HIWORD(wParam) == BN_CLICKED && control->click_callback) {
					control->click_callback(control->id);
				}
				// 处理列表框双击
				else if (HIWORD(wParam) == LBN_DBLCLK && control->dblclick_callback) {
					control->dblclick_callback(control->id);
				}
				// 处理单选框/复选框状态变化
				else if ((control->type == HGUI_RADIO || control->type == HGUI_CHECKBOX) &&
						 HIWORD(wParam) == BN_CLICKED) {
					
					// 对于单选框，先处理组内互斥
					if (control->type == HGUI_RADIO) {
						// 确保点击单选框时设置为选中状态
						hgui.setCheck(control->id, true);
						uncheck_radio_group(control->id);
					}
					// 对于复选框，切换状态
					else if (control->type == HGUI_CHECKBOX) {
						bool checked = hgui.getCheck(control->id);
						hgui.setCheck(control->id, !checked);
					}
					
					// 触发change事件
					if (control->change_callback) {
						control->change_callback(control->id);
					}
				}
			}
		}
		break;
	}
		
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
		
	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}

// 核心功能实现
static void hgui_init(void) {
	hInstance = GetModuleHandle(NULL);
}

static int hgui_run(void) {
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0) > 0) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return (int)msg.wParam;
}

static void hgui_cleanup(void) {
	// 释放所有控件
	HGUI_Control* current = controls;
	while (current) {
		HGUI_Control* next = current->next;
		
		// 销毁窗口句柄
		if (current->hwnd) {
			DestroyWindow(current->hwnd);
		}
		
		// 销毁菜单句柄
		if (current->hmenu) {
			DestroyMenu(current->hmenu);
		}
		
		// 释放内存
		free(current->id);
		if (current->parent_id) free(current->parent_id);
		free(current);
		
		current = next;
	}
	controls = NULL;
	main_window_hwnd = NULL;
	next_menu_id = 1000;
}

// 控件操作实现
static void hgui_remove(const char* id) {
	if (!id) return;
	
	HGUI_Control* current = controls;
	HGUI_Control* prev = NULL;
	
	// 查找要删除的控件
	while (current) {
		if (strcmp(current->id, id) == 0) {
			// 从链表中移除控件
			if (prev) {
				prev->next = current->next;
			} else {
				controls = current->next;
			}
			
			// 销毁窗口句柄
			if (current->hwnd) {
				DestroyWindow(current->hwnd);
			}
			
			// 销毁菜单句柄
			if (current->hmenu) {
				DestroyMenu(current->hmenu);
			}
			
			// 如果删除的是主窗口，更新主窗口句柄
			if (current->type == HGUI_WINDOW && current->hwnd == main_window_hwnd) {
				main_window_hwnd = NULL;
			}
			
			// 释放内存
			free(current->id);
			if (current->parent_id) free(current->parent_id);
			free(current);
			return;
		}
		
		prev = current;
		current = current->next;
	}
}

// 隐藏控件
static void hgui_hide(const char* id) {
	HGUI_Control* control = find_control(id);
	if (control && control->hwnd) {
		ShowWindow(control->hwnd, SW_HIDE);
		// 通知父窗口重绘
		if (control->parent_id) {
			HWND parent_hwnd = find_parent_hwnd(control->parent_id);
			if (parent_hwnd) {
				InvalidateRect(parent_hwnd, NULL, TRUE);
			}
		}
	}
}

// 显示控件
static void hgui_show(const char* id) {
	HGUI_Control* control = find_control(id);
	if (control && control->hwnd) {
		ShowWindow(control->hwnd, SW_SHOW);
		// 通知父窗口重绘
		if (control->parent_id) {
			HWND parent_hwnd = find_parent_hwnd(control->parent_id);
			if (parent_hwnd) {
				InvalidateRect(parent_hwnd, NULL, TRUE);
			}
		}
	}
}

static void hgui_bind(const char* id, const char* event, void (*callback)(const char*)) {
	HGUI_Control* control = find_control(id);
	if (!control || !event || !callback) return;
	
	if (strcmp(event, "click") == 0) {
		control->click_callback = callback;
	} else if (strcmp(event, "dblclick") == 0) {
		control->dblclick_callback = callback;
	} else if (strcmp(event, "change") == 0) {
		control->change_callback = callback;
	}
}

static void hgui_setText(const char* id, const char* text) {
	HGUI_Control* control = find_control(id);
	if (control && control->hwnd && text) {
		SendMessage(control->hwnd, WM_SETTEXT, 0, (LPARAM)text);
	}
}

static void hgui_getText(const char* id, char* buffer, int buffer_size) {
	HGUI_Control* control = find_control(id);
	if (control && control->hwnd && buffer && buffer_size > 0) {
		SendMessage(control->hwnd, WM_GETTEXT, (WPARAM)buffer_size, (LPARAM)buffer);
	}
}

static void hgui_addItem(const char* list_id, const char* item_text) {
	HGUI_Control* control = find_control(list_id);
	if (control && control->type == HGUI_LISTBOX && item_text) {
		SendMessage(control->hwnd, LB_ADDSTRING, 0, (LPARAM)item_text);
	}
}

static void hgui_removeItem(const char* list_id, int index) {
	HGUI_Control* control = find_control(list_id);
	if (control && control->type == HGUI_LISTBOX && index >= 0) {
		SendMessage(control->hwnd, LB_DELETESTRING, (WPARAM)index, 0);
	}
}

static void hgui_clearList(const char* list_id) {
	HGUI_Control* control = find_control(list_id);
	if (control && control->type == HGUI_LISTBOX) {
		SendMessage(control->hwnd, LB_RESETCONTENT, 0, 0);
	}
}

static int hgui_getSelectedIndex(const char* list_id) {
	HGUI_Control* control = find_control(list_id);
	if (control && control->type == HGUI_LISTBOX) {
		return (int)SendMessage(control->hwnd, LB_GETCURSEL, 0, 0);
	}
	return -1;
}

static void hgui_getListItem(const char* list_id, int index, char* buffer, int buffer_size) {
	HGUI_Control* control = find_control(list_id);
	if (control && control->type == HGUI_LISTBOX && index >= 0 && buffer && buffer_size > 0) {
		// 先获取项目文本长度
		int length = (int)SendMessage(control->hwnd, LB_GETTEXTLEN, (WPARAM)index, 0);
		if (length >= buffer_size - 1) length = buffer_size - 2;
		
		// 获取项目文本
		SendMessage(control->hwnd, LB_GETTEXT, (WPARAM)index, (LPARAM)buffer);
		buffer[length + 1] = '\0'; // 确保字符串终止
	}
}

static void hgui_setCheck(const char* id, bool checked) {
	HGUI_Control* control = find_control(id);
	if (!control || (control->type != HGUI_CHECKBOX && control->type != HGUI_RADIO)) return;
	
	// 设置复选框/单选框状态
	SendMessage(control->hwnd, BM_SETCHECK, checked ? BST_CHECKED : BST_UNCHECKED, 0);
	
	// 视觉反馈：选中状态使用粗体（修复了BOLD_FONT未定义的问题）
	HFONT font;
	if (checked) {
		// 获取默认字体信息
		LOGFONT lf;
		GetObject((HFONT)GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONT), &lf);
		// 设置为粗体
		lf.lfWeight = FW_BOLD;
		// 创建粗体字体
		font = CreateFontIndirect(&lf);
	} else {
		// 使用默认字体
		font = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
	}
	SendMessage(control->hwnd, WM_SETFONT, (WPARAM)font, TRUE);
}

static bool hgui_getCheck(const char* id) {
	HGUI_Control* control = find_control(id);
	if (!control || (control->type != HGUI_CHECKBOX && control->type != HGUI_RADIO)) return false;
	
	return SendMessage(control->hwnd, BM_GETCHECK, 0, 0) == BST_CHECKED;
}

// 创建控件函数实现
static void hgui_create_window(const char* id, const char* title, int x, int y, int width, int height) {
	const char* class_name = "HGUI_WindowClass";
	register_window_class(class_name);
	
	// 分配控件结构体
	HGUI_Control* control = (HGUI_Control*)malloc(sizeof(HGUI_Control));
	control->id = _strdup(id);
	control->parent_id = NULL;
	control->type = HGUI_WINDOW;
	control->hwnd = NULL;
	control->hmenu = NULL;
	control->menu_id = 0;
	control->is_submenu = false;
	control->click_callback = NULL;
	control->dblclick_callback = NULL;
	control->change_callback = NULL;
	
	// 创建窗口（添加WS_CLIPCHILDREN确保菜单正确显示）
	control->hwnd = CreateWindowEx(
								   WS_EX_CLIENTEDGE,
								   class_name,
								   title,
								   WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
								   x, y, width, height,
								   NULL, NULL, hInstance, NULL
								   );
	
	// 添加到控件链表
	control->next = controls;
	controls = control;
	
	// 显示窗口
	ShowWindow(control->hwnd, SW_SHOW);
	UpdateWindow(control->hwnd);
	
	main_window_hwnd = control->hwnd;
}

static void hgui_create_label(const char* id, const char* parent_id, const char* text, 
							  int x, int y, int width, int height) {
	HWND parent_hwnd = find_parent_hwnd(parent_id);
	if (!parent_hwnd) return;
	
	// 分配控件结构体
	HGUI_Control* control = (HGUI_Control*)malloc(sizeof(HGUI_Control));
	control->id = _strdup(id);
	control->parent_id = _strdup(parent_id);
	control->type = HGUI_LABEL;
	control->hwnd = NULL;
	control->hmenu = NULL;
	control->menu_id = 0;
	control->is_submenu = false;
	control->click_callback = NULL;
	control->dblclick_callback = NULL;
	control->change_callback = NULL;
	
	// 创建标签
	control->hwnd = CreateWindowEx(
								   0, "STATIC", text,
								   WS_CHILD | WS_VISIBLE | SS_LEFT,
								   x, y, width, height,
								   parent_hwnd, NULL, hInstance, NULL
								   );
	
	// 添加到控件链表
	control->next = controls;
	controls = control;
}

static void hgui_create_button(const char* id, const char* parent_id, const char* text,
							   int x, int y, int width, int height) {
	HWND parent_hwnd = find_parent_hwnd(parent_id);
	if (!parent_hwnd) return;
	
	// 分配控件结构体
	HGUI_Control* control = (HGUI_Control*)malloc(sizeof(HGUI_Control));
	control->id = _strdup(id);
	control->parent_id = _strdup(parent_id);
	control->type = HGUI_BUTTON;
	control->hwnd = NULL;
	control->hmenu = NULL;
	control->menu_id = 0;
	control->is_submenu = false;
	control->click_callback = NULL;
	control->dblclick_callback = NULL;
	control->change_callback = NULL;
	
	// 创建按钮
	control->hwnd = CreateWindowEx(
								   0, "BUTTON", text,
								   WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
								   x, y, width, height,
								   parent_hwnd, NULL, hInstance, NULL
								   );
	
	// 添加到控件链表
	control->next = controls;
	controls = control;
}

static void hgui_create_input(const char* id, const char* parent_id,
							  int x, int y, int width, int height) {
	HWND parent_hwnd = find_parent_hwnd(parent_id);
	if (!parent_hwnd) return;
	
	// 分配控件结构体
	HGUI_Control* control = (HGUI_Control*)malloc(sizeof(HGUI_Control));
	control->id = _strdup(id);
	control->parent_id = _strdup(parent_id);
	control->type = HGUI_INPUT;
	control->hwnd = NULL;
	control->hmenu = NULL;
	control->menu_id = 0;
	control->is_submenu = false;
	control->click_callback = NULL;
	control->dblclick_callback = NULL;
	control->change_callback = NULL;
	
	// 创建输入框
	control->hwnd = CreateWindowEx(
								   WS_EX_CLIENTEDGE, "EDIT", "",
								   WS_CHILD | WS_VISIBLE | ES_LEFT | ES_AUTOHSCROLL,
								   x, y, width, height,
								   parent_hwnd, NULL, hInstance, NULL
								   );
	
	// 添加到控件链表
	control->next = controls;
	controls = control;
}

static void hgui_create_listbox(const char* id, const char* parent_id,
								int x, int y, int width, int height) {
	HWND parent_hwnd = find_parent_hwnd(parent_id);
	if (!parent_hwnd) return;
	
	// 分配控件结构体
	HGUI_Control* control = (HGUI_Control*)malloc(sizeof(HGUI_Control));
	control->id = _strdup(id);
	control->parent_id = _strdup(parent_id);
	control->type = HGUI_LISTBOX;
	control->hwnd = NULL;
	control->hmenu = NULL;
	control->menu_id = 0;
	control->is_submenu = false;
	control->click_callback = NULL;
	control->dblclick_callback = NULL;
	control->change_callback = NULL;
	
	// 创建列表框
	control->hwnd = CreateWindowEx(
								   WS_EX_CLIENTEDGE, "LISTBOX", "",
								   WS_CHILD | WS_VISIBLE | LBS_NOTIFY | WS_VSCROLL,
								   x, y, width, height,
								   parent_hwnd, NULL, hInstance, NULL
								   );
	
	// 添加到控件链表
	control->next = controls;
	controls = control;
}

static void hgui_create_radio(const char* id, const char* parent_id, const char* text,
							  int x, int y, int width, int height, bool is_group_first) {
	HWND parent_hwnd = find_parent_hwnd(parent_id);
	if (!parent_hwnd) return;
	
	// 分配控件结构体
	HGUI_Control* control = (HGUI_Control*)malloc(sizeof(HGUI_Control));
	control->id = _strdup(id);
	control->parent_id = _strdup(parent_id);
	control->type = HGUI_RADIO;
	control->hwnd = NULL;
	control->hmenu = NULL;
	control->menu_id = 0;
	control->is_submenu = false;
	control->click_callback = NULL;
	control->dblclick_callback = NULL;
	control->change_callback = NULL;
	
	// 单选框样式：WS_GROUP用于标记一组单选框的第一个
	DWORD style = WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON;
	if (is_group_first) {
		style |= WS_GROUP;
	}
	
	control->hwnd = CreateWindowEx(
								   0, "BUTTON", text,
								   style,
								   x, y, width, height,
								   parent_hwnd, NULL, hInstance, NULL
								   );
	
	// 添加到控件链表
	control->next = controls;
	controls = control;
}

static void hgui_create_checkbox(const char* id, const char* parent_id, const char* text,
								 int x, int y, int width, int height) {
	HWND parent_hwnd = find_parent_hwnd(parent_id);
	if (!parent_hwnd) return;
	
	// 分配控件结构体
	HGUI_Control* control = (HGUI_Control*)malloc(sizeof(HGUI_Control));
	control->id = _strdup(id);
	control->parent_id = _strdup(parent_id);
	control->type = HGUI_CHECKBOX;
	control->hwnd = NULL;
	control->hmenu = NULL;
	control->menu_id = 0;
	control->is_submenu = false;
	control->click_callback = NULL;
	control->dblclick_callback = NULL;
	control->change_callback = NULL;
	
	control->hwnd = CreateWindowEx(
								   0, "BUTTON", text,
								   WS_CHILD | WS_VISIBLE | BS_CHECKBOX,
								   x, y, width, height,
								   parent_hwnd, NULL, hInstance, NULL
								   );
	
	// 添加到控件链表
	control->next = controls;
	controls = control;
}

static void hgui_create_menubar(const char* id, const char* parent_id) {
	HWND parent_hwnd = find_parent_hwnd(parent_id);
	if (!parent_hwnd) return;
	
	// 分配控件结构体
	HGUI_Control* control = (HGUI_Control*)malloc(sizeof(HGUI_Control));
	control->id = _strdup(id);
	control->parent_id = _strdup(parent_id);
	control->type = HGUI_MENUBAR;
	control->hwnd = parent_hwnd;  // 菜单栏关联到父窗口
	control->hmenu = CreateMenu(); // 创建主菜单
	control->menu_id = 0;
	control->is_submenu = true;    // 菜单栏是顶级菜单容器
	control->click_callback = NULL;
	control->dblclick_callback = NULL;
	control->change_callback = NULL;
	
	// 设置窗口菜单
	SetMenu(parent_hwnd, control->hmenu);
	DrawMenuBar(parent_hwnd);  // 强制重绘菜单
	
	// 添加到控件链表
	control->next = controls;
	controls = control;
}

static void hgui_create_addMenuItem(const char* parent_id, const char* id, const char* text, bool is_submenu) {
	HGUI_Control* parent = find_control(parent_id);
	if (!parent || !parent->is_submenu || !parent->hmenu) return;
	
	// 分配控件结构体
	HGUI_Control* item = (HGUI_Control*)malloc(sizeof(HGUI_Control));
	item->id = _strdup(id);
	item->parent_id = _strdup(parent_id);
	item->type = HGUI_MENUITEM;
	item->hwnd = parent->hwnd;  // 关联到父窗口
	item->hmenu = NULL;
	item->menu_id = 0;
	item->is_submenu = is_submenu;
	item->click_callback = NULL;
	item->dblclick_callback = NULL;
	item->change_callback = NULL;
	
	if (is_submenu) {
		// 子菜单容器
		item->hmenu = CreatePopupMenu();
		AppendMenu(parent->hmenu, MF_STRING | MF_POPUP, 
				   (UINT_PTR)item->hmenu, text);
	} else {
		// 普通菜单项
		item->hmenu = NULL;
		
		// 处理分隔线（空文本）
		if (text && strlen(text) == 0) {
			AppendMenu(parent->hmenu, MF_SEPARATOR, 0, NULL);
			item->menu_id = 0;
		} else {
			item->menu_id = next_menu_id++;  // 分配唯一ID
			AppendMenu(parent->hmenu, MF_STRING, 
					   (UINT_PTR)item->menu_id, text);
		}
	}
	
	// 添加到控件链表
	item->next = controls;
	controls = item;
	
	// 强制重绘菜单
	DrawMenuBar(parent->hwnd);
}

// 命名空间实例初始化
const HGUI_Namespace hgui = {
	// 核心功能
	.init = hgui_init,
	.run = hgui_run,
	.cleanup = hgui_cleanup,
	
	// 控件操作
	.remove = hgui_remove,
	.hide = hgui_hide,
	.show = hgui_show,
	.bind = hgui_bind,
	.setText = hgui_setText,
	.getText = hgui_getText,
	.addItem = hgui_addItem,
	.removeItem = hgui_removeItem,
	.clearList = hgui_clearList,
	.getSelectedIndex = hgui_getSelectedIndex,
	.getListItem = hgui_getListItem,
	.setCheck = hgui_setCheck,
	.getCheck = hgui_getCheck,
	
	// 创建控件的子命名空间
	.create = {
		.window = hgui_create_window,
		.label = hgui_create_label,
		.button = hgui_create_button,
		.input = hgui_create_input,
		.listbox = hgui_create_listbox,
		.radio = hgui_create_radio,
		.checkbox = hgui_create_checkbox,
		.menubar = hgui_create_menubar,
		.addMenuItem = hgui_create_addMenuItem
	}
};

