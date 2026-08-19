# 交付计划

## 需求原文

bug:
1. 点击左上角关闭按钮后，执行动画瞬间，布局全部错误，导致视觉上点击关闭后，窗口会闪一下然后缩小，而不是直接缩小或者关闭。修复！

## 需求对齐结论（需求本身已明确，无待确认项）

- 关闭按钮实际在窗口**右上角** ×（用户所述「左上角」按关闭按钮理解，不改位置）。
- 现象：点 × 后先闪一帧「布局全乱」的真窗口，再缩小/消失。期望：点 × 后直接按快照缩小（吸入）或立刻关闭，过程中表盘/按钮/滚筒不得重排。
- 根因：
  1. **设置窗** `SettingsDialog::reject()` 对**真窗口**做 `geometry` 收缩动画（420×460 → 20×20）。Qt 布局随尺寸重排，滚筒 `setFixedSize` 无法等比缩放，出现错乱布局闪帧。
  2. **主窗口** `hideToTray()` 虽有 `GenieGhost` 快照替身，但 `hide()` 会触发 Windows DWM 默认隐藏过渡，真窗口矩形被系统动画缩放，`FocusTimerWidget` 按比例重绘，同样闪错乱布局；且 `hide()` 在 ghost `show()` 之前，中间可能空一帧。
- 修法：关闭动画只缩放**快照替身**；真窗口几何与子控件布局在动画期间保持不变（不可见即可）。

## 工作区

- 根目录：`D:\qsw\禅道`
- 源码目录：`main/src/`
- 构建目录：`_build/`
- 安装目录：`_install/`（产物 `_install/main.exe`）

## 范围

- 做：
  1. 主窗口 `hideToTray()`：先 `grab()` 并立刻显示覆盖真窗口的 `GenieGhost`；真窗口先 `setWindowOpacity(0)`（几何不动），必要时禁用 DWM 过渡后再 `hide()`；再播替身吸入动画。恢复显示时透明度仍由现有 `showEvent` 弹出动画负责。
  2. 设置窗 `reject()`：改为同一套快照替身。不可对真 Dialog 做 `geometry` 动画。模态 Dialog **不能**在 `exec()` 返回前 `hide()`（会提前 `done/reject`），故用 `setWindowOpacity(0)` 隐藏真窗、几何不动；替身结束后再 `QDialog::reject()`。`accept()`（确定）保持现状，本任务不改。
  3. 将 `GenieGhost` 抽到双方可共用的头文件（例如 `main/src/genie_ghost.h`，header-only，不必改 CMake），避免复制两份动画逻辑。
- 不做：
  - 关闭按钮改到左上角。
  - 改变吸入时长/缓动曲线/终点尺寸（仍约 20×20、InCubic、约 200–220ms）。
  - 弹出动画、托盘逻辑、滚筒交互、截图模式（`-t`）行为。
  - git commit / push。

## 技术栈与约束

- C++20 + Qt6 Widgets + CMake（Visual Studio 18 2026，x64）。
- Qt 路径：`-DCMAKE_PREFIX_PATH="D:\Qt\6.8.3\msvc2022_64"`。
- 构建：`cmake --build _build --config Release`；安装：`cmake --install _build --config Release`。
- 可改：`main/src/frameless_window.cpp/.h`、`main/src/settings_dialog.cpp/.h`、可选新建 `main/src/genie_ghost.h`。
- 若用 `DwmSetWindowAttribute` 禁用过渡，仅限 Windows，用 `#ifdef Q_OS_WIN` 包起来，失败则静默忽略。
- 代码标识符、目录名、目标名保持项目原样。

## 任务拆分

1. 抽出共用 `GenieGhost`（快照 + suckInto） | `main/src/genie_ghost.h`（及两处 include） | 双方关闭动画共用同一替身。
2. 主窗口关闭不再让真窗口被系统/Qt 缩放 | `main/src/frameless_window.cpp` | 点 × 后只看到快照缩小，表盘/按钮不重排、无错乱闪帧。
3. 设置窗关闭改为替身 | `main/src/settings_dialog.cpp` | 点 × 后滚筒/按钮不重排；动画结束才 `reject()`。

## 验收标准

- 默认：Release 编译成功，`_install/main.exe` 更新。
- 截图回归：`-t 1` / `-t 1.5` / `-t 3` 仍严格 400×400、退出码 0（主窗口静态画面零回归）。
- 代码审查：
  - `SettingsDialog::reject()` 不再对 `this` 做 `QPropertyAnimation(..., "geometry")`。
  - `hideToTray()` 在 `hide()` 之前已让真窗口不可见且几何未改；ghost 在真窗口消失前已 `show()`。
  - 设置窗动画期间不 `hide()`/`setVisible(false)` 真 Dialog。
- 无参启动 5 秒不崩溃。
- 人工验证项：主窗口点 ×、设置窗点 ×，动画过程中布局不错乱、无「先闪一下再缩小」。

## 风险与未知

- Windows DWM 隐藏动画是否仍插一帧：用透明度 + 先盖 ghost 双保险；若仍闪，再加 `DWMWA_TRANSITIONS_FORCEDISABLED`。
- 高 DPI 下 `grab()` 与 ghost `paintEvent` 已有 DPR 归一化，保持现有写法。

## 派工策略

- 执行 Agent：delivery-implementer（回退 generalPurpose）
- 测试 Agent：delivery-tester（回退 shell）
- 排查 Agent：delivery-triage（回退 explore + generalPurpose）
- 最大测试-修复轮次：5

## 对齐状态

- [x] 用户已确认（需求原文即「修复」，关闭闪烁根因已定位，无歧义项），可以执行
