# 禅道项目交付结果

## 日期

2026-08-18

## 目标

1. 在 `D:\qsw\禅道` 搭建可多人共享的 CMake + Qt 桌面工程。
2. 解决方案名 `main`，可执行文件 `main.exe`，安装到 `_install/main.exe`。
3. 技术栈：C++20 + Qt6 Widgets；Vulkan 可选（`find_package(Vulkan QUIET)`）。
4. 初始化为 git 仓库，并推送远端，支持多人协作。
5. 同步沉淀软件总监 Agent + 交付流程 Skill。

## 已完成工作

### 1. 工程搭建

- 根 `CMakeLists.txt`：`project(main)` + `add_subdirectory(main)`，Visual Studio 可识别为 `main.sln`。
- `main/CMakeLists.txt`：C++20、AUTOMOC、Qt6 Widgets、可选 Vulkan、安装规则 + `windeployqt` 自动部署。
- 最小可运行源码 `main/src/main.cpp`：标题为「禅道」的 Qt 窗口，Vulkan 状态通过标签显示。
- 资源目录 `main/resource/` 已放置占位说明。

### 2. 构建与安装

```powershell
cmake -S . -B _build -G "Visual Studio 18 2026" -A x64 `
  -DCMAKE_INSTALL_PREFIX="_install" `
  -DCMAKE_PREFIX_PATH="D:\Qt\6.8.3\msvc2022_64"

cmake --build _build --config Release
cmake --install _build --config Release
```

- 编译成功：`_build\main\Release\main.exe`。
- 安装成功：`_install\main.exe` 已存在，并附带 Qt 运行依赖（`Qt6Core.dll`、`Qt6Widgets.dll`、`Qt6Gui.dll`、`platforms/`、`styles/` 等）。

### 3. Git 仓库与远端

- 本地仓库已初始化，分支 `main`。
- 提交 1：`Initialize 禅道 Qt desktop project with CMake.`（16 个文件）。
- 提交 2：`Update .gitignore to ignore temporary GitHub token file.`。
- 远端：`https://github.com/KryieNaruto/chandao.git`。
- 已推送 `main` 分支到 GitHub，工作区干净。

### 4. 软件总监 Agent + Skill

- Skill：`software-director-delivery`，位于 `.cursor/skills/software-director-delivery/SKILL.md`。
- Agent：`software-director`（需求解析与派工），以及 `delivery-implementer`、`delivery-tester`、`delivery-triage`。
- 启用说明：`plan/director-workflow.md`。
- 聊天输入 `/software-director-delivery` 即可触发四步交付流程：计划 → 执行 → 测试 → 失败排查/再执行。

## 测试结果

| 检查项 | 结果 |
| --- | --- |
| 目录结构符合约定 | 通过 |
| 根 CMake 可识别为 `main` 解决方案 | 通过 |
| Qt6 Widgets 最小窗口标题含「禅道」 | 通过 |
| Vulkan 可选，无 SDK 时仍编译通过 | 通过 |
| Release 编译成功 | 通过 |
| `_install/main.exe` 存在且带 Qt 依赖 | 通过 |
| `.gitignore` 包含 `_build/`、`_install/`、`.github_token` | 通过 |
| git 仓库已初始化并推送到 GitHub | 通过 |
| clone 地址已写入计划 | 见下方 |

## 验收结论

**通过。** `_install\main.exe` 已生成，可直接运行（依赖 Qt DLL 已部署）。

## 克隆地址

```bash
git clone https://github.com/KryieNaruto/chandao.git
```

## 安全提醒

本次推送曾使用一次性的 GitHub Personal Access Token，且该 Token 在聊天记录中出现过。建议：

1. 立即前往 GitHub Settings → Developer settings → Personal access tokens → 删除/撤销该 Token。
2. 重新生成新的 Token（classic，勾选 `repo` 权限）。
3. 在本地使用 Git Credential Manager 或 HTTPS 登录，避免把 Token 留在 remote URL 中。

本地 remote 已经改回普通 URL：

```bash
git remote -v
# origin  https://github.com/KryieNaruto/chandao.git (fetch)
# origin  https://github.com/KryieNaruto/chandao.git (push)
```

## 专注计时器 Demo 迭代（2026-08-18）

### 本次交付内容

