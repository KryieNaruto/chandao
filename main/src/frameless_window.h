#pragma once

#include <QMainWindow>

class FocusTimerWidget;
class QSystemTrayIcon;

// 无边框主窗口：背景与表盘同色，右上角关闭按钮由中央控件绘制，
// 空白区域按住拖拽移动，边缘 6px 热区支持 8 方向缩放（纯 Qt 实现）。
// 全程保持正方形（以短边为准回正），最小 200×200。
// 点击关闭按钮隐藏到系统托盘，托盘右键菜单「退出」才真正关闭；
// 工作→休息切换时若窗口处于托盘隐藏状态则恢复显示，并触发任务栏闪烁提醒。
class FramelessWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit FramelessWindow(QWidget *parent = nullptr);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    bool event(QEvent *event) override;

private:
    int edgeHit(const QPoint &pos) const;
    void updateCursor(const QPoint &pos);
    void applyResize(const QPoint &globalPos);
    void setupTray();
    void setupRestAlert();

    FocusTimerWidget *m_timerWidget;
    QSystemTrayIcon *m_trayIcon = nullptr;

    static constexpr int kEdgeMargin = 6; // 边缘缩放热区宽度（像素）

    // 缩放方向位标志
    enum Edge {
        EdgeNone   = 0,
        EdgeLeft   = 1,
        EdgeTop    = 2,
        EdgeRight  = 4,
        EdgeBottom = 8
    };

    bool m_moving = false;   // 拖拽移动中
    int m_resizing = EdgeNone; // 缩放方向（Edge 组合）
    QPoint m_moveOffset;     // 移动时指针相对窗口左上角的偏移
    QRect m_startGeometry;   // 缩放起始几何
    QPoint m_startGlobal;    // 缩放起始全局指针位置
};
