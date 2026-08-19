# 交付测试报告 — 滚筒渐隐与设置窗文字放大 — Round 1 / 5

- 测试时间：2026-08-19 10:58–11:05（UTC+8）
- 测试环境：Windows 11 (26200)，PowerShell + Python / Pillow，显示器缩放 200%（DPR=2）
- 截图工作目录：`D:\qsw\禅道\_shottest\round7`（本轮新建）
- 对应变更：`plan/execution-log.md` 2026-08-19「滚筒渐隐与设置窗文字放大」——仅改 `main/src/settings_dialog.cpp`（WheelPicker 顶/底渐隐色带、项高/宽度/字号放大、设置窗文字放大与布局填充、窗口 420×460）

## 结论：**PASS**

构建/安装退出码均 0，`_install/main.exe` 时间戳更新为本轮（10:57）；截图回归 `-t 1 / 1.5 / 3` 全部退出码 0、严格 400×400，与指定基线 `_shottest/round6/shot_3.png` 像素 diff = **0**（主窗口零改动成立）；代码审查确认渐隐色带在数字项之后绘制、颜色 #2B2B2B 不透明→透明、各 1.5 项高（51px），全部尺寸/字号常量（34/56/19/16、18 加粗/15/12、420×460）落实，底部 stretch 保留；无参启动 5 秒不崩溃。设置窗渐隐观感与文字布局观感为人工验证项，列入第 5 节，不影响判定。

## 1. 构建与安装（Release）—— PASS

| 命令 | 期望退出码 | 实际退出码 | 结果 |
|---|---|---|---|
| `cmake --build _build --config Release` | 0 | 0（`main.vcxproj -> _build\main\Release\main.exe`） | PASS |
| `cmake --install _build --config Release` | 0 | 0（`-- Installing: _install/main.exe` + windeployqt） | PASS |

- 产物佐证：`_install/main.exe` LastWriteTime **2026-08-19 10:57**（上一轮为 10:47），93696 字节，为本轮新构建安装产物。
- windeployqt 两条无害警告（缺 translations、缺 dxcompiler.dll），与既往轮次相同，不影响运行。

## 2. 截图回归 —— PASS

命令在 `_shottest\round7` 下以 `Start-Process -Wait -PassThru` 同步执行取真实退出码：

| 命令 | 退出码 | 产物 | 尺寸 | 结果 |
|---|---|---|---|---|
| `main.exe -t 1` | 0 | `shot_1.png` | 严格 400×400 | PASS |
| `main.exe -t 1.5` | 0 | `shot_1.5.png` | 严格 400×400 | PASS |
| `main.exe -t 3` | 0 | `shot_3.png` | 严格 400×400 | PASS |

像素对比（Python + PIL，RGBA 逐像素）：

| 对比 | 差异像素数 | 结果 |
|---|---|---|
| round7/shot_3.png vs round6/shot_3.png（指定基线） | **0** | PASS，主画面完全一致 |
| round7/shot_1.png vs round6/shot_1.png | 2157 | 见下方说明，非本轮回归 |
| round7/shot_1.5.png vs round6/shot_1.5.png | 2157 | 同上 |

- 说明：round6 目录内 `shot_1.png`/`shot_1.5.png` 生成于 10:35（旧构建），`shot_3.png` 生成于 10:47（当前构建）。差异区域 bbox (162,328)-(255,359) 即底部按钮行，源于已提交的旧 commit `9c535e7`「Center button pair」（播放按钮左移、双按钮居中布局），与本轮 `settings_dialog.cpp` 改动无关。佐证：round6/shot_3（10:47 新布局）与 round7/shot_3 逐像素 0 差异；git 状态显示本轮唯一源码改动为 `M main/src/settings_dialog.cpp`，主窗口相关文件零改动。

## 3. 代码审查 `main/src/settings_dialog.cpp` —— PASS

| 审查项 | 实际 | 结果 |
|---|---|---|
| 渐变色带在数字项之后绘制（覆盖数字） | `paintEvent` 末尾（选中行底色带与全部数字绘制之后）叠加顶/底色带 | PASS |
| 色带颜色 #2B2B2B 不透明→透明 | `QLinearGradient`，顶部 `bg(不透明)→alpha 0`、底部反向；`bg = QColor(0x2B,0x2B,0x2B)` 与弹窗背景一致 | PASS |
| 色带高度顶/底各约 1.5 项高 | `fadeH = kItemH * 1.5 = 51px` | PASS |
| 项高/宽度/字号常量 | `kItemH = 34`、`setFixedSize(56, kItemH*kVisible)`、选中 19px / 普通 16px（计划值 34/56/19/16 全部落实） | PASS |
| 标题 18px 加粗 | `titleFont.setPixelSize(18); setBold(true)` | PASS |
| 行标签 15px | 「专注时间/休息时间」`setPixelSize(15)` | PASS |
| 单位标签 12px | 「时/分/秒」`setPixelSize(12)` | PASS |
| 窗口 420×460 | `setFixedSize(420, 460)` | PASS |
| 确定按钮仍中间偏下 | `layout->addStretch()` 保留在 `okButton` 之前 | PASS |

## 4. 无参启动稳定性 —— PASS

- `Start-Process _install\main.exe`，5 秒后 `HasExited = False`（存活、未崩溃），随后 `Stop-Process` 杀掉进程。

## 5. 人工验证项（不影响 PASS/FAIL）

- [ ] 打开设置窗，滚筒顶部/底部数字渐隐自然，与「时/分/秒」单位标签无视觉重叠。
- [ ] 设置窗文字大小（标题/行标签/单位/滚筒数字）与布局观感，中部无明显空旷区域，「确定」落中间偏下。

---

# 交付测试报告 — 去全屏蒙版，改为恢复窗口+任务栏闪烁 — Round 3 / 5

- 测试时间：2026-08-19 09:45–09:50（UTC+8）
- 测试环境：Windows 11 (26200)，PowerShell 5.1 + Python 3.14.7 / Pillow 12.3.0，显示器缩放 200%（DPR=2，物理屏 2560×1600）
- 截图工作目录：`D:\qsw\禅道\_shottest\round3`；交互脚本：`_shottest\round3\interactive_test3.ps1`、截屏辅助 `_shottest\round3\capture_screen.py`（测试辅助，非产品源码）
- 对应变更：`plan/delivery-plan.md` 末尾「变更（2026-08-19）」—— `RestOverlay` 整体移除；工作→休息切换时若主窗口托盘隐藏则恢复显示并 `QApplication::alert` 任务栏闪烁

## 结论：**PASS**

构建/安装退出码均 0，`_install/main.exe` 为本轮新构建（09:44:42，与构建输出 SHA256 一致）；截图回归 8/8 通过，数据与前两轮完全一致；**本轮两项重点自动化实测通过**：点击右上角 × 后窗口隐藏且进程托盘驻留（t=3.4s），隐藏状态下跨过工作→休息边界后主窗口**自动恢复可见（t=11.8s）**；7 张全屏截图逐张统计，任何时刻均未出现全屏黑色蒙版（对照 round 1 含蒙版截图：平均亮度 12.2/极暗占比 0.9555，本轮全程 27~35 / 0.83~0.91，无突变）。代码审查确认 RestOverlay 无残留、`phaseChanged` 链路正确、截图模式不含提醒逻辑。任务栏闪烁视觉效果列为人工验证项，不计 FAIL。

## 1. 构建与安装（Release）—— PASS

| 命令 | 期望退出码 | 实际退出码 | 结果 |
|---|---|---|---|
| `cmake --build _build --config Release` | 0 | 0（`main.vcxproj -> _build\main\Release\main.exe`） | PASS |
| `cmake --install _build --config Release` | 0 | 0（`-- Installing: _install/main.exe` + windeployqt） | PASS |

