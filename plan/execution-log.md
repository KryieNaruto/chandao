# 执行记录

## 2026-08-18 交付执行（任务 1、2）

### 做了什么

- 任务 1：`FocusTimerWidget` 增加公共方法 `setTime(double seconds)`。实现逻辑：停止内部 `QTimer`，置 `m_isRunning = true`，按「工作 10s + 休息 3s」循环用 `fmod(seconds, 13.0)` 取模；余数 < `m_workDuration` 时 `m_state = 0`、`m_elapsed = 余数`，否则 `m_state = 1`、`m_elapsed = 余数 - m_workDuration`。未改动现有 `paintEvent`/鼠标/定时器逻辑。
- 任务 2：`main.cpp` 增加命令行解析。支持 `-t <秒数>`（double，支持小数）与 `-s <边长>`（int，默认 400）。提供 `-t` 时：不显示窗口，创建 `FocusTimerWidget`，`resize(size, size)`，调用 `setTime(t)`，用 `grab()` 转 `QImage` 保存 PNG，文件名为 `shot_<t参数原文>.png`，保存到进程当前工作目录，成功返回 0。未提供 `-t` 时保持原有交互窗口行为（屏幕面积 30% 正方形窗口，标题「专注」）完全不变。

### 改了哪些文件

- `main/src/focus_timer_widget.h`：声明 `setTime(double)`。
- `main/src/focus_timer_widget.cpp`：实现 `setTime(double)`，新增 `<cmath>` 头文件。
- `main/src/main.cpp`：新增 `-t` / `-s` 解析与离屏截图保存逻辑。

### 编译自检

- `cmake --build _build --config Release` 通过，`main.exe` 生成成功，无编译错误。

### 未做事项

- 未执行 `cmake --install` 与出图对比验收（按计划由测试 Agent 负责）。
- 未做自动图像对比功能（计划明确不做）。

## 2026-08-18 round 2 修复（高 DPI 截图尺寸翻倍）

### 修复内容

- 现象：DPR=2 机器上 `QWidget::grab()` 返回物理像素，PNG 尺寸为 `-s` 指定值的 2 倍（默认 400 出 800×800）。
- 按排查 Agent 方案 A（最小改动），在 `main/src/main.cpp` 截图保存段：`grab()` 转 `QImage` 后，若 `image.size() != QSize(shotSize, shotSize)`，则用 `image.scaled(shotSize, shotSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation)` 归一化，再保存 PNG；保存失败返回 1。100% 缩放机器走原路径，零开销无回归。
- 未改动 `focus_timer_widget.*`、CMake 及其他任何逻辑。

### 改动文件

- `main/src/main.cpp`：截图保存段新增 DPR 尺寸归一化（含 `QImage` 变量与条件缩放，含中文注释说明原因）。

### 编译自检

- `cmake --build _build --config Release` 通过，`main.exe` 重新生成成功，无编译错误。

### 未做事项 / 阻塞

- 未执行 `cmake --install` 与逐条验收（尺寸 400×400 / 200×200、蓝色像素对比、无参弹窗回归），由测试 Agent 在 round 2 复测。
- 无阻塞。

## 2026-08-18 交付执行（按钮缩小 / 按钮动画 / 刻度参数化 / 自适应 / 无边框窗口）

### 做了什么

1. **按钮缩小**：`drawButton` 与 `buttonRect` 半径由 `base * 0.126` 改为 `base * 0.05`（直径 ≈ 窗口边长 10%），内部图标（双竖线/三角）尺寸全部改为按按钮半径等比（间距 `r*0.30`、半高 `r*0.42`、线宽 `r*0.22`），小窗口下天然自适应。
2. **按钮动画**：复用现有 60fps `QTimer`（`updateTimer`）推进 4 个 0..1 插值系数——悬停提亮（按钮径向渐变两端按 `lighter()` 提亮，约 120ms）、按压瞬时缩放 0.92 倍并回弹（约 60ms 响应）、暂停↔播放图标约 150ms 交叉淡化过渡（`m_iconT` 中间态两图标按透明度叠加绘制）；图标保留线性渐变（顶端 `#101010` → 底端 `#3A3A3A`）。新增 `setMouseTracking` 支持悬停；点击语义改为按下缩放、松开在按钮内才切换状态（观感更自然）。
3. **刻度参数化**：`focus_timer_widget.h` 顶部新增 `TickStyle` 结构体（数量 36、宽度比 0.04→0.026 调细、高度比 0.08、圆角比 0.013≈宽度一半呈胶囊形、环半径比 0.28、前沿渐变带宽 0.03），每项带中文注释与调节建议；`drawRing` 全部改读结构体成员。
4. **自适应布局**：所有绘制继续按 `min(w,h)` 比例坐标，无固定像素；窗口最小 200×200、全程以短边回正保持正方形（拖拽缩放在 `applyResize` 内回正，程序性 resize 在 `resizeEvent` 内回正）；`-t` 截图路径未动（仅 `setTime` 内部增加 `m_timerStopped` 置位，停表后不再推进动画，画面保持确定）。
5. **无边框窗口**：新增 `main/src/frameless_window.h/.cpp`（`FramelessWindow : QMainWindow`，`Qt::FramelessWindowHint`），背景 `#2B2B2B` 与表盘一致；控件左上角绘制扁平风 × 关闭按钮（直径 `base*0.07`，悬停浮现浅灰圆底 + × 提亮），窗口层命中后 `close()`；空白区域按住拖拽移动；边缘 6px 热区 8 方向缩放（`edgeHit` + 对应缩放光标 + `applyResize` 手动 `setGeometry`，纯 Qt 实现）；构造时取主屏幕面积 30% 正方形并居中。关闭按钮/播放按钮命中优先于移动/缩放判定，互不冲突。

