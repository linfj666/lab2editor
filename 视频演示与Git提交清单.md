# 实验2 视频演示清单 与 Git 提交说明

> **当前状态**：已推送到 GitHub（public）。
> 仓库地址：<https://github.com/linfj666/lab2editor>（5 次提交、31 个文件、无构建产物）
>
> | # | 哈希 | 提交信息 | 涉及文件 |
> |---|---|---|---|
> | 1 | `261c075` | chore: 初始化 lab2 工程骨架与 .gitignore | .gitignore / .pro / main.cpp / mainwindow.h / mainwindow.ui |
> | 2 | `59c8dbf` | feat: 实现文本编辑器（文件读写/编辑动作/格式/查找替换/行号） | mainwindow.cpp / codeeditor.* / finddialog.* / replacedialog.* / README.md |
> | 3 | `27a8508` | docs: 补充测试报告、界面截图与视频/Git 提交清单 | screenshots/ 11 张 + 测试报告.txt + 本文档 |
> | 4 | `d381215` | chore: 添加 .gitattributes 统一文本文件换行 | .gitattributes |
> | 5 | `a05994d` | chore: 填写作者信息并更新关于对话框截图与说明文档 | mainwindow.cpp（作者常量）+ 截图 + 本文档 |
>
> 下面的第三节是"从零重演一遍"的参考流程，其中第 3、4 次提交已按实际结果调整过；
> 若与实际历史有出入，**以 `git log` 为准**。
> 报告第五节会自动读取真实 `git log`，重新生成报告即可同步，不需要手工改哈希。