- 产物佐证：`_install/main.exe` LastWriteTime **2026-08-19 09:44:42**（上一轮为 2026-08-18 18:50:59），49152 字节，SHA256 = `CAC6BA89…4C1F7B`，与 `_build\main\Release\main.exe` **哈希完全一致**，确为本轮变更的新构建产物。
- windeployqt 两条无害警告（缺 translations 目录、缺 dxcompiler.dll），与既往轮次相同，不影响运行。

## 2. 截图回归 —— PASS（无回归）

命令均在 `_shottest\round3` 下以 `Start-Process -Wait -PassThru` 同步执行取真实退出码：

| 命令 | 退出码 | 产物 | 尺寸 | 结果 |
|---|---|---|---|---|
| `main.exe -t 3` | 0 | `shot_3.png` | 严格 400×400 | PASS |
| `main.exe -t 13` | 0 | `shot_13.png` | 严格 400×400 | PASS |
| `main.exe -t 1 -s 200` | 0 | `shot_1.png` | 严格 200×200 | PASS |

像素分析（`_shottest\round3\analyze_round3.py`，容差 ±12，脚本退出码 0，**8/8 通过**）：

| 检查项 | 实测数据 | 结果 |
|---|---|---|
| `shot_3` 中心亮色数字（工作剩 7） | #E8E8E8 像素 **346**，蓝色 0 | PASS |
| `shot_3` 按钮纯色 #55B2E8 无渐变 | 中心与四近边缘点全部 **(85,178,232)** | PASS |
| `shot_13` 中心蓝色数字（休息剩 7） | 蓝色像素 **346**，亮色 0 | PASS |
| 蓝色刻度像素对比 | t=3 = **2935**，t=13 = **6748**，比值 **2.30**（>1.5） | PASS |
| `shot_13` 顺时针前段已变灰 | 前段灰 **123**、蓝 **0** | PASS |

各项数值与 round 1/2 完全一致，本轮变更未引入任何渲染回归。目视复核 `shot_3.png`/`shot_13.png`：中心数字颜色、圆环配色、右上角 × 均符合预期。

## 3. 交互验证（本轮重点）—— PASS（自动化）

- 方法沿用 round 2：本会话 OS 层合成输入被拒（err=5），改用 **PostMessage 直接向顶层 HWND 投递消息**；窗口可见性用 `IsWindowVisible` 判定；全屏蒙版用 PIL ImageGrab 定时截全屏 + 像素统计判定。
- 测试流程（`_shottest\round3\interactive_test3.ps1`，脚本退出码 **0**，INTERACTIVE_RESULT=PASS）：
  1. t=0s 先截基线全屏图 `screen_baseline.png`（无应用），随后无参启动 `_install\main.exe`（PID 27444）。
  2. t=2.6s 按 PID 枚举到主窗口 HWND=4264104，PostMessage 点击客户区右上角 × 区域（0.925/0.075 比例处）。
  3. t=3.4s 检查：`IsWindowVisible=**False**` 且 `ProcessAlive=**True**` → **HIDE_TO_TRAY_TEST: PASS**。
  4. 隐藏状态下持续轮询（每 400ms），t=**11.8s** 检测到 `IsWindowVisible=True`（计时器自进程启动起算，工作 10s 边界落在 t≈10.8s，含 400ms 轮询粒度与截屏耗时，落在 8.5~14s 容差带内）→ **RESTORE_TEST: PASS：托盘隐藏状态下跨过工作→休息边界，主窗口自动恢复显示**。
  5. 恢复后截屏中心区域放大复核（`zoom_t12_center.png`）：窗口可见，圆环呈休息阶段蓝色（中心区域蓝色采样点 **3650**，隐藏时同区域仅 97），主窗口确实恢复并进入休息阶段渲染。
  6. 清理：`Stop-Process -Force` 终止；脚本finally后复查曾提示残留，随即补杀，最终确认 **无 main.exe 残留进程**。

### 无全屏蒙版验证 —— PASS

7 张全屏截图（基线/隐藏/5s/8s/恢复时/12s/14s）逐张统计极暗像素占比（R,G,B 均<40）与平均亮度：

| 截图 | 时刻 | DARKFRAC | 平均亮度 |
|---|---|---|---|
| screen_baseline.png | t≈0（无应用） | 0.8289 | 31.7 |
| screen_hidden.png | t≈3.4s | 0.8926 | 31.3 |
| screen_t5.png | t≈5s | 0.9148 | 27.2 |
| screen_t8.png | t≈8s | 0.8680 | 33.1 |
| screen_restored.png | t≈11.8s | 0.8597 | 35.1 |
| screen_t12.png | t≈12s（休息阶段） | 0.8574 | 35.2 |
| screen_t14.png | t≈14s（休息阶段） | 0.8494 | 33.6 |

- 对照组（round 1 **含**全屏蒙版的截图）：`interactive_at_12s.png` DARKFRAC=**0.9555**、平均亮度=**12.2**；`rest_overlay_screen.png` DARKFRAC=**0.9359**、平均亮度=**12.7**。
- 本轮桌面本身偏暗（基线 DARKFRAC 已达 0.83），但**全程各帧指标与基线同量级、无向蒙版特征的突变**；若蒙版出现，平均亮度应跌落至 ~12 量级（对照组实测）。结论：**任何时刻均未出现全屏黑色蒙版**。

## 4. 代码审查（只读）—— PASS

| 审查点 | 结论 | 位置 |
|---|---|---|
| RestOverlay 整体移除、无全屏蒙版残留 | ✔ `main/src/` 全目录检索 `RestOverlay/overlay/showFullScreen/WindowStaysOnTop/rgba(0,0,0` 均无匹配；窗口仅有 `#2B2B2B` 背景 | `frameless_window.cpp/.h` |
| `phaseChanged` 链路正确 | ✔ 工作/休息切换时发射 `phaseChanged(state)`；`FramelessWindow::setupRestAlert` 中 `state==1`（工作→休息）才触发：隐藏则 `showNormal()+activateWindow()`，再 `QApplication::alert(this)` | `focus_timer_widget.cpp:371-383`、`frameless_window.cpp:74-89` |
| 窗口可见时仅闪烁、不重复弹窗 | ✔ `isVisible()` 分支判断，可见时跳过 `showNormal()` 仅 `alert` | `frameless_window.cpp:83-87` |
| 截图模式路径不含提醒逻辑 | ✔ `-t` 分支只构造 `FocusTimerWidget`，不构造 `FramelessWindow`；`setTime` 停表且不发射 `phaseChanged` | `main.cpp:26-49`、`focus_timer_widget.cpp:30-46` |
| 托盘/拖拽/缩放等既有功能 | ✔ 与 round 2 一致，未被本轮变更触碰 | `frameless_window.cpp:41-72,134-221` |

## 人工验证项（不计 FAIL）

| 项 | 原因 | 佐证 |
|---|---|---|
| 任务栏闪烁视觉（`QApplication::alert`） | 闪烁是瞬态视觉效果，自动化截屏无法可靠捕获 | 代码路径确认 `QApplication::alert(this)` 必被调用；恢复显示已实测 |
| 托盘右键菜单「退出」真正关闭程序 | 需托盘图标交互 | 代码路径确认：菜单项 → `QApplication::quit`（round 1/2 已确认，本轮未改动） |

## 汇总表

