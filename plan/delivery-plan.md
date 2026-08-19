# 交付计划

## 需求原文

1. 继续暂停按钮下移一定距离，目前贴着圆盘不好看。同时其中的图案改成白色。
2. 暂停继续按钮旁边增加 ... 按钮。点击后，一个下拉框，里面包含功能按钮
   1) 设置，点击后，弹出窗口，设置专注时间与休息时间。UI风格沿用当前。弹出窗口无边框，右上角关闭按钮，中间偏下是确定按钮。

## 需求对齐结论（用户已确认）

- 时长最小单位为秒，设置窗口中以 HH:MM:SS 形式显示与输入（`QTimeEdit`，`displayFormat "HH:mm:ss"`）。
- 圆环中心剩余时间显示同步改为 HH:MM:SS 格式（秒以下不显示），避免长时长时显示超大整数。
- 设置持久化：使用 `QSettings` 保存，启动时（仅交互模式）自动加载。
- 点「确定」应用规则：对当前阶段，若新时长 > 已经过时长，则继续当前阶段（剩余 = 新时长 - 已用）；若已经过时长 ≥ 新时长（时间缩短），直接结束当前阶段进入下一阶段。
- 「...」下拉框用 `QMenu` + 深色样式表实现（背景沿用 `#2B2B2B` 系，选中高亮 `#55B2E8` 系）。
- 截图模式（`-t`）不加载持久化设置，始终用内置默认 10s/10s，保证回归确定性。

## 工作区

- 根目录：`D:\qsw\禅道`
- 源码目录：`main/src/`
- 构建目录：`_build/`
- 安装目录：`_install/`（产物 `_install/main.exe`）

## 范围

- 做：
  1. 播放/暂停按钮下移：中心 y 从 `h*0.78` 下移至约 `h*0.86`（`buttonRect` / `drawButton` 同步），拉开与圆环底边（约 `0.72h`）的间距；具体比例以截图验证为准，若 0.86 视觉仍不理想可微调并记录。
  2. 按钮图标改白色：`drawPauseIcon` / `drawPlayIcon` 的 `#101010` → `#FFFFFF`。
  3. 「...」按钮：与播放按钮同一水平线、右侧排列（如 x ≈ `w*0.5 + base*0.11`，半径约 `base*0.04`，可微调），自绘三个白色横向圆点，悬停提亮动画复用现有插值机制；命中检测 + 未命中 `ignore()` 保持父窗口拖拽/缩放不回归。
  4. 点击「...」弹出 `QMenu`（深色样式表），含功能项「设置」；菜单弹出在按钮下方。
  5. 设置弹窗（新文件 `main/src/settings_dialog.h/.cpp`）：`QDialog` + `Qt::FramelessWindowHint`，背景 `#2B2B2B`；右上角自绘 × 关闭按钮（取消不保存）；两行「专注时间」「休息时间」各一个 `QTimeEdit`（HH:mm:ss，深色样式）；中间偏下「确定」按钮（扁平 `#55B2E8` 底、白字）。支持标题区拖拽移动（与主窗口风格一致，工作量小则做，否则记录为遗留）。
  6. 时长应用：`FocusTimerWidget` 新增 `setDurations(double workSec, double restSec)`，按对齐结论的规则应用（当前阶段续跑或立即进入下一阶段，发 `phaseChanged` 的规则与现有代码一致；截图模式不触发该信号）。非当前阶段的时长直接更新存储值。
  7. 持久化：`QSettings("Chandao", "FocusTimer")` 存 `workSeconds` / `restSeconds`；`main.cpp` 交互模式启动时加载并调用 `setDurations`；截图模式跳过加载。
  8. 中心时间显示：`drawCenterTimeText` 改为 HH:MM:SS 格式（如 `00:00:07`；不足 1 小时时可仍显示 HH:MM:SS 全格式保持统一），字号按需微调保证圆环内放得下。
- 不做：
  - 植物生长模式、Vulkan 特效。
  - 下拉框中除「设置」外的其他功能项（后续迭代再加）。
  - git commit / push（除非用户另行要求）。

## 技术栈与约束

- C++20 + Qt6 Widgets + CMake（Visual Studio 18 2026，x64）。
- Qt 路径：`-DCMAKE_PREFIX_PATH="D:\Qt\6.8.3\msvc2022_64"`。
- 构建：`cmake --build _build --config Release`；安装：`cmake --install _build --config Release`。
- 新增源文件必须加入 `main/CMakeLists.txt` 的 `main` 目标。
- 代码标识符、目录名、目标名保持项目原样。

## 任务拆分

1. 按钮下移 + 图标白色 | `main/src/focus_timer_widget.cpp` | 按钮中心 y ≈ 0.86h；图标纯白。
2. 「...」按钮绘制与交互 | `main/src/focus_timer_widget.h/.cpp` | 圆点按钮可见、悬停提亮、点击发信号（如 `dotsClicked`）；未命中事件 `ignore()` 不回归。
3. 下拉菜单 | `main/src/focus_timer_widget.cpp` 或 `frameless_window`（执行 Agent 自决归属） | 点击「...」弹出深色 `QMenu`，含「设置」项。
4. 设置弹窗 | 新增 `main/src/settings_dialog.h/.cpp`，改 `main/CMakeLists.txt` | 无边框、右上角 ×、两个 HH:mm:ss 输入、中间偏下「确定」；× 取消、确定发结果。
5. 时长应用 + 持久化 | `main/src/focus_timer_widget.h/.cpp`、`main/src/main.cpp`、`main/src/frameless_window.h/.cpp` | 确定后按规则应用；QSettings 存取；交互模式启动加载；截图模式不加载。
6. 中心时间 HH:MM:SS | `main/src/focus_timer_widget.cpp` | 工作与休息阶段均显示 HH:MM:SS。

## 验收标准

- 默认：Release 编译成功，`_install/main.exe` 更新。
- 截图回归：`-t 1` / `-t 1.5` / `-t 13` 仍输出严格 400×400、退出码 0（默认时长不变，进度语义不变）。
- 截图验证新 UI：`-t 3` 截图中：按钮中心 y 明显下移（实测圆心 y/边长 ≥ 0.83）；按钮图标区域为白色（像素检测）；「...」按钮三个白点可见；中心时间显示 HH:MM:SS 格式（截图人工/脚本核验）。
- 设置应用逻辑：代码审查确认「新时长 > 已用 → 续跑；已用 ≥ 新时长 → 进下一阶段」分支正确。
- 无参启动 5 秒不崩溃；窗口拖拽 / 右上角 × 隐藏托盘 / 边缘缩放不回归（已有自动化 PostMessage 方式可复用）。
- 人工验证项（自动测试覆盖不到的交互）：点「...」出下拉 → 点「设置」出弹窗 → 修改时长 → 确定生效；× 关闭弹窗不保存；重启程序时长保持。

## 风险与未知

- HH:MM:SS 文本变长，中心字号需适配；截图对比基线（蓝色像素数）可能因中心文字变化而变动，测试时以同版本两次截图对比为准。
- QMenu 在无边框自绘窗口中的弹出定位需注意屏幕边界（QMenu 自带处理，风险低）。
- 持久化设置若被用户改大，旧的 `-t` 截图断言脚本不受影响（截图模式不加载设置）。

## 派工策略

- 执行 Agent：delivery-implementer（回退 generalPurpose）
- 测试 Agent：delivery-tester（回退 shell）
- 排查 Agent：delivery-triage（回退 explore + generalPurpose）
- 最大测试-修复轮次：5

## 对齐状态

- [x] 用户已确认（关键决策经问答拍板），可以执行
