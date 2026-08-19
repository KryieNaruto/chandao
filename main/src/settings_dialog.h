#pragma once

#include <QDialog>

class WheelPicker;

// 设置弹窗：无边框圆角深色对话框，设置专注时间与休息时间（HH:MM:SS）。
// 时间为竖向滚筒选择器（可按住拖动滚动、松手吸附、支持滚轮）。
// 右上角自绘 × 关闭（取消不保存，reject）；中间偏下「确定」按钮 accept。
// 顶部标题区按住可拖拽移动；弹出时带淡入 + 下滑动画。
class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(double workSec, double restSec, QWidget *parent = nullptr);

    // 仅在 accept 后有意义：返回设置的时长（秒）
    double workSeconds() const;
    double restSeconds() const;

protected:
    void paintEvent(QPaintEvent *event) override;
    void showEvent(QShowEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    QRect closeButtonRect() const;
    QRect titleRect() const;

    WheelPicker *m_workH;
    WheelPicker *m_workM;
    WheelPicker *m_workS;
    WheelPicker *m_restH;
    WheelPicker *m_restM;
    WheelPicker *m_restS;

    bool m_moving = false;
    QPoint m_moveOffset;
};
