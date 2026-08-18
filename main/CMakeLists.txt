cmake_minimum_required(VERSION 3.21)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

add_executable(main WIN32
    src/main.cpp
    src/focus_timer_widget.h
    src/focus_timer_widget.cpp
)

target_link_libraries(main PRIVATE Qt6::Widgets)

# Visual Studio 调试时自动把 Qt 的 bin 目录加入 PATH，避免提示缺少 Qt DLL
if(WIN32 AND MSVC)
    set_property(TARGET main PROPERTY VS_DEBUGGER_ENVIRONMENT "PATH=$<TARGET_FILE_DIR:Qt6::Core>;$(PATH)")
endif()

# Vulkan 为可选：有 SDK 则启用，没有也不要导致配置/编译失败
find_package(Vulkan QUIET)
if(Vulkan_FOUND)
    message(STATUS "Vulkan found: ${Vulkan_LIBRARY}")
    target_compile_definitions(main PRIVATE HAS_VULKAN)
    target_link_libraries(main PRIVATE Vulkan::Vulkan)
else()
    message(STATUS "Vulkan SDK not found; building without HAS_VULKAN")
endif()

# 安装到前缀根目录，使验收能找到 _install/main.exe
install(TARGETS main RUNTIME DESTINATION .)

# Windows：安装后用 windeployqt 拷贝 Qt 运行依赖
if(WIN32)
    get_target_property(_qmake_executable Qt6::qmake IMPORTED_LOCATION)
    cmake_path(GET _qmake_executable PARENT_PATH _qt_bin_dir)
    find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS "${_qt_bin_dir}" REQUIRED)

    install(CODE "
        set(_exe \"\$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/main.exe\")
        message(STATUS \"Running windeployqt on \${_exe}\")
        execute_process(
            COMMAND \"${WINDEPLOYQT_EXECUTABLE}\" --release --no-compiler-runtime --no-translations \"\${_exe}\"
            RESULT_VARIABLE _wd_result
        )
        if(NOT _wd_result EQUAL 0)
            message(FATAL_ERROR \"windeployqt failed with code \${_wd_result}\")
        endif()
    ")
endif()