### 改了哪些文件

- `main/src/focus_timer_widget.h`：新增 `TickStyle` 结构体、动画状态成员、`closeButtonHit`/`playButtonHit` 公共接口、图标/关闭按钮绘制私有方法。
- `main/src/focus_timer_widget.cpp`：按钮缩小、动画插值、刻度参数化、关闭按钮绘制、悬停/按压/松开事件处理。
- `main/src/frameless_window.h` / `main/src/frameless_window.cpp`：新增无边框窗口类。
- `main/src/main.cpp`：交互模式改用 `FramelessWindow`（去掉 `setFixedSize`）；`-t`/`-s` 截图逻辑一行未改。
- `main/CMakeLists.txt`：`main` 目标加入两个新源文件。

### 编译自检

- `cmake --build _build --config Release` 通过，`main.exe` 生成成功，无编译错误/警告。
- 轻量验证（非完整验收）：`-t 1 -s 200` 退出码 0、输出严格 200×200，截图中按钮直径 ≈ 边长 10%、刻度明显变细。

### 未做事项 / 阻塞

- 未执行 `cmake --install` 与逐条交互验收（拖拽/缩放手感、动画观感、`-t 1.5` 回归），由测试 Agent 负责。
- 无阻塞。

## 2026-08-18 交付执行（按钮扁平化 / 托盘化 / 中心数字 / 休息反向 / 全屏提醒）

### 做了什么

1. **按钮扁平化**：`drawButton` 去掉径向渐变（原中心 `#77C8F8` → 边缘 `#55B2E8`），改为纯色 `#55B2E8`（`m_activeColor`），悬停提亮（`lighter(100 + lift*100)`）、按压缩放 0.92、图标交叉淡化动画全部保留；暂停/播放图标的线性渐变同步去除，改为纯色 `#101010`（保留 alpha 交叉淡化）。
2. **关闭按钮右上角 + 托盘化**：`closeButtonRect()` 从左上角移到右上角（`width() - m - d, m`）；`FramelessWindow::mousePressEvent` 命中关闭按钮后由 `close()` 改为 `hide()`；`setupTray()` 创建 `QSystemTrayIcon`（图标取窗口图标，为空则回退 `QStyle::SP_TitleBarNormalButton` 标准图标，未引入新资源文件），右键菜单含「退出」→ `QApplication::quit()`，左键/双击托盘图标恢复主窗口；托盘不可用时跳过创建。截图模式（`-t`）不创建 `FramelessWindow`，天然不建托盘。
3. **CenterMode 扩展架构**：`focus_timer_widget.h` 新增 `enum class CenterMode { TimeText, Plant }`、`setCenterMode`/`centerMode` 接口与 `drawCenterContent` 分发函数；`Plant` 分支留空并注释「植物模式预留」。本期实现 `TimeText`：圆环中心（与 `drawRing` 同圆心 `w*0.5, h*0.4`）绘制剩余秒数，向上取整为整数（减 1e-9 防浮点误差，下限钳到 1，范围 10→1），工作阶段 `#E8E8E8`、休息阶段 `#55B2E8`，字号 `min(w,h) * 0.12` 加粗居中。
4. **休息计时反向**：`m_restDuration` 3.0 → 10.0（周期 20s）；`drawRing` 休息阶段 `progress = elapsed / restDuration`，刻度的蓝色占比 `t = 1 - cover`，从全蓝开始顺时针逐刻度变灰（与工作灰→蓝正好相反）；每个刻度内部沿切线方向的前沿渐变保留，休息分支方向反转（灰在后侧、蓝在前侧）；休息结束自动回到工作阶段。
5. **全屏休息提醒**：`frameless_window.cpp` 内新增内部小类 `RestOverlay : QWidget`（`Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool`，`WA_TranslucentBackground`），覆盖主屏 `screen->geometry()`，背景 `rgba(0,0,0,180)`，居中绘制白色「休息一下」+ 剩余秒数倒计时（100ms 自刷新）；`FocusTimerWidget` 新增 `phaseChanged(int)` 信号在阶段切换时发射，`FramelessWindow::setupRestOverlay()` 持有遮罩并按信号弹出/关闭；截图模式 `setTime` 停表不发射信号、也不创建 `FramelessWindow`，不触发提醒。
6. **setTime 适配**：`setTime` 原有 `fmod(seconds, workDuration + restDuration)` 逻辑不变，随 `restDuration` 变 10s 自动适配 20s 周期；中心数字与休息反向配色均按确定性状态渲染。

