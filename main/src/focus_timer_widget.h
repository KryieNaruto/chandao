#pragma once

#include <QWidget>
#include <QColor>
#include <QTimer>

class FocusTimerWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FocusTimerWidget(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private slots:
    void updateTimer();

private:
    void drawRing(QPainter &p);
    void drawButton(QPainter &p);
    QRect buttonRect() const;

    QTimer *m_timer;
    QColor m_activeColor;
    QColor m_inactiveColor;
    QColor m_bgColor;
    bool m_isRunning;
    int m_state; // 0: 工作, 1: 休息
    double m_elapsed;
    double m_workDuration;
    double m_restDuration;
};