配套工程：`D:\大学课程\大三上\Qt\lab2\lab2editor\`
配套报告：`D:\大学课程\大三上\Qt\lab2\实验2-简单文本编辑器_2026秋-已完成.docx`

---

## 一、录屏前的准备（3 分钟）

| 项 | 建议 |
|---|---|
| 窗口 | 只留编辑器一个窗口；关掉 QQ、浏览器、文档，避免弹窗入镜 |
| 分辨率 | 1920×1080，缩放 100%（缩放变了会显得字发虚） |
| 程序 | 先编译好，直接双击 `build\debug\lab2editor.exe`（先在 PATH 里加 `D:\soft\Qt\6.11.2\mingw_64\bin`，或用 Qt Creator 的 Run） |
| 演示文件 | 提前在桌面建好 `演示.txt`（写两行中文），录到"打开"时直接选它 |
| 剪贴板 | 先复制一段文字，录"粘贴"时直接 Ctrl+V |
| 终端 | 提前开好一个 PowerShell，`cd` 到工程目录，录到最后切过去执行 `git log` |
| 时长 | 控制在 3～5 分钟，一段录完不剪辑也可以 |
| 命名 | `实验2_学号_姓名.mp4` |

---

## 二、视频演示流程（按顺序做，每步停 0.5～1 秒让对方看清）

| 时间 | 操作 | 嘴里说的 / 字幕写的 | 这一步证明了什么 |
|---|---|---|---|
| 0:00–0:10 | 双击启动，露出主窗口 | "Qt 6.11.2 + MinGW，实验二 简单文本编辑器" | 开场、环境 |
| 0:10–0:25 | 依次点开 文件 / 编辑 / 格式 / 查看 / 帮助 五个菜单，鼠标划过工具栏 | "菜单和工具栏用的是同一批 Action，按功能分组，分隔线是分组边界" | 混合式 UI、Action 分组 |
| 0:25–0:45 | 点"新建" → 输入两行中英混排文字 | "标题出现星号，行号、状态栏 length/lines/Ln/Col 实时变化" | 新建、行号、状态栏 |
| 0:45–1:10 | Ctrl+S → 另存为对话框 → 输入 `演示.txt` → 保存 | "星号消失，说明保存成功" | 保存 / 另存为 |
| 1:10–1:20 | 关掉窗口，再"文件→打开"选 `演示.txt` | "内容读回来了，标题显示文件名，没有星号" | 打开 |
| 1:20–1:40 | 随便改几个字 → 直接点右上角 × | "没保存，应该提示我" | **未保存提示（必拍）** |
| 1:40–1:50 | 在提示里点"取消" → 窗口还在、内容还在 | "取消就是不退出" | 取消分支 |
| 1:50–2:00 | 再点 × → 这次点"不保存" | "改的内容丢了，但这是我选的" | 不保存分支 |
| 2:00–2:20 | Ctrl+Z 两次、Ctrl+Shift+Z 一次；选中一段 → Ctrl+X → Ctrl+V | "撤销/重做，剪切粘贴；注意按钮灰亮在变" | **撤销/重做（必拍）** + 使能 |
| 2:20–2:45 | 格式→字体（改字号）→ 字体颜色（选红）→ 编辑器背景色（选浅黄）→ 自动换行开关各点一次 | "字体颜色作用于选区，背景色是整个编辑器" | 格式设置 |
| 2:45–3:00 | 格式→显示行号（Ctrl+L）关掉 → 再打开；查看→工具栏/状态栏各关一次再打开 | "菜单勾选和实际显示始终一致" | 视图、toggleViewAction |
| 3:00–3:20 | Ctrl+F → 输入"演示" → 点"查找下一个"两次（到文末）→ 再点一次 | "到文末会自动从头继续，状态栏有提示" | **查找（必拍）** + 回绕 |
| 3:20–3:30 | 查找框里输入 `zzz` → 查找下一个 | "未找到，光标不动" | 未找到分支 |
| 3:30–3:50 | Ctrl+H → 查找"演示"、替换"DEMO" → 点"全部替换" | "提示共替换几处" | **替换（必拍）** |
| 3:50–4:00 | 立刻按一次 Ctrl+Z | "一次撤销就把全部替换退回去了" | beginEditBlock 效果 |
| 4:00–4:20 | 文件→打开 → 选一个不存在的路径（或手动输入不存在的文件名）→ 确定 | "文件不存在，但编辑区内容没被清空" | **异常场景（必拍）** |
| 4:20–4:30 | （可选第二个异常）另存为到 `C:\Windows\` 下 | "保存失败，标题星号还在，说明没被当成保存成功" | 保存失败分支 |
| 4:30–4:45 | 帮助→关于 | "个人信息、版本号、Qt 版本" | 关于对话框 |
| 4:45–5:00 | 切到终端：`git log --oneline --graph --decorate` ，再 `git status` | "构建目录没有进版本库" | **Git 历史（必拍）** |

### 拍摄要点（评分最容易丢的地方）

1. **每次操作后停一下**，让画面里能看清标题星号、状态栏数字、按钮灰亮变化。
2. **未保存提示一定要拍"取消后不退出"**，只拍弹出提示是不完整的。
3. 查找分两次：一次命中、一次"未找到"；替换要拍"全部替换 + 一次撤销还原"。
4. 异常场景至少 1 个，建议拍"打开不存在的文件"，因为现象最直观（内容不丢）。
5. 最后一定要有 `git log` 画面，否则"Git 日志"这条算没交。

---

## 三、Git 提交步骤（已经写完代码的情况下，4 次提交）

> 说明：代码是整体写完的，所以按"骨架 → 功能 → 修复 → 文档"拆分，比较符合真实过程。
> 想拆得更细（UI / 编辑 / 对话框各一次），中间那几个提交单独 checkout 不能编译，属正常现象，报告里说明即可。

### 第 0 步：初始化

```powershell
cd 'D:\大学课程\大三上\Qt\lab2\lab2editor'
git init -b main
git config user.name  "你的姓名"
git config user.email "你的邮箱@example.com"

# 先确认构建产物不会被提交（应输出空的 build/ 相关条目或什么都不输出）
git status --short --ignored | Select-String 'build'
```

`.gitignore` 里已包含 `build*/`、`debug/`、`release/`、`*.user`、`moc_*`、`ui_*`，正常情况下 `git status` 不会列出 `build`。

### 第 1 次提交：工程骨架

```powershell
git add .gitignore lab2editor.pro main.cpp mainwindow.h mainwindow.ui
git commit -m "chore: 初始化 lab2 工程骨架与 .gitignore"
```

### 第 2 次提交：完整功能

```powershell
git add mainwindow.cpp codeeditor.h codeeditor.cpp `
        finddialog.h finddialog.cpp finddialog.ui `
        replacedialog.h replacedialog.cpp replacedialog.ui `
        README.md
git commit -m "feat: 实现文本编辑器（文件读写/编辑动作/格式/查找替换/行号）"
```