### 改了哪些文件

- `main/src/focus_timer_widget.h`：`CenterMode` 枚举、`setCenterMode`/`centerMode`、`remainingSeconds()`、`phaseChanged` 信号、`drawCenterContent`/`drawCenterTimeText` 声明、关闭按钮注释更新。
- `main/src/focus_timer_widget.cpp`：按钮/图标扁平化、关闭按钮右上角、中心数字绘制、休息反向进度与渐变、休息时长 10s、阶段切换发信号。
- `main/src/frameless_window.h`：托盘与休息提醒成员、`setupTray`/`setupRestOverlay` 声明、类注释更新。
- `main/src/frameless_window.cpp`：`RestOverlay` 内部类、托盘创建与菜单、关闭按钮改 `hide()`、阶段信号联动。
- `main/CMakeLists.txt` 未改动（未新增文件）。

### 编译自检

- `cmake --build _build --config Release` 通过，`main.exe` 生成成功，无编译错误。

### 未做事项 / 阻塞

- 未执行 `cmake --install`，未做截图/交互验收（`-t 3`、`-t 13` 中心数字与反向配色、托盘隐藏/退出、全屏提醒弹出与自动关闭），由测试 Agent 负责。
- 植物生长模式仅留枚举与分发接口，未实现（计划明确不做）。
- 无阻塞。

## 2026-08-18 交付执行 round 2（修复鼠标事件不传播：拖拽 / 关闭托盘 / 边缘缩放失效）

### 做了什么

按排查 Agent 定位的根因（`FocusTimerWidget` 铺满客户区且不 `ignore()` 鼠标事件，父窗口 `FramelessWindow` 的拖拽/关闭/边缘缩放处理器收不到事件），仅对 `main/src/focus_timer_widget.cpp` 三个函数做最小修复：

1. `mousePressEvent`：未命中播放按钮 `buttonRect`（含命中右上角 ×）时调用 `event->ignore()`，事件传播给父窗口接管拖拽/关闭/边缘缩放。
2. `mouseReleaseEvent`：`m_buttonPressed` 为假时 `event->ignore()`。
3. `mouseMoveEvent`：悬停状态更新后，既不在播放按钮也不在关闭按钮上时 `event->ignore()`，救活父窗口边缘缩放光标。

坐标无需换算（子控件位于父窗口 (0,0)，Qt 传播时自动转换）。未动窗口 flags（无 `WindowStaysOnTopHint` 残留）。

同时将 round 2 排查段落（审核/根因/修复建议/是否建议停止）代写落盘到 `plan/triage-log.md`。

### 改了哪些文件

- `main/src/focus_timer_widget.cpp`：上述三个鼠标事件函数补 `event->ignore()`。
- `plan/triage-log.md`：追加 round 2 排查段落。
- `plan/execution-log.md`：追加本段。

### 编译自检

- `cmake --build _build --config Release` 通过，`main.exe` 生成成功，无编译错误（实现侧自查）。

### 未做事项 / 阻塞

- 未执行 `cmake --install`，未做截图回归与人工交互复测（拖动、× 隐藏到托盘、边缘缩放），完整验收由测试 Agent 负责。
- 无阻塞。

## 2026-08-19 移除全屏休息提醒，改为「恢复 + 闪烁提醒」

### 做了什么

按 `plan/delivery-plan.md` 末尾「变更（2026-08-19）」段落执行：

1. 整体删除 `RestOverlay` 全屏半透明蒙版小类（创建、显示、倒计时刷新、关闭全部代码），并清理因此不再使用的 `#include <QPainter>` / `<QTimer>` / `<QtMath>`（`<cmath>` 保留，构造器仍用 `std::sqrt`）。
2. `setupRestOverlay()` 改名为 `setupRestAlert()`：仍挂在现有 `FocusTimerWidget::phaseChanged(int)` 信号上，`state == 1`（工作→休息）时——若主窗口不可见（托盘隐藏态）则 `showNormal()` + `activateWindow()` 恢复显示；随后无条件调用 `QApplication::alert(this)`（Windows 上即任务栏闪烁提醒）。休息→工作方向不做任何动作。
3. 其余行为未动：托盘、中心数字、休息反向进度、拖拽、边缘缩放、`-t` 截图模式（信号不发，不触发提醒）均不回归。

### 改了哪些文件

- `main/src/frameless_window.h`：删除 `m_restOverlay` 成员与 `setupRestOverlay()` 声明，新增 `setupRestAlert()`，更新类注释。
- `main/src/frameless_window.cpp`：删除 `RestOverlay` 类与相关 include；实现 `setupRestAlert()`。
- `plan/execution-log.md`：追加本段。

### 编译自检

- `cmake --build _build --config Release` 通过，`main.exe` 生成成功，无编译错误/警告（实现侧自查）。

### 未做事项 / 阻塞

- 未执行 `cmake --install`，未做运行态验证（跨工作→休息边界的恢复显示与任务栏闪烁、托盘/截图回归），完整验收由测试 Agent 负责。
- 无阻塞。