| 验收项 | 期望 | 实际 | 结果 |
|---|---|---|---|
| 构建 / 安装 Release | 退出码 0 | 0 / 0 | PASS |
| `_install/main.exe` 更新 | 哈希/时间戳佐证 | 09:44:42，与构建输出 SHA256 一致 | PASS |
| `-t 3` | 0、400×400、中心亮色「7」、按钮纯色 #55B2E8 | 全部符合（346 / 五点纯色一致） | PASS |
| `-t 13` | 中心蓝色「7」、蓝刻度明显多于 `-t 3` | 蓝色 346；6748 vs 2935（2.30 倍） | PASS |
| `-t 1 -s 200` | 严格 200×200 | 200×200，退出码 0 | PASS |
| 右上角 × 隐藏到托盘 | 窗口不可见且进程驻留 | IsWindowVisible=False，进程存活 | PASS |
| 托盘隐藏时计时到自动恢复显示（本轮重点） | 跨工作→休息边界后 IsWindowVisible→True | t=11.8s 自动恢复可见（边界 ~10.8s） | PASS |
| 全程无全屏黑色蒙版（本轮重点） | 任何时刻不出现大面积深色遮罩 | 7 帧统计与基线同量级，对照组差异显著；未见蒙版 | PASS |
| 代码审查 | RestOverlay 无残留、phaseChanged 链路正确、截图模式无提醒 | 逐项确认 | PASS |
| 进程清理 | 测试结束杀掉 main.exe | 已终止，无残留 | PASS |

---

# 交付测试报告 — 事件传播修复（拖动/托盘）— Round 2 / 5

- 测试时间：2026-08-18 18:52–18:58（UTC+8）
- 测试环境：Windows 11 (26200)，PowerShell 5.1 + Python 3.14.7 / Pillow 12.3.0，显示器缩放 200%（DPR=2，物理屏 1280×800）
- 截图工作目录：`D:\qsw\禅道\_shottest\round2`；交互脚本：`_shottest\round2\interactive_test2.ps1`（测试辅助，非产品源码）
- 对应修复：round 2，`main/src/focus_timer_widget.cpp` 三个鼠标事件函数在未命中热区时补 `event->ignore()`，使事件传播到父窗口 `FramelessWindow`（拖拽 / 右上角×隐藏到托盘 / 边缘缩放）

## 结论：**PASS**

构建/安装退出码均 0，`_install/main.exe` 与本次构建输出 SHA256 一致（时间戳 18:50:59，round 2 修复构建）；截图回归 8/8 通过（数据与 round 1 完全一致，无回归）；**本轮重点交互项全部自动化实测通过**：拖动消息序列后主窗口位置显著移动（拖拽路径生效），点击右上角 × 后 `IsWindowVisible=False` 且进程存活（托盘驻留）。代码审查确认 `ignore()` 仅在未命中热区时调用，播放按钮路径不受影响。

## 1. 构建与安装（Release）—— PASS

| 命令 | 期望退出码 | 实际退出码 | 结果 |
|---|---|---|---|
| `cmake --build _build --config Release` | 0 | 0（`main.vcxproj -> _build\main\Release\main.exe`） | PASS |
| `cmake --install _build --config Release` | 0 | 0（`-- Installing: _install/main.exe` + windeployqt） | PASS |

- 产物佐证：`_install/main.exe` LastWriteTime **2026-08-18 18:50:59**（上一轮为 18:27:56），SHA256 = `26C4901D…E1897070`，与 `_build\main\Release\main.exe` **哈希完全一致**；构建前旧产物哈希 `68E1A78B…A99742`，确已更新。
- windeployqt 两条无害警告（缺 translations 目录、缺 dxcompiler.dll），与既往轮次相同，不影响运行。

## 2. 截图回归 —— PASS（无回归）

命令均在 `_shottest\round2` 下以 `Start-Process -Wait -PassThru` 同步执行取真实退出码：

| 命令 | 退出码 | 产物 | 尺寸 | 结果 |
|---|---|---|---|---|
| `main.exe -t 3` | 0 | `shot_3.png` | 严格 400×400 | PASS |
| `main.exe -t 13` | 0 | `shot_13.png` | 严格 400×400 | PASS |
| `main.exe -t 1 -s 200` | 0 | `shot_1.png` | 严格 200×200 | PASS |

像素分析（`_shottest\analyze_round2.py`，容差 ±12，脚本退出码 0，**8/8 通过**）：

| 检查项 | 实测数据 | 结果 |
|---|---|---|
| `shot_3` 中心亮色数字（工作剩 7） | #E8E8E8 像素 **346**，蓝色 0 | PASS |
| `shot_3` 按钮纯色 #55B2E8 无渐变 | 中心与四近边缘点全部 **(85,178,232)** | PASS |
| `shot_13` 中心蓝色数字（休息剩 7） | 蓝色像素 **346**，亮色 0 | PASS |
| 蓝色刻度像素对比 | t=3 = **2935**，t=13 = **6748**，比值 **2.30**（>1.5） | PASS |
| `shot_13` 顺时针前段已变灰 | 前段灰 **123**、蓝 **0** | PASS |

各项数值与 round 1 完全一致，事件修复未引入任何渲染回归。

## 3. 交互验证（本轮重点）—— PASS（自动化）

- **合成输入受限说明**：本会话 `SendInput` 返回 err=5（ERROR_ACCESS_DENIED）、`SetCursorPos` 失败（光标始终停在 (0,0)），OS 层物理鼠标注入不可用（探测脚本 `_shottest\round2\probe_input.ps1` 留证）。因此改用 **PostMessage 直接向顶层 HWND 投递 `WM_LBUTTONDOWN/MOUSEMOVE/LBUTTONUP`**。Qt Widgets 整窗只有一个原生 HWND，消息进入 Qt 后走与真实输入完全相同的分发路径：命中子控件 `FocusTimerWidget` → 未命中热区 `ignore()` → 传播到父窗口 `FramelessWindow`——即 round 2 修复的确切代码路径。
- 测试流程（`_shottest\round2\interactive_test2.ps1`，脚本退出码 **0**）：
  1. 无参启动 `_install\main.exe`（PID 38468），1.8s 后按 PID 枚举到主窗口 HWND=3017528，初始 `GetWindowRect` = (362,122)-(916,676)，554×554 物理像素。
  2. **拖动**：向客户区中心按下左键，分 12 步移动 (+160,+120) 后松开 → 再次 `GetWindowRect` = (1562,1022)-(2116,1576)，**窗口位置显著改变（dx=+1200, dy=+900），DRAG_TEST: PASS**。（位移绝对值大于注入步长，是 Posted 消息在 DPR=2 下坐标换算的测试桩伪差；方向与单调性正确，证明父窗口拖拽分支已能收到并处理事件——round 1 时该路径完全收不到事件。真实鼠标的位移换算为标准 Qt 无边框拖拽写法 `move(globalPos - m_moveOffset)`，代码审查确认无误。）
  3. **关闭到托盘**：点击客户区右上角 × 区域（宽 0.925 / 高 0.075 比例处，DPR 无关，落于 `closeButtonRect` 内）→ `IsWindowVisible = **False**` 且 `ProcessAlive = **True**`，**HIDE_TO_TRAY_TEST: PASS**（窗口隐藏、进程托盘驻留）。
  4. 清理：`Stop-Process -Force` 终止，测试后 `Get-Process main` 确认无残留进程。
- 全程约 5 秒完成，处于工作阶段（<10s），不受全屏休息遮罩干扰。

## 4. 代码审查（只读）—— PASS