### 第 3 次提交：文档与截图

> 实际情况：代码的几处修复（查找回绕、全部替换死循环、段落分隔符、提示框中文按钮）已经包含在第 2 次提交里，
> 所以这一次只提交文档与截图，用 `docs:` 前缀。

```powershell
git add screenshots 测试报告.txt 视频演示与Git提交清单.md
git commit -m "docs: 补充测试报告、界面截图与视频/Git 提交清单"
```

### 第 4 次提交：统一换行（已执行）

```powershell
"* text=auto eol=lf" | Set-Content -Encoding ascii .gitattributes
git add .gitattributes
git commit -m "chore: 添加 .gitattributes 统一文本文件换行"
```

### 推送到 GitHub

```powershell
# 先在 GitHub 新建空仓库（不要勾选 README/.gitignore，否则要先 pull）
git remote add origin https://github.com/你的账号/lab2editor.git
git push -u origin main
```

### 检查

```powershell
git log --oneline --graph --decorate   # 录屏要拍这个
git status                             # 应为 working tree clean
git ls-files | Select-String 'build'   # 应无输出（构建产物没入库）
```

---

## 四、报告里还需要你手工填的地方

| 位置 | 内容 | 说明 |
|---|---|---|
| 表格第 1 行 | 姓名 / 学号 / 班级 | **已填写**：林福佳 / 2024414300110 / 24软卓一班 |
| 表格第 2 行 | 实验地点 / 日期 / 指导老师 | **已填写**：8b302 / 2026 年 10 月 26 日 / 刘文果 |
| `screenshots\05_关于对话框.png` | 姓名 / 学号 / 邮箱 | 已改为真实信息并重新生成 |
| 报告第五部分 | `git log` 列表 | 自动读取真实提交历史（文本形式），无需手工贴图 |

改完个人信息后重新生成截图的方法（可选）：在 Qt Creator 里跑起程序 → 帮助→关于 → 用 Win+Shift+S 截图 → 覆盖 `screenshots\05_关于对话框.png` 即可。

---

## 五、网络问题：GitHub 连不上怎么办

现象：

```
fatal: unable to access 'https://github.com/...': Failed to connect to github.com port 443 after 21109 ms
```

说明是网络层不通（不是权限、不是仓库不存在）。按顺序试：

1. **开代理再推**（最干净）
   ```powershell
   # 先启动代理软件并开启系统代理/TUN，确认端口（Clash Verge 通常 7897，Clash for Windows 7890）
   git config --global http.https://github.com.proxy http://127.0.0.1:7897
   git ls-remote https://github.com/linfj666/lab2editor.git    # 能列出 refs 就通了
   git push -u origin main
   # 用完想撤掉：
   git config --global --unset http.https://github.com.proxy
   ```

2. **试 SSH 的 443 端口**（HTTPS 被拦但 SSH 主机可能能通）
   ```powershell
   ssh -T -p 443 git@ssh.github.com
   ```

3. **Gitee 保底**（不需要代理，今天就能交；历史完全一样）
   ```powershell
   # 到 gitee.com 建空仓库 lab2editor
   git remote add gitee https://gitee.com/账号/lab2editor.git
   git push -u gitee main
   # 网络恢复后再补推 GitHub，哈希不变：
   git push origin main
   ```

4. **完全离线转移**（兜底）
   ```powershell
   git bundle create ..\lab2editor.bundle --all
   # 拷到有网的电脑：git clone lab2editor.bundle → 设 remote → push
   ```

**不要做**：`git config --global http.sslVerify false`（关证书校验）、把 remote 改成第三方加速镜像（token 会经过陌生服务器）、反复重试（每次卡 21 秒）。

**不影响交作业的部分**：3～4 条提交、`git log` 截图、报告的 Git 一节都已就绪，只有"网页 Commits 截图"和"仓库链接"依赖网络。