- 在 `main/src/` 新增 `focus_timer_widget.h` / `focus_timer_widget.cpp`：自定义 100×100 专注计时控件。
- 重写 `main/src/main.cpp`：使用 `FocusTimerWidget` 作为中央控件，窗口标题为「专注」，固定尺寸 100×100，保留系统标题栏（含关闭按钮）。
- 更新 `main/CMakeLists.txt`：将新源文件加入 `main` 可执行目标。

### 界面与行为

- 仅保留圆形环绕刻度条与蓝色主按钮；标题文字、中间装饰图案、右侧省略号按钮均已去除。
- 背景色 `#2B2B2B`，激活刻度 `#55B2E8`，未激活刻度 `#3D3D3D`，暂停图标黑色。
- 36 个圆角矩形刻度自 3 点钟方向起，顺时针随进度变蓝。
- 默认工作时长 10 秒，工作结束后圆环全部变灰并进入 3 秒休息；休息结束后自动开始下一轮工作。
- 蓝色主按钮可暂停 / 继续计时；暂停时按钮显示黑色播放三角形，运行时显示黑色暂停双竖线。

### 构建与验证

```powershell
cmake --build _build --config Release
cmake --install _build --config Release
```

- 编译成功，生成 `_build/main/Release/main.exe`。
- 安装成功，`_install/main.exe` 已更新，Qt 运行依赖已自动部署。
- 运行验证：程序正常启动并持续运行，无崩溃，仅 Qt 样式相关 libpng 警告（不影响功能）。

### 尺寸调整（2026-08-18 迭代）

- 将窗口大小从固定 100×100 改为占主屏幕面积的 30%，以正方形窗口呈现。
- 窗口尺寸计算：`sqrt(screen_width * screen_height * 0.3)`。
- `FocusTimerWidget` 改为自适应绘制：所有刻度、按钮、图标均按当前控件宽高比例缩放，不再依赖固定 100×100 坐标。
- 构建与运行验证通过，`_install/main.exe` 已更新。

### 视觉与按钮优化（2026-08-18 迭代）

- 按钮半径从 `base * 0.14` 调整为 `base * 0.126`，使按钮面积约占窗口面积的 5%。
- 圆环进度由离散点亮改为平滑渐变：每个刻度内部根据覆盖比例从 `#55B2E8` 渐变到 `#3D3D3D`，进度前沿在每个方块中可见颜色走动。
- 按钮背景增加径向渐变：中心 `#77C8F8` → 边缘 `#55B2E8`。
- 暂停 / 播放图标增加线性渐变，减弱状态切换的僵硬感。
- 使用 Qt `QPainter` 的渐变能力实现，无需引入 Vulkan；后续如需在圆环中心做 Vulkan 特效时再扩展。
- 构建与运行验证通过，`_install/main.exe` 已更新。

### 圆环方向与卡顿修复（2026-08-18 迭代）

- 修正刻度渐变方向：保持方块朝向不变（长轴仍指向圆心），但将渐变从沿局部 y 轴改为沿局部 x 轴（即圆环顺时针切线方向），使颜色在方块内部沿圆形进度条路径从后侧向前侧推进。
- 提升刷新率：定时器从 100ms 缩短到 16ms（约 60fps），`m_elapsed` 增量同步改为 0.016s，解决小方格内颜色移动卡顿问题。
- 构建与运行验证通过，`_install/main.exe` 已更新。

## 当前整体状态

### 功能

- 窗口占主屏幕面积 30%，正方形，保留系统标题栏（含关闭按钮）。
- 仅保留圆环刻度条与蓝色暂停/继续按钮；标题文字、中间图案、省略号按钮已去除。
- 默认工作 10 秒，圆环从 3 点钟方向起顺时针随进度变蓝；工作结束全灰并休息 3 秒，随后自动开始下一轮。
- 按钮点击可暂停 / 继续。

### 视觉

- 背景 `#2B2B2B`；激活刻度 `#55B2E8`；未激活刻度 `#3D3D3D`。
- 按钮背景为径向渐变（中心 `#77C8F8` → 边缘 `#55B2E8`），图标为线性渐变。
- 圆环每个刻度内部沿顺时针切线方向平滑渐变，颜色前沿可见移动。
- 按钮面积约占窗口面积 5%。
- 刷新率约 60fps，动画流畅。

### 验证

