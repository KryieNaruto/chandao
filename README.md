# 禅道

最小可运行的 Qt 桌面窗口项目（CMake + MSVC）。解决方案 / 可执行目标名为 `main`。

## 目录

- `main/`：源码与子工程
- `plan/`：计划文档
- `_build/`：CMake 构建目录（不入库）
- `_install/`：安装输出，验收目标为 `_install/main.exe`（不入库）

## 构建（Windows PowerShell）

将 `<Qt路径>` 换成本机 Qt 套件目录（例如 `D:\Qt\6.8.3\msvc2022_64`）：

```powershell
cmake -S . -B _build -G "Visual Studio 18 2026" -A x64 `
  -DCMAKE_INSTALL_PREFIX="_install" `
  -DCMAKE_PREFIX_PATH="<Qt路径>"

cmake --build _build --config Release
cmake --install _build --config Release
```

安装阶段会调用 `windeployqt`，把 Qt DLL 拷到 `_install`。

用 Visual Studio 打开 `_build/main.sln`，或直接打开根目录 `CMakeLists.txt`。

## 远端

（推送成功后填写 clone 地址）