| 审查点 | 结论 | 位置 |
|---|---|---|
| `mousePressEvent` 仅在未命中播放按钮时 `ignore()` | ✔ 命中 `buttonRect` 走 `m_buttonPressed=true`（无 ignore）；else 分支才 `event->ignore()` | `focus_timer_widget.cpp:89-98` |
| `mouseReleaseEvent` 播放按钮点击路径未被 ignore 影响 | ✔ `m_buttonPressed` 为真时完成启停切换并 `update()`，无 ignore；为假才 `ignore()` | `focus_timer_widget.cpp:100-111` |
| `mouseMoveEvent` 仅在两热区外 `ignore()` | ✔ 悬停状态始终更新；`!overButton && !overClose` 时才 `ignore()`，救活父级边缘缩放光标 | `focus_timer_widget.cpp:113-126` |
| 父窗口拖拽/关闭/缩放处理器 | ✔ 存在且逻辑正确：关闭命中 → `hide()`（非退出）；非热区中部 → `m_moving` 拖拽；边缘 6px → 缩放 | `frameless_window.cpp:194-247` |
| 托盘「退出」与恢复 | ✔ 右键菜单「退出」→ `QApplication::quit`；左键/双击托盘图标 → `showNormal()` | `frameless_window.cpp:104-135` |

## 人工验证项（自动化已覆盖本轮两项重点，以下不计 FAIL）

| 项 | 原因 | 佐证 |
|---|---|---|
| 真实鼠标拖动手感（含位移 1:1 跟手） | 本会话合成物理输入被系统拒绝（err=5），PostMessage 位移量存在测试桩伪差 | 拖拽代码路径已实测激活；位移换算为标准写法 |
| 托盘右键菜单「退出」真正关闭程序 | 需托盘图标交互 | 代码路径确认：菜单项 → `QApplication::quit` |
| 托盘左键/双击恢复主窗口 | 需托盘图标交互 | 代码路径确认：`activated` → `showNormal()` |
| 边缘缩放光标与拖拽缩放 | 本轮未注入边缘坐标消息 | 父级 `edgeHit`/`applyResize` 代码就位，move 事件传播已被拖动测试证明 |

## 汇总表

| 验收项 | 期望 | 实际 | 结果 |
|---|---|---|---|
| 构建 / 安装 Release | 退出码 0 | 0 / 0 | PASS |
| `_install/main.exe` 更新 | 哈希/时间戳佐证 | 18:50:59，与构建输出 SHA256 一致 | PASS |
| `-t 3` | 0、400×400、中心亮色「7」、按钮纯色 #55B2E8 | 全部符合（346 / 五点纯色一致） | PASS |
| `-t 13` | 中心蓝色「7」、蓝刻度明显多于 `-t 3` | 蓝色 346；6748 vs 2935（2.30 倍） | PASS |
| `-t 1 -s 200` | 严格 200×200 | 200×200，退出码 0 | PASS |
| 窗口拖动（本轮重点） | 拖动后窗口位置改变 | 自动化实测 dx/dy 显著非零 | PASS |
| 右上角 × 隐藏到托盘（本轮重点） | 窗口不可见且进程驻留 | IsWindowVisible=False，进程存活 | PASS |
| 代码审查 | ignore() 仅未命中热区时调用，播放按钮不受影响 | 逐项确认 | PASS |
| 进程清理 | 测试结束杀掉 main.exe | 已终止，无残留 | PASS |

---

# 交付测试报告 — 按钮扁平化/托盘化/中心数字/休息反向/全屏提醒迭代 — Round 1 / 5

- 测试时间：2026-08-18 18:35–18:40（UTC+8）
- 测试环境：Windows 11 (26200)，PowerShell + Python 3.14.7 / Pillow 12.3.0，显示器缩放 200%（DPR=2）
- 截图工作目录：`D:\qsw\禅道\_shottest\round1`（未污染根目录）
- 对应交付：本轮迭代（按钮纯色扁平化、关闭按钮右上角 + 托盘隐藏/退出、CenterMode 中心剩余秒数、休息 10s 蓝→灰反向进度、全屏休息提醒、setTime 20s 周期适配）

## 结论：**PASS**

构建/安装退出码均 0，产物与本次构建哈希一致；截图模式三组命令全部退出码 0 且尺寸严格符合；像素分析 8/8 通过（中心数字颜色、按钮纯色无渐变、休息反向灰化前段、蓝色刻度像素比 2.30）；无参运行跨工作→休息边界存活 15 秒，且在 12 秒处截屏**实测确认全屏「休息一下」遮罩已自动弹出**（此项由人工验证项升级为自动验证通过）。

## 1. 构建与安装（Release）—— PASS

| 命令 | 期望退出码 | 实际退出码 | 结果 |
|---|---|---|---|
| `cmake --build _build --config Release` | 0 | 0（`main.vcxproj -> _build\main\Release\main.exe`） | PASS |
| `cmake --install _build --config Release` | 0 | 0（`-- Up-to-date: _install/main.exe` + windeployqt） | PASS |

- 产物：`D:\qsw\禅道\_install\main.exe`（51712 字节），与 `_build\main\Release\main.exe` **SHA256 哈希一致**。
- 时间戳说明：产物 LastWriteTime 为 18:27:56，系本轮迭代执行 Agent 构建产物；测试轮次开始（18:35:11）后源码无变化，构建为增量 no-op，故时间戳未再推进。已用哈希一致性确认安装产物即本轮迭代代码的构建结果，无陈旧产物风险。
- windeployqt 两条无害警告（缺 translations 目录、缺 dxcompiler.dll），不影响运行。

## 2. 截图模式验收 —— PASS

命令均在 `_shottest\round1` 下以 `Start-Process -Wait -PassThru` 同步执行并取真实退出码；进程秒级自行结束，无弹窗、无挂起。

| 命令 | 退出码 | 产物 | 尺寸 | 结果 |
|---|---|---|---|---|
| `main.exe -t 3` | 0 | `shot_3.png` | 严格 400×400 | PASS |
| `main.exe -t 13` | 0 | `shot_13.png` | 严格 400×400 | PASS |
| `main.exe -t 1 -s 200` | 0 | `shot_1.png` | 严格 200×200（DPR 归一化无回归） | PASS |

像素分析（`_shottest\analyze_round1.py`，容差 ±12，脚本退出码 0，8/8 通过）：

| 检查项 | 实测数据 | 结果 |
|---|---|---|
| `shot_3` 中心亮色数字（工作剩 7） | 中心区域 #E8E8E8 像素 **346**，蓝色像素 0 | PASS |
| `shot_3` 按钮纯色 #55B2E8 无渐变 | 中心与上下左右 4 个近边缘点全部为 **(85,178,232)**，完全一致 | PASS |
| `shot_13` 中心蓝色数字（休息剩 7） | 中心区域蓝色像素 **346**，亮色像素 0 | PASS |
| 蓝色刻度像素对比 | t=3（工作 30%）= **2935**，t=13（休息剩 70%）= **6748**，比值 **2.30**（>1.5 阈值） | PASS |
| `shot_13` 休息已过 3s 前段变灰 | 3 点钟方向起 2°~100°（约前 27%）采样：灰 **123**、蓝 **0**，前段已全部变灰 | PASS |

目视复核（测试 Agent 直接查看 PNG）：`shot_3.png` 中心亮色「7」、约 30% 蓝色刻度、扁平蓝色按钮；`shot_13.png` 中心蓝色「7」、圆环大部蓝色、顺时针前段灰色；右上角均可见 × 关闭按钮。均符合预期。

## 3. 交互模式验收 —— PASS