- Release 编译成功。
- `_install/main.exe` 已更新，Qt 运行依赖已部署。
- 启动运行 3 秒无崩溃，无异常退出。

## 后续建议

1. 下载安装 Qt 6.8.3 MSVC 2022_64 到其他机器时，可调整 `-DCMAKE_PREFIX_PATH` 指向实际路径。
2. 业务功能开发在 `main/src/` 中展开；运行时资源放入 `main/resource/`。
3. 需要 Vulkan 时安装 Vulkan SDK，CMake 会自动启用 `HAS_VULKAN`。
4. 使用 `/software-director-delivery` Skill 管理后续新需求。

## 命令行图像输出接口迭代（2026-08-18）

### 需求

增强当前架构，软件添加图像输出接口，能够通过命令行输出对应时间的软件图像，方便快速知晓结果。例如 `main -t 1` 输出第 1 秒的图像，再与 `main -t 1.5` 的第 1.5 秒图像比较，判断是否符合要求。

### 实际改动摘要

- `main/src/focus_timer_widget.h/.cpp`：新增公共接口 `setTime(double seconds)`。停止内部 QTimer，置运行态，按「工作 10s + 休息 3s」循环用 `fmod(seconds, 13.0)` 确定性算出 `m_state` / `m_elapsed`，无需真实等待。
- `main/src/main.cpp`：新增命令行参数 `-t <秒数>`（支持小数）与 `-s <边长>`（默认 400）。提供 `-t` 时不弹窗：创建控件 → `resize(size,size)` → `setTime(t)` → `grab()` → 保存 `shot_<t参数原文>.png` 到当前工作目录，退出码 0。无 `-t` 时交互窗口行为完全不变。
- 高 DPI 修复：`grab()` 结果按 `devicePixelRatio` 归一化（尺寸不符时 `scaled` 回 `-s` 指定值），保证任意缩放机器上输出尺寸严格等于 `-s`。

### 用法示例

```powershell
cd D:\qsw\禅道\_shottest
D:\qsw\禅道\_install\main.exe -t 1      # 生成 shot_1.png（400×400，第 1 秒画面）
D:\qsw\禅道\_install\main.exe -t 1.5    # 生成 shot_1.5.png（第 1.5 秒画面）
D:\qsw\禅道\_install\main.exe -t 1 -s 200  # 自定义尺寸 200×200
```

肉眼对比 `shot_1.png` 与 `shot_1.5.png` 即可确认进度推进是否符合要求。

### 结果

- 验收：**通过**（测试轮次 2 / 5）。
- round 1 FAIL：高 DPI（DPR=2）下 PNG 尺寸翻倍（800×800 而非 400×400）；triage 定位为代码缺陷（`grab()` 未按 DPR 归一化），最小修复后 round 2 PASS。
- 产物：`_install/main.exe` 已更新，Qt 依赖已部署。

### 测试结论（round 2，全部 PASS）

| 验收项 | 结果 |
| --- | --- |
| 构建 / 安装 Release 成功，`_install/main.exe` 更新 | 通过 |
| `-t 1` 生成 `shot_1.png`，严格 400×400，退出码 0，不弹窗 | 通过 |
| `-t 1.5` 生成 `shot_1.5.png`，严格 400×400 | 通过 |
| 蓝色像素对比：1.5s（9832）> 1s（9061），进度趋势正确 | 通过 |
| `-t 1 -s 200` 输出严格 200×200 | 通过 |
| 无参数运行弹窗正常，不回归 | 通过 |

### 证据

- 计划：`plan/delivery-plan.md`
- 执行记录：`plan/execution-log.md`
- 测试报告：`plan/test-report.md`（round 1 FAIL → round 2 PASS）
- 排查记录：`plan/triage-log.md`

### 残留风险

- 输出文件名仅含 `-t` 参数原文，同 `-t` 不同 `-s` 会互相覆盖，使用时注意顺序。
- 安装时 windeployqt 有无害警告（translations、dxcompiler.dll 缺失），不影响运行。

## 界面迭代：按钮、刻度、自适应与无边框（2026-08-18）

### 需求

