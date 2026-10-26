# lab2editor — 简单文本编辑器（Qt 实验二）

Qt Widgets 单窗口文本编辑器，配合《实验2-简单文本编辑器》完成。界面用 Qt Designer（`.ui`）搭建，
功能用 C++ 实现，菜单栏与工具栏共享同一批 `QAction`。

## 环境

| 项 | 值 |
|---|---|
| Qt | 6.11.2（mingw_64），`D:\soft\Qt\6.11.2\mingw_64` |
| 编译器 | MinGW-w64 GCC 13.1.0，`D:\soft\Qt\Tools\mingw1310_64` |
| 构建 | qmake + mingw32-make |
| Qt Creator Kit | `Desktop_Qt_6_11_2_MinGW_64_bit` |

## 构建与运行

Qt Creator：打开 `lab2editor.pro` → 选择上面的 kit → Ctrl+R。

命令行：

```powershell
$env:Path = 'D:\soft\Qt\6.11.2\mingw_64\bin;D:\soft\Qt\Tools\mingw1310_64\bin;' + $env:Path
mkdir build; cd build
qmake ..\lab2editor.pro -spec win32-g++ "CONFIG+=debug"
mingw32-make -j8
.\debug\lab2editor.exe
```

> 直接双击 exe 需要 Qt 运行库：把 `mingw_64\bin` 加入 PATH，或执行
> `windeployqt debug\lab2editor.exe` 做成绿色包。

## 功能

- 文件：新建、打开、保存、另存为；关闭/新建/打开前对未保存内容提示（保存/不保存/取消），
  保存失败会保留"已修改"标记，打开失败不会清空当前内容。
- 编辑：撤销、恢复、剪切、复制、粘贴、全选；使能状态随选区和撤销栈自动变化（粘贴还跟随剪贴板）。
- 查找/替换（非模态对话框）：向上/向下、区分大小写、到文末自动回绕、"未找到"提示、
  全部替换（可一次撤销整体回退，替换为空串不会死循环）。
- 格式：字体（含字号）、字体颜色、编辑器背景色、自动换行。
- 视图：工具栏、状态栏显隐；行号显示/隐藏（Ctrl+L）。
- 状态栏：`length` / `lines` / `Ln` / `Col` 实时更新；标题随修改状态显示 `*`。
- 帮助：关于（`QMessageBox::about`）。

快捷键：Ctrl+N/O/S、Ctrl+Shift+S、Ctrl+Z、Ctrl+Shift+Z、Ctrl+X/C/V/A、Ctrl+F、Ctrl+H、Ctrl+L、Ctrl+Q。

## 目录结构

```
lab2editor/
├── lab2editor.pro
├── main.cpp
├── mainwindow.h/.cpp/.ui     主窗口：菜单、工具栏、状态栏、文件读写、查找替换算法
├── codeeditor.h/.cpp         QPlainTextEdit 派生类 + 行号区（Designer 中提升为 CodeEditor）
├── finddialog.h/.cpp/.ui     查找对话框（非模态，只发信号）
├── replacedialog.h/.cpp/.ui  替换对话框（非模态，只发信号）
└── .gitignore
```

## 设计要点

1. **Action 单一来源**：一个功能只建一个 `QAction`，菜单和工具栏 `addAction` 同一个对象，
   使能状态天然同步。
2. **对话框只发信号**：`findRequested / replaceRequested / replaceAllRequested`，
   查找替换算法只在 `MainWindow` 里实现一份。
3. **文件状态唯一**：`m_filePath` 只在成功读取/成功写入后修改；`document()->isModified()`
   驱动标题 `[*]` 和关闭提示。
4. **编码与换行**：统一 UTF-8（无 BOM）、Windows 下写 CRLF、读取时统一成 LF。
5. **原子写入**：`QSaveFile`，写失败不会破坏原文件，也不会清除修改标记。

## 已知限制

- 定时自动保存（选做）未实现。
- 只支持 UTF-8 文本；不支持 GBK/UTF-16 自动探测。
- 单文档窗口；工具栏图标为程序内绘制的字符图标（无 png 资源），可替换为真实图标。
- 行号按逻辑行编号，开启自动换行后视觉行与行号不一一对应（与 Qt codeeditor 示例一致）。