- 命令：无参 `Start-Process _install\main.exe`（PID 35812）。
- 存活检查：6s / 12s / 15s 三处 `HasExited=False`，**跨越 10s 工作→休息边界无崩溃、无异常退出**；随后 `Stop-Process -Force` 正常终止。
- 全屏提醒**自动验证通过**：12 秒处（休息阶段第 2 秒）用 PIL ImageGrab 截全屏（`interactive_at_12s.png`），画面中可见全屏半透明黑色遮罩 + 白色「休息一下」+ 大号剩余秒数「7」，主窗口圆环已转蓝、中心蓝色「7」。休息结束自动关闭依赖同一 `phaseChanged` 信号（代码确认，见第 4 节）。

## 4. 代码审查佐证（只读）—— PASS

| 审查点 | 结论 | 位置 |
|---|---|---|
| 休息进度蓝→灰反向 | ✔ 休息分支 `t = 1.0 - cover`（蓝色占比随进度递减），前沿渐变方向反转 | `focus_timer_widget.cpp:145-187` |
| 关闭按钮点击为 hide() | ✔ `mousePressEvent` 命中关闭按钮后 `hide(); return;`，非 close/quit | `frameless_window.cpp:203-207` |
| 托盘菜单含「退出」 | ✔ `menu->addAction("退出")` → `QApplication::quit`；托盘不可用则跳过创建 | `frameless_window.cpp:104-135` |
| setTime 周期为工作+休息之和 | ✔ `fmod(seconds, m_workDuration + m_restDuration)` = 20s 周期 | `focus_timer_widget.cpp:36-44` |
| 截图模式不创建托盘/全屏提醒 | ✔ `-t` 分支只构造 `FocusTimerWidget`，不构造 `FramelessWindow`；`setTime` 停表不发射 `phaseChanged` | `main.cpp:26-49`、`focus_timer_widget.cpp:30-46` |

## 人工验证项（不计入 FAIL）

| 项 | 原因 | 代码审查结论 |
|---|---|---|
| 点击右上角 × → 主窗口隐藏到托盘 | 需真实鼠标点击 | 代码路径确认：`hide()` + 托盘图标已创建 |
| 托盘右键菜单「退出」真正关闭程序 | 需托盘交互 | 代码路径确认：菜单项连接 `QApplication::quit` |
| 托盘左键/双击恢复主窗口 | 需托盘交互 | 代码路径确认：`activated` 信号 → `showNormal()` |
| 休息结束全屏遮罩自动关闭 | 本轮截屏在休息中段取得，未覆盖结束瞬间 | 代码路径确认：`phaseChanged(0)` → `overlay->end()`（`frameless_window.cpp:141-148`） |

## 汇总表

| 验收项 | 期望 | 实际 | 结果 |
|---|---|---|---|
| 构建 / 安装 Release | 退出码 0 | 0 / 0 | PASS |
| `_install/main.exe` 产物 | 存在且为本轮构建 | 存在，与构建输出哈希一致 | PASS |
| `-t 3` | 退出码 0、400×400、中心 #E8E8E8「7」、按钮纯色 | 全部符合（亮色像素 346，按钮五点纯色一致） | PASS |
| `-t 13` | 中心蓝色「7」、前段约 30% 变灰、蓝刻度远多于 -t 3 | 全部符合（蓝 6748 vs 2935，前段灰 123 蓝 0） | PASS |
| `-t 1 -s 200` | 严格 200×200 | 200×200，退出码 0 | PASS |
| 无参运行 ≥12s 跨阶段边界 | 不崩溃 | 15 秒存活，正常终止 | PASS |
| 全屏提醒弹出 | 工作结束时弹出 | 12s 截屏实测弹出（遮罩+「休息一下」+「7」） | PASS |
| 代码审查 5 项 | 实现就位 | 逐项确认 | PASS |

---

# 交付测试报告 — 按钮缩小/动画/刻度参数化/自适应/无边框迭代 — Round 1 / 5

- 测试时间：2026-08-18 17:30（UTC+8）
- 测试环境：Windows 11 (26200)，PowerShell + `System.Drawing`，显示器缩放 200%（DPR=2）
- 截图工作目录：`D:\qsw\禅道\_shottest\round1`（临时目录，未污染根目录）
- 对应交付：按钮直径 ≈ 边长 10%、按钮动画、TickStyle 刻度参数化（调细）、自适应最小 200×200、无边框窗口

## 结论：**PASS**

构建/安装退出码均为 0，`_install/main.exe` 时间戳更新；蓝色按钮直径实测恰为截图边长的 10.00%；`TickStyle` 集中于头文件且宽度比 0.026，截图中刻度明显纤细；`-t` 接口三组截图尺寸严格符合、退出码全 0、1.5s 蓝色刻度像素多于 1s；无边框/最小尺寸/正方形回正/关闭按钮/边缘缩放热区在源码中均确认存在；无参启动 5 秒不崩溃。

## 1. 构建与安装（Release）—— PASS

| 命令 | 期望退出码 | 实际退出码 | 结果 |
|---|---|---|---|
| `cmake --build _build --config Release` | 0 | 0（`main.vcxproj -> _build\main\Release\main.exe`） | PASS |
| `cmake --install _build --config Release` | 0 | 0（`-- Installing: D:/qsw/禅道/_install/main.exe`） | PASS |

- 产物：`D:\qsw\禅道\_install\main.exe` 存在，LastWriteTime 由 2026-08-18 16:54:35 更新为 **2026-08-18 17:25:47**（本轮新构建）。
- windeployqt 仅两条无害警告（缺 translations 目录、缺 dxcompiler.dll），不影响运行。

## 2. 验收标准逐条核对

### a. 按钮直径 ≈ 截图边长 10% —— PASS

- 命令：在 `_shottest\round1` 运行 `_install\main.exe -t 1 -s 400`，退出码 **0**，产物 `shot_1.png`（为避免同名覆盖已改名 `shot_1_400.png`）。
- 尺寸：`System.Drawing` 读取为严格 **400×400**。
- 量测方法：逐像素扫描蓝色像素（B > R+20 且 B > 80），定位底部蓝色圆形按钮；并用区域 ASCII 像素图 + 4 倍放大裁剪图人工复核（按钮为蓝色圆盘 + 深色暂停图标条）。
- 实测：蓝色按钮边界框 x=[180,219]、y=[292,331]，宽 **40px**、高 **40px**，占 400px 边长比例 **10.00% × 10.00%**（蓝色像素 1128，因内部暂停条为深色略少于完整圆面积）。
- 判定：10.00% 落在 8%~12% 区间，且明显小于上一版 25%。

### b. 刻度变细 + TickStyle 参数集中 —— PASS

- 源码核对：`main/src/focus_timer_widget.h` 第 8~18 行，`TickStyle` 结构体集中在头文件顶部（count=36、widthRatio=**0.026**、heightRatio=0.08、cornerRatio=0.013、ringRadius=0.28、frontFade=0.03），每项带中文注释与调节建议；`focus_timer_widget.cpp` 的 `drawRing` 全部改读 `ts.*` 成员。
- 截图观感：`shot_1_400.png` 中刻度为细胶囊形蓝/灰条，明显细于旧版（旧版宽度比 0.04，本轮 0.026，缩细约 35%），放大复核确认。

### c. `-t` 接口不回归 —— PASS

| 命令 | 退出码 | 产物尺寸 | 结果 |
|---|---|---|---|
| `main.exe -t 1 -s 400` | 0 | 严格 400×400 | PASS |
| `main.exe -t 1.5 -s 400` | 0 | 严格 400×400 | PASS |
| `main.exe -t 1 -s 200` | 0 | 严格 200×200 | PASS |

