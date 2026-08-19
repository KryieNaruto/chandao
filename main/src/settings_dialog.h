#pragma once

#include <QDialog>

class QTimeEdit;

// 设置弹窗：无边框深色对话框，设置专注时间与休息时间（HH:MM:SS）。
// 右上角自绘 × 关闭（取消不保存，reject）；中间偏下「确定」按钮 accept。
// 顶部标题区按住可拖拽移动。
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
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    QRect closeButtonRect() const;
    QRect titleRect() const;

    QTimeEdit *m_workEdit;
    QTimeEdit *m_restEdit;

    bool m_moving = false;
    QPoint m_moveOffset;
};