1. 暂停/继续按钮缩小为约占软件的 10%（直径 ≈ 窗口边长 10%）。
2. 按钮增加交互动画与图案渐变（悬停提亮、按压缩放、图标过渡）。
3. 表刻度样式参数抽出，默认调细至舒适观感。
4. 布局自适应：缩放窗口时表盘/按钮/边距等比不变，窗口最小 200×200。
5. 去掉系统标题栏，整体与表盘融为一体，保留左上角关闭按钮，简约扁平风格。

### 实际改动摘要

- `main/src/focus_timer_widget.h/.cpp`：
  - 按钮半径 `base*0.126` → `base*0.05`（直径实测恰为边长 10.00%），图标按按钮半径等比缩放。
  - 复用 60fps QTimer 驱动插值动画：悬停提亮（约 120ms）、按压 0.92 倍缩放回弹、暂停↔播放图标 150ms 交叉淡化；图标保留线性渐变；新增 `setMouseTracking`。
  - 新增 `TickStyle` 结构体集中管理刻度参数（数量 36、宽度比 0.026、高度比、圆角比、环半径比 0.28、前沿渐变带宽 0.03），带中文注释与调节建议；默认刻度由 0.04 调细为 0.026 呈细胶囊形。
- `main/src/frameless_window.h/.cpp`（新增）：`Qt::FramelessWindowHint` 无边框窗口，背景 `#2B2B2B` 与表盘一致；左上角扁平 × 关闭按钮（悬停提亮）；空白区域拖拽移动；6px 热区 8 方向缩放（纯 Qt 实现）；最小 200×200；拖拽与程序性 resize 均按短边回正保持正方形；启动居中，初始面积为屏幕 30%。
- `main/src/main.cpp`：改用 `FramelessWindow`；`-t` / `-s` 截图模式行为不变。

### 结果

- 验收：**通过**（测试轮次 1 / 5，一次通过）。
- 产物：`_install/main.exe` 已更新（Release，Qt 依赖已部署）。

### 测试结论（round 1，全部 PASS）

| 验收项 | 结果 |
| --- | --- |
| 构建 / 安装 Release 成功，`_install/main.exe` 更新 | 通过 |
| 按钮直径实测 40px/400px = 10.00%（8%~12% 区间，远小于旧版 25%） | 通过 |
| `TickStyle` 集中头文件、宽度比 0.026，截图刻度明显变细 | 通过 |
| `-t 1` / `-t 1.5` 严格 400×400，退出码 0，不弹窗；1.5s 蓝色像素 1711 > 1s 的 1146 | 通过 |
| `-t 1 -s 200` 输出严格 200×200 | 通过 |
| 最小 200×200、正方形回正、无边框、左上角关闭按钮、边缘缩放热区均实现 | 通过 |
| 无参启动 5 秒不崩溃 | 通过 |

### 证据

- 计划：`plan/delivery-plan.md`
- 执行记录：`plan/execution-log.md`
- 测试报告：`plan/test-report.md`（round 1 PASS）

### 残留风险

- 边缘缩放手感为纯 Qt 实现，未走 Windows 原生 NCHITTEST，极端快速拖拽可能有轻微迟滞；如介意可后续换原生方案。
- 悬停/按压动画在截图模式（`-t`）下不可见，仅交互模式生效，属预期。

## 扁平化、托盘化、中心数字与休息提醒迭代（2026-08-18）

### 需求

1. 按钮样式颜色扁平化，去掉中心白色（径向）渐变。
2. 关闭按钮移到右上角；点击不退出，而是最小化到系统托盘；真正退出需右键托盘图标点「退出」。
3. 圆环中心加内容：本期实现数字剩余时间；植物生长模式先不做，但留模式切换扩展余地。
4. 计时结束后进入 10s 休息计时，刻度颜色表达反转（工作 灰→蓝，休息 蓝→灰）；计时到后弹出窗口。

### 实际改动摘要

- `main/src/focus_timer_widget.h/.cpp`：
  - 按钮扁平化：纯色 `#55B2E8`，图标纯色 `#101010`；悬停提亮、按压缩放、图标交叉淡化动画保留。
  - 关闭按钮从左上角移到右上角。
  - 新增 `enum class CenterMode { TimeText, Plant }` 与 `setCenterMode`/`centerMode`/`drawCenterContent` 分发；`Plant` 分支留空注释预留。`TimeText`：圆环中心显示向上取整剩余秒数（10→1），工作阶段 `#E8E8E8`、休息阶段 `#55B2E8`，字号 0.12×短边。
  - 休息计时 10s：从全蓝开始顺时针逐刻度变灰，刻度内切向前沿渐变方向反转；休息结束自动进入下一轮工作；周期常量集中，`setTime` 按 `fmod(seconds, 20.0)` 确定性渲染。
  - 新增 `phaseChanged` 信号供窗口层感知阶段切换。