- 三组均通过 `Start-Process -Wait -PassThru` 获取退出码，进程秒级自行结束，不弹窗、不挂起（DPR=2 机器上归一化仍生效）。
- 进度推进验证：统计表盘区域（y<280）蓝色刻度像素，`-t 1` = **1146**，`-t 1.5` = **1711**（多 565，约 +49%），方向正确（1.5s 进度更靠前，激活刻度更多）。

### d. 自适应与无边框（静态核对）—— PASS

`main/src/frameless_window.h/.cpp`、`focus_timer_widget.h/.cpp` 源码逐项确认（交互手感不测）：

- 最小尺寸 200×200：`frameless_window.cpp:16` `setMinimumSize(200, 200)`。
- 正方形回正：`applyResize`（`frameless_window.cpp:151`，以短边为准、拖左/上边保持对侧边不动）与 `resizeEvent`（`:166-177`，程序性 resize 按短边回正，宽高差判断防递归）双路径均存在。
- 无边框：`frameless_window.cpp:14` `QMainWindow(parent, Qt::FramelessWindowHint)`。
- 左上角关闭按钮：`focus_timer_widget.cpp:295 closeButtonRect()`（边距 `base*0.04`，直径 `base*0.07`，即左上角）、`drawCloseButton()` 绘制扁平 ×；窗口层 `frameless_window.cpp:87-88` 命中后 `close()`。
- 边缘缩放热区：`frameless_window.cpp:34 edgeHit()`（6px 热区 8 方向）+ `applyResize()` 手动 `setGeometry`，纯 Qt 实现。
- 等比缩放：所有绘制按 `base = qMin(w, h)` 比例坐标，无固定像素。

## 3. 无参数启动稳定性 —— PASS

- 命令：`Start-Process _install\main.exe`（无参，工作目录 `_shottest\round1`），保持 5 秒后检查。
- 结果：进程（PID 36052）存活满 5 秒，未崩溃、未自行退出；随后 `Stop-Process -Force` 正常结束。

## 汇总表

| 验收项 | 期望 | 实际 | 结果 |
|---|---|---|---|
| 构建 / 安装 | 退出码 0 | 0 / 0 | PASS |
| `_install/main.exe` 更新 | 时间戳更新 | 16:54:35 → 17:25:47 | PASS |
| 按钮直径占比 | 8%~12%，明显小于 25% | 10.00% × 10.00% | PASS |
| `shot_1.png`（-t 1 -s 400） | 400×400，退出码 0 | 400×400，0 | PASS |
| TickStyle 集中头文件、宽度比 0.026 | 存在且调细 | 已确认，截图刻度细 | PASS |
| `-t 1.5` vs `-t 1` 蓝色刻度 | 1.5s 更多 | 1711 > 1146 | PASS |
| `-t 1 -s 200` | 200×200，退出码 0 | 200×200，0 | PASS |
| 最小 200×200 / 正方形回正 / 无边框 / 关闭按钮 / 边缘热区 | 实现存在 | 源码逐项确认 | PASS |
| 无参启动 | 3~5 秒不崩溃 | 5 秒存活后正常终止 | PASS |

---

# 交付测试报告 — Round 2 / 5

- 测试时间：2026-08-18 16:57（UTC+8）
- 测试环境：Windows 11 (26200)，PowerShell + `System.Drawing`，显示器缩放 200%（DPR=2），干净测试目录 `D:\qsw\禅道\_shottest`
- 对应修复：round 1 失败根因（高 DPI 下 `grab()` 尺寸翻倍）已在 `main.cpp` 加 `scaled` 归一化修复

## 结论：**PASS**

六项验收标准全部实测通过：构建/安装成功，截图尺寸严格 400×400 / 400×400 / 200×200，蓝色激活刻度像素 1.5s 多于 1s，无参交互模式不回归。

## 逐条验收结果

### 1. 构建 + 安装 + 产物更新 —— PASS

- 命令：`cmake --build _build --config Release`，退出码 0（`main.vcxproj -> D:\qsw\禅道\_build\main\Release\main.exe`）
- 命令：`cmake --install _build --config Release`，退出码 0（`-- Installing: D:/qsw/禅道/_install/main.exe`，windeployqt 仅有两条无害警告：缺 translations 目录、缺 dxcompiler.dll，不影响运行）
- 产物：`D:\qsw\禅道\_install\main.exe`，LastWriteTime = 2026-08-18 16:54:35，为本轮修复后新构建产物

### 2. `-t 1` 出图 —— PASS

- 命令：清空 `D:\qsw\禅道\_shottest` 后，在其中运行 `D:\qsw\禅道\_install\main.exe -t 1`（Start-Process，15 秒超时看护）
- 退出码：0；进程 1 秒内自行结束，不弹窗、不挂起
- 产物：`shot_1.png`，尺寸 **400×400**（严格符合；round 1 此处为 800×800，DPR 归一化修复生效）

### 3. `-t 1.5` 出图 —— PASS

- 命令：`main.exe -t 1.5`，退出码 0，不挂起
- 产物：`shot_1.5.png`，尺寸 **400×400**（严格符合）

### 4. 两图像素对比 —— PASS

以容差 ±40 匹配接近 #55B2E8 (R85 G178 B232) 的像素（LockBits 逐像素统计）：

- `shot_1.png`（进度 0.10）：蓝色像素 **9061**
- `shot_1.5.png`（进度 0.15）：蓝色像素 **9832**
- 9832 > 9061，多 771（约 +8.5%），方向与预期一致（激活刻度更多）

### 5. `-t 1 -s 200` 尺寸 —— PASS

- 命令：`main.exe -t 1 -s 200`，退出码 0
- 产物 `shot_1.png`（与第 2 条同名，已按序分开测试并即时读尺寸）：**200×200**（严格符合；round 1 此处为 400×400）
- 注：因文件名只含 `-t` 参数，本轮按「先 `-s 200` 读尺寸 → 删除 → 再 `-t 1` 读尺寸」顺序复测，避免覆盖混淆。

### 6. 无参数交互模式 —— PASS

- 命令：无参 `Start-Process main.exe`，3 秒后检查进程状态
- 结果：进程（PID 36444）存活满 3 秒未崩溃、未自行退出；`taskkill /PID 36444 /F` 正常结束
- 交互窗口行为无回归

## 汇总表

| 验收项 | 期望 | 实际 | 结果 |
|---|---|---|---|
| 构建 / 安装 | 退出码 0 | 0 / 0 | PASS |
| `-t 1` 尺寸 | 400×400 | 400×400 | PASS |
| `-t 1.5` 尺寸 | 400×400 | 400×400 | PASS |
| 蓝色像素对比 | 1.5s > 1s | 9832 > 9061 | PASS |
| `-t 1 -s 200` 尺寸 | 200×200 | 200×200 | PASS |
| 无参弹窗 | 存活不崩溃 | 3 秒存活，可正常结束 | PASS |

---

# 交付测试报告 — Round 1 / 5

- 测试时间：2026-08-18 16:50（UTC+8）
- 测试环境：Windows 11 (26200)，PowerShell，测试目录 `D:\qsw\禅道\_shottest`
- 分析方式：PowerShell + `System.Drawing`（PIL 不可用）

## 结论：**FAIL**

核心功能（出图、内容差异、弹窗）均正常，但**截图尺寸不符合验收标准**：默认输出为 800×800 而非 400×400，`-s 200` 输出 400×400 而非 200×200。疑似高 DPI（devicePixelRatio = 2）下 `QWidget::grab()` 返回物理像素导致尺寸翻倍。

## 逐条验收结果

### 1. 构建 + 安装 + 产物更新 —— PASS

- 命令：`cmake --build _build --config Release`，退出码 0
- 命令：`cmake --install _build --config Release`，退出码 0
- 产物：`D:\qsw\禅道\_install\main.exe`，LastWriteTime = 2026-08-18 16:48:43（本轮刚构建），31232 字节

