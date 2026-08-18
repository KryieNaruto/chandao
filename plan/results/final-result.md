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

## 后续建议

1. 下载安装 Qt 6.8.3 MSVC 2022_64 到其他机器时，可调整 `-DCMAKE_PREFIX_PATH` 指向实际路径。
2. 业务功能开发在 `main/src/` 中展开；运行时资源放入 `main/resource/`。
3. 需要 Vulkan 时安装 Vulkan SDK，CMake 会自动启用 `HAS_VULKAN`。
4. 使用 `/software-director-delivery` Skill 管理后续新需求。
