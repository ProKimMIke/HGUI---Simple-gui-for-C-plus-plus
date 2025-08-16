#ifndef HGUI_H
#define HGUI_H

#include <windows.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

// 控件类型枚举
typedef enum {
	HGUI_WINDOW,
	HGUI_LABEL,
	HGUI_BUTTON,
	HGUI_INPUT,
	HGUI_LISTBOX,
	HGUI_RADIO,
	HGUI_CHECKBOX,
	HGUI_MENUBAR,
	HGUI_MENUITEM
} HGUI_ControlType;

// 控件结构体前向声明
typedef struct HGUI_Control HGUI_Control;

// 控件结构体定义
struct HGUI_Control {
	char* id;                   // 控件ID
	char* parent_id;            // 父控件ID
	HGUI_ControlType type;      // 控件类型
	HWND hwnd;                  // 窗口句柄
	HMENU hmenu;                // 菜单句柄
	UINT_PTR menu_id;           // 菜单项ID
	bool is_submenu;            // 是否为子菜单
	
	// 回调函数
	void (*click_callback)(const char* id);
	void (*dblclick_callback)(const char* id);
	void (*change_callback)(const char* id);
	
	HGUI_Control* next;         // 链表中的下一个控件
};

// 创建控件的函数指针结构体
typedef struct {
	void (*window)(const char* id, const char* title, int x, int y, int width, int height);
	void (*label)(const char* id, const char* parent_id, const char* text, int x, int y, int width, int height);
	void (*button)(const char* id, const char* parent_id, const char* text, int x, int y, int width, int height);
	void (*input)(const char* id, const char* parent_id, int x, int y, int width, int height);
	void (*listbox)(const char* id, const char* parent_id, int x, int y, int width, int height);
	void (*radio)(const char* id, const char* parent_id, const char* text, int x, int y, int width, int height, bool is_group_first);
	void (*checkbox)(const char* id, const char* parent_id, const char* text, int x, int y, int width, int height);
	void (*menubar)(const char* id, const char* parent_id);
	void (*addMenuItem)(const char* parent_id, const char* id, const char* text, bool is_submenu);
} HGUI_CreateFunctions;

// HGUI命名空间结构体
typedef struct {
	// 核心功能
	void (*init)(void);
	int (*run)(void);
	void (*cleanup)(void);
	
	// 控件操作
	void (*remove)(const char* id);
	void (*hide)(const char* id);
	void (*show)(const char* id);
	void (*bind)(const char* id, const char* event, void (*callback)(const char*));
	void (*setText)(const char* id, const char* text);
	void (*getText)(const char* id, char* buffer, int buffer_size);
	void (*addItem)(const char* list_id, const char* item_text);
	void (*removeItem)(const char* list_id, int index);
	void (*clearList)(const char* list_id);
	int (*getSelectedIndex)(const char* list_id);
	void (*getListItem)(const char* list_id, int index, char* buffer, int buffer_size);
	void (*setCheck)(const char* id, bool checked);
	bool (*getCheck)(const char* id);
	
	// 创建控件的子命名空间
	HGUI_CreateFunctions create;
} HGUI_Namespace;

// 全局命名空间实例
extern const HGUI_Namespace hgui;

#endif // HGUI_H