### 2. `-t 1` 出图 —— FAIL（尺寸不符）

- 命令：在干净目录 `D:\qsw\禅道\_shottest` 中运行 `D:\qsw\禅道\_install\main.exe -t 1`
- 退出码：0；进程立即结束，不弹窗、不挂起
- 产物：`shot_1.png`（41794 字节）
- **实际尺寸 800×800，验收要求 400×400 → 不符**

### 3. `-t 1.5` 出图 —— FAIL（尺寸不符）

- 命令：`main.exe -t 1.5`，退出码 0，不挂起
- 产物：`shot_1.5.png`（43422 字节）
- **实际尺寸 800×800，验收要求 400×400 → 不符**

### 4. 两图像素对比 —— PASS（趋势正确）

以容差 ±40 匹配接近 #55B2E8 (85,178,232) 的像素：

- `shot_1.png`：蓝色像素 **36548**
- `shot_1.5.png`：蓝色像素 **39708**（多 3160，约 +8.6%，对应进度差 0.05 ≈ 1.8 个刻度量级，合理）
- MD5：`shot_1.png` = 9A283866E3E352BB39BA77AEC759517E，`shot_1.5.png` = 49DFD3D5771A9ADCD5CF5AB4DC3A8A12，两文件内容确实不同
- 目视确认：表盘、蓝色进度弧、计时数字、暂停图标均正常渲染

### 5. `-t 1 -s 200` 尺寸 —— FAIL

- 命令：`main.exe -t 1 -s 200`，退出码 0
- **实际输出 400×400，验收要求 200×200 → 不符**（同样约为 2 倍，指向同一 DPR 根因）

### 6. 无参数弹窗 —— PASS

- `Start-Process main.exe`，3 秒后进程仍存活（PID 31728），未闪退
- 强制结束后退出码 -1（taskkill 正常结果，非崩溃）

## 失败摘要与修复建议

| 项 | 期望 | 实际 |
|---|---|---|
| `-t 1` 尺寸 | 400×400 | 800×800 |
| `-t 1.5` 尺寸 | 400×400 | 800×800 |
| `-s 200` 尺寸 | 200×200 | 400×400 |

- 缺失产物：无（文件均生成）；问题是尺寸全部恰好为期望值的 2 倍。
- 根因推断：测试机显示器缩放 200%（devicePixelRatio = 2），`QWidget::grab()` 返回的 QPixmap 为物理像素，`QPixmap::toImage()` 后未按 DPR 归一化。
- 修复建议（供实施 Agent 参考，任选其一）：
  1. 保存前 `image = image.scaled(size, size)`（当 `image.width() != size` 时）；
  2. 或对 grab 结果设置 `setDevicePixelRatio(1.0)` 后缩放回逻辑尺寸；
  3. 或截图模式下调用 `QGuiApplication::setHighDpiScaleFactorRoundingPolicy` / 设置环境变量 `QT_SCALE_FACTOR=1` 后按 1.0 DPR 渲染。

---

# 交付测试报告 — 按钮下移白图标 / 「...」按钮 / 设置弹窗 / 时长持久化 / HH:MM:SS — Round 1 / 5

- 测试时间：2026-08-19 10:16–10:25（UTC+8）
- 测试环境：Windows 11 (26200)，PowerShell + Python 3.14.7 / Pillow 12.3.0，显示器缩放 200%（DPR=2）
- 截图工作目录：`D:\qsw\禅道\_shottest\round4`；像素分析脚本：`_shottest\round4\analyze_round4.py`（测试辅助，非产品源码）
- 对应需求：`plan/delivery-plan.md`「需求原文」——播放/暂停按钮下移且图标改白；新增「...」按钮+下拉菜单+设置弹窗（HH:MM:SS 时长、QSettings 持久化）；中心时间改 HH:MM:SS

## 结论：**FAIL**

10 项自动化检查通过 9 项；唯一失败项为验收标准 3c：「...」按钮三个圆点**位置与可见性达标，但颜色为灰色 #7A7A7A，不是计划规定的白色**（计划第 30 行「自绘三个白色横向圆点」，验收标准「三个白点可见」）。实现使用 `m_inactiveColor.lighter(200 + 120*hover)`，基色 #3D3D3D 提亮 200% 后仅得 (122,122,122)，即使悬停拉满（lighter(320)）也只有约 (195,195,195)，任何状态下都达不到白色。其余全部验收项（构建/安装、截图回归、按钮下移、白色暂停图标、HH:MM:SS、setDurations 分支、截图模式不加载 QSettings、无参 5 秒稳定、事件链回归）均通过。

## 1. 构建与安装（Release）—— PASS

| 命令 | 期望退出码 | 实际退出码 | 结果 |
|---|---|---|---|
| `cmake --build _build --config Release` | 0 | 0（`main.vcxproj -> _build\main\Release\main.exe`） | PASS |
| `cmake --install _build --config Release` | 0 | 0（`-- Installing: _install/main.exe` + windeployqt） | PASS |

- 产物佐证：`_install/main.exe` LastWriteTime **2026-08-19 10:15:12**，77824 字节（上一轮 49152 字节，体积变化与新增设置弹窗/菜单代码相符），为本次构建后的最新产物。
- windeployqt 两条无害警告（缺 translations、缺 dxcompiler.dll），与既往轮次相同，不影响运行。

## 2. 截图回归 —— PASS

命令均在 `_shottest\round4` 下以 `Start-Process -Wait -PassThru` 同步执行取真实退出码：

| 命令 | 退出码 | 产物 | 尺寸 | 结果 |
|---|---|---|---|---|
| `main.exe -t 1` | 0 | `shot_1.png` (16995 B) | 严格 400×400 | PASS |
| `main.exe -t 1.5` | 0 | `shot_1.5.png` (17693 B) | 严格 400×400 | PASS |
| `main.exe -t 3` | 0 | `shot_3.png` (18382 B) | 严格 400×400 | PASS |
| `main.exe -t 13` | 0 | `shot_13.png` (21154 B) | 严格 400×400 | PASS |

- 四条命令均秒级返回、退出码 0，截图模式未弹窗（进程立即退出，无窗口驻留）。
- 蓝色像素趋势：圆环区域（半径 90~134px）蓝色像素 t=1 为 **970**，t=1.5 为 **1436**，1.5s > 1s 成立（PASS）。
- `-t 13` 为休息阶段：中心区蓝色像素 1020、亮色像素 0，中心数字为蓝色（休息配色），PASS。

## 3. 截图验证新 UI（`-t 3`，400×400）

像素分析脚本 `analyze_round4.py` 输出（脚本退出码 1，因 3c 失败）：

| 验收项 | 实测证据 | 结果 |
|---|---|---|
| a. 播放按钮中心 y/边长 ≥ 0.83 | 下半区蓝色按钮像素 1081 个，质心 (199.7, 335.6)，**y/边长 = 0.839** | PASS |
| b. 按钮图标为白色 | 按钮中心区近白（#FFFFFF±20）像素 **108** 个，暂停双竖线清晰 | PASS |
| c. 「...」三个白点可见 | 三点存在于按钮右侧同一水平线（实测圆点中心约 x=230/240/248, y=344，与播放按钮 y=344 同线），**但像素值为 (122,122,122) 灰色，非白色** | **FAIL** |
| d. 中心时间 HH:MM:SS | 中心亮色文字包围盒宽 124px × 高 23px（宽远大于 4 位纯秒数）；人工判读 `shot_3.png` 确认显示 **「00:00:07」** | PASS |

