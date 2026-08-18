#include "focus_timer_widget.h"

#include <QPainter>
#include <QMouseEvent>
#include <QtMath>

FocusTimerWidget::FocusTimerWidget(QWidget *parent)
    : QWidget(parent)
    , m_activeColor(0x55, 0xB2, 0xE8)
    , m_inactiveColor(0x3D, 0x3D, 0x3D)
    , m_bgColor(0x2B, 0x2B, 0x2B)
    , m_isRunning(true)
    , m_state(0)
    , m_elapsed(0.0)
    , m_workDuration(10.0)
    , m_restDuration(3.0)
{
    setFixedSize(100, 100);

    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &FocusTimerWidget::updateTimer);
    m_timer->start(100);
}

void FocusTimerWidget::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.fillRect(rect(), m_bgColor);

    drawRing(p);
    drawButton(p);
}

void FocusTimerWidget::mousePressEvent(QMouseEvent *event)
{
    if (buttonRect().contains(event->pos())) {
        m_isRunning = !m_isRunning;
        update();
    }
}

void FocusTimerWidget::drawRing(QPainter &p)
{
    constexpr int count = 36;
    constexpr double radius = 28.0;
    constexpr QPoint center(50, 40);
    constexpr double startAngle = 0.0; // 3 点钟方向

    double progress = 0.0;
    if (m_state == 0) { // 工作阶段
        progress = qMin(1.0, m_elapsed / m_workDuration);
    }

    int activeCount = static_cast<int>(progress * count + 0.5);

    for (int i = 0; i < count; ++i) {
        double angle = startAngle + (360.0 / count) * i;
        double rad = qDegreesToRadians(angle);
        int x = center.x() + static_cast<int>(radius * std::cos(rad));
        int y = center.y() + static_cast<int>(radius * std::sin(rad));

        p.save();
        p.translate(x, y);
        p.rotate(angle + 90);
        p.setPen(Qt::NoPen);
        p.setBrush(i < activeCount ? m_activeColor : m_inactiveColor);
        p.drawRoundedRect(-2, -4, 4, 8, 2, 2);
        p.restore();
    }
}

void FocusTimerWidget::drawButton(QPainter &p)
{
    constexpr QPoint center(50, 78);
    constexpr int radius = 14;

    p.setPen(Qt::NoPen);
    p.setBrush(m_activeColor);
    p.drawEllipse(center, radius, radius);

    if (m_isRunning) {
        // 暂停图标：两条竖线
        p.setPen(QPen(Qt::black, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        p.drawLine(center.x() - 3, center.y() - 4, center.x() - 3, center.y() + 4);
        p.drawLine(center.x() + 3, center.y() - 4, center.x() + 3, center.y() + 4);
    } else {
        // 播放图标：三角形
        QPoint points[3] = {
            QPoint(center.x() + 4, center.y()),
            QPoint(center.x() - 3, center.y() - 4),
            QPoint(center.x() - 3, center.y() + 4)
        };
        p.setBrush(Qt::black);
        p.drawPolygon(points, 3);
    }
}

QRect FocusTimerWidget::buttonRect() const
{
    return QRect(36, 64, 28, 28);
}

void FocusTimerWidget::updateTimer()
{
    if (!m_isRunning) {
        update();
        return;
    }

    m_elapsed += 0.1;

    if (m_state == 0) {
        if (m_elapsed >= m_workDuration) {
            m_state = 1;
            m_elapsed = 0.0;
        }
    } else {
        if (m_elapsed >= m_restDuration) {
            m_state = 0;
            m_elapsed = 0.0;
        }
    }

    update();
}