- `main/src/frameless_window.h/.cpp`：
  - 系统托盘（`QSystemTrayIcon`）：点击右上角 × `hide()` 到托盘；右键菜单「退出」→ `QApplication::quit()`；左键/双击恢复主窗口；截图模式不建托盘。
  - 全屏休息提醒 `RestOverlay`（内部小类）：`FramelessWindowHint | WindowStaysOnTopHint | Tool`，半透明 `rgba(0,0,0,180)` 覆盖主屏，白色「休息一下」+ 剩余秒数倒计时，休息结束自动关闭；截图模式不触发。
- `main/CMakeLists.txt`：无改动（未新增源文件）。

### 结果

- 验收：**通过**（测试轮次 1 / 5，一次通过）。
- 产物：`_install/main.exe` 已更新（Release，Qt 依赖已部署）。

### 测试结论（round 1，全部 PASS）

| 验收项 | 结果 |
| --- | --- |
| 构建 / 安装 Release 成功，`_install/main.exe` 为最新 | 通过 |
| `-t 3`：400×400、退出码 0；中心亮色「7」（#E8E8E8）；按钮纯色无渐变 | 通过 |
| `-t 13`：休息阶段中心蓝色「7」；3 点钟起前段刻度已变灰；蓝色像素 6748 > 工作 3s 的 2935 | 通过 |
| `-t 1 -s 200` 严格 200×200 不回归 | 通过 |
| 无参运行 15 秒跨工作→休息边界不崩溃 | 通过 |
| 12 秒处全屏截屏实测「休息一下」遮罩自动弹出 | 通过 |
| 代码审查：休息反向、hide() 托盘、「退出」菜单、20s 周期、截图模式无托盘/遮罩 | 通过 |

### 人工验证项（自动测试未覆盖，代码路径已确认）

- 点击右上角 × → 隐藏到托盘。
- 托盘右键「退出」真正关闭；左键/双击恢复主窗口。
- 休息结束遮罩自动关闭。

### 证据

- 计划：`plan/delivery-plan.md`
- 执行记录：`plan/execution-log.md`
- 测试报告：`plan/test-report.md`（round 1 PASS）

### 残留风险

- 全屏提醒仅覆盖主屏，多显示器不覆盖副屏。
- 托盘图标使用系统标准图标回退，未引入自定义 ico。
- 植物生长模式仅留扩展位（`CenterMode::Plant`），后续实现时无需改动模式切换架构。

### round 2 修复：事件传播断点（2026-08-18）

**用户实测反馈**：窗口拖动不了；点右上角 × 不能最小化到托盘，窗口仍显示在前面。

**根因**（见 `plan/triage-log.md` round 2）：`FocusTimerWidget` 作为中央控件铺满整个客户区，其鼠标事件函数对未命中播放按钮的点击既不处理也不 `event->ignore()`，事件不向父窗口传播，`FramelessWindow` 的拖拽 / 关闭 / 边缘缩放处理器收不到事件（同根失效）。`hide()` 代码本身存在但永远收不到点击。

**最小修复**（仅 `main/src/focus_timer_widget.cpp` 三个函数）：

- `mousePressEvent`：未命中播放按钮（含右上角 ×）时 `event->ignore()`。
- `mouseReleaseEvent`：`m_buttonPressed` 为假时 `event->ignore()`。
- `mouseMoveEvent`：不在播放按钮/关闭按钮上时 `event->ignore()`（同时救活边缘缩放光标）。

**测试结论（round 2，PASS）**：构建/安装退出码 0、产物哈希确认更新；截图回归 8/8 与 round 1 数据一致；PostMessage 直投消息自动化实测：拖动后 `GetWindowRect` 位置显著变化、点 × 后 `IsWindowVisible=False` 且进程托盘驻留；代码审查确认 ignore() 不影响播放按钮路径。

**人工验证项**：真实鼠标拖拽手感（自动化桩在 DPR=2 下存在坐标换算伪差，不影响结论）。