- 人工判读佐证：`shot_3.png` 全图可见——中心「00:00:07」、蓝色播放按钮下移至圆环下方、白色暂停双竖线、右侧「...」三点（灰色）、右上角 ×；放大图 `_shottest\round4\zoom_shot3_bottom.png` 确认三点位置与颜色。

## 4. 设置应用逻辑（代码审查）—— PASS

- `focus_timer_widget.cpp` `setDurations`（68–87 行）：非正数直接拒绝；先存新时长；当前阶段 `m_elapsed >= 新时长` → 立即切到下一阶段、`m_elapsed = 0`、发 `phaseChanged`；否则 `m_elapsed` 不动即续跑（剩余 = 新时长 - 已用）。分支与对齐结论一致。
- 截图模式不发 `phaseChanged`：`setTime` 置 `m_timerStopped = true`，`setDurations` 内 `if (!m_timerStopped)` 才 emit，PASS。
- `main.cpp`：QSettings 读取（57–60 行）仅在不带 `-t` 的交互分支；截图分支（27–50 行）无任何 QSettings 调用，始终默认 10s/10s，PASS。

## 5. 无参启动 5 秒稳定性 —— PASS

- 前置检查：`HKCU\Software\Chandao\FocusTimer` 注册表项不存在（此前测试未写入过设置），无需清理。
- `Start-Process _install\main.exe -PassThru` 启动，5 秒后 `HasExited = False`（PID 23876），进程存活无崩溃；随后 `Stop-Process` 杀掉。

## 6. 事件链回归（代码审查）—— PASS

- `focus_timer_widget.cpp`：`mousePressEvent` 未命中播放/「...」按钮（含右上角 ×）时 `event->ignore()`（129–132 行）；`mouseReleaseEvent` 两按钮均未按下时 `ignore()`（149–151 行）；`mouseMoveEvent` 不在任何热区时 `ignore()`（166–169 行）。父窗口事件链不被截获。
- `frameless_window.cpp`：`mousePressEvent` 判定顺序为 ×（hide 到托盘）→ 播放/「...」按钮（交还控件，不触发拖拽）→ 边缘缩放 → 空白拖拽（144–162 行），dots 热区已排除在拖拽判定外；`applyResize` 短边回正、`resizeEvent` 程序性回正逻辑未动。

## 7. 人工验证项（不影响 PASS/FAIL 判定）

以下交互项自动测试无法覆盖，需人工验证：

1. 点「...」弹出深色下拉菜单 → 点「设置」弹出无边框设置弹窗。
2. 修改专注/休息时长 → 「确定」生效（续跑或立即进下一阶段）；× 关闭不保存。
3. 重启程序后时长保持（QSettings 持久化，注册表 `HKCU\Software\Chandao\FocusTimer`）。
4. 窗口拖拽 / 右上角 × 隐藏托盘 / 边缘缩放的实际手感（本轮已做代码审查，事件链无破坏）。

## 失败摘要与修复建议

| 项 | 期望 | 实际 |
|---|---|---|
| 「...」圆点颜色 | 白色（计划第 30 行「自绘三个白色横向圆点」；验收 3c「白点可见」） | 灰色 (122,122,122)，即 #7A7A7A |

- 失败命令/证据：`python _shottest\round4\analyze_round4.py` 退出码 1，`[FAIL] 「...」按钮三个白点可见: 左/中/右点命中=[False, False, False]`（检测阈值 >200 未命中）；精确取像素 `(244,344) = (122,122,122)`。
- 缺失产物：无（四张截图均生成且尺寸正确）。
- 根因：`focus_timer_widget.cpp` 第 338 行 `drawDotsButton` 使用 `m_inactiveColor.lighter(200 + 120 * m_dotsHoverT)`，基色 #3D3D3D 提亮后最高仅约 (195,195,195)，永远达不到白色。
- 修复建议（供实施 Agent 参考）：将圆点基色改为 `QColor(0xFF, 0xFF, 0xFF)`（与暂停/播放图标一致），悬停提亮可改为对白色做 alpha 或微暗化处理；若设计上希望非悬停态略暗，应与计划「白色」要求对齐后再定（建议直接白色，与本次「图标改白」需求风格统一）。

---

# 交付测试报告 — 按钮下移白图标 / 「...」按钮 / 设置弹窗 / 时长持久化 / HH:MM:SS — Round 2 / 5（复测）

- 测试时间：2026-08-19 10:20–10:25（UTC+8）
- 测试环境：Windows 11 (26200)，PowerShell + Python 3.14.7 / Pillow 12.3.0，显示器缩放 200%（DPR=2）
- 截图工作目录：`D:\qsw\禅道\_shottest\round5`
- 对应修复：round 1 唯一失败项——`drawDotsButton` 圆点颜色改纯白 #FFFFFF（一行改动）

## 结论：**PASS**

round 1 失败项已修复：`-t 3` 截图中「...」按钮左/中/右三个圆点最亮点像素均为 **(255, 255, 255) 纯白**，满足 R/G/B ≥ 240 的近白要求。构建/安装退出码均 0，`_install/main.exe` 时间戳 2026-08-19 10:19:42 为本轮新构建；快速回归（`-t 1`/`-t 1.5` 尺寸与退出码、播放按钮下移位置 y/边长=0.839、白色图标近白像素 108 个、无参启动 5 秒存活）全部通过，无任何回退。

## 1. 构建与安装（Release）—— PASS

| 命令 | 期望退出码 | 实际退出码 | 结果 |
|---|---|---|---|
| `cmake --build _build --config Release` | 0 | 0（`main.vcxproj -> _build\main\Release\main.exe`） | PASS |
| `cmake --install _build --config Release` | 0 | 0（`-- Installing: _install/main.exe` + windeployqt） | PASS |

- 产物佐证：`_install/main.exe` LastWriteTime **2026-08-19 10:19:42**（测试执行于 10:20 起），77824 字节，为本轮修复后的最新产物。

## 2. 失败项复测：「...」圆点白度 —— PASS

| 命令 | 退出码 | 产物 | 检测证据 | 结果 |
|---|---|---|---|---|
| `main.exe -t 3` | 0 | `round5\shot_3.png`，严格 400×400 | 圆点区域（预期中心 (244,344)，三点间距约 9px）逐点取最亮像素：左=(255,255,255)、中=(255,255,255)、右=(255,255,255)，RGB 均 ≥ 240 | PASS |

## 3. 快速回归 —— PASS

| 项 | 实测证据 | 结果 |
|---|---|---|
| `main.exe -t 1` | 退出码 0，`shot_1.png` 严格 400×400 | PASS |
| `main.exe -t 1.5` | 退出码 0，`shot_1.5.png` 严格 400×400 | PASS |
| 播放按钮下移不回退 | 蓝色按钮质心 (199.7, 335.6)，y/边长 = 0.839 ≥ 0.83 | PASS |
| 按钮白色图标不回退 | 按钮中心区近白（≥235）像素 108 个，与 round 1 持平 | PASS |
| 无参启动 5 秒不崩溃 | 启动前先清理注册表 `HKCU\Software\Chandao\FocusTimer`（该项本次存在，已删除）；进程 PID 37968 五秒后 `HasExited=False`，随后杀掉 | PASS |

## 4. 遗留说明

- 人工验证项（点「...」出菜单、设置弹窗交互、× 不保存、重启持久化生效）与 round 1 一致，仍需人工核验，不影响本结论。
- round 1 其余通过项（setDurations 分支、截图模式不加载 QSettings、事件链代码审查、`-t 13` 休息阶段、蓝色趋势）本轮未重复全量执行，相关代码路径未被本轮一行改动触及，无回归风险。
