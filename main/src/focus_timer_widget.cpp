#include "focus_timer_widget.h"
#include "settings_dialog.h"

#include <QPainter>
#include <QMouseEvent>
#include <QMenu>
#include <QAction>
#include <QSettings>
#include <QtMath>
#include <cmath>

// 各动画在 60fps 下的单帧步长：图标切换约 150ms，悬停/按压取相近量级的快响应
static constexpr double kIconStep  = 0.016 / 0.150;
static constexpr double kHoverStep = 0.016 / 0.120;
static constexpr double kPressStep = 0.016 / 0.060;

FocusTimerWidget::FocusTimerWidget(QWidget *parent)
    : QWidget(parent)
    , m_activeColor(0x55, 0xB2, 0xE8)
    , m_inactiveColor(0x3D, 0x3D, 0x3D)
    , m_bgColor(0x2B, 0x2B, 0x2B)
    , m_isRunning(true)
    , m_state(0)
    , m_elapsed(0.0)
    , m_workDuration(10.0)
    , m_restDuration(10.0)
{
    setMouseTracking(true); // 悬停提亮需要不按下也能收到移动事件
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &FocusTimerWidget::updateTimer);
    m_timer->start(16); // 约 60fps
    connect(this, &FocusTimerWidget::dotsClicked, this, &FocusTimerWidget::showDotsMenu);
}

void FocusTimerWidget::setTime(double seconds)
{
    m_timer->stop();
    m_timerStopped = true;
    m_isRunning = true;

    const double cycle = m_workDuration + m_restDuration;
    const double rem = std::fmod(seconds, cycle);
    if (rem < m_workDuration) {
        m_state = 0;
        m_elapsed = rem;
    } else {
        m_state = 1;
        m_elapsed = rem - m_workDuration;
    }
    update();
}

bool FocusTimerWidget::closeButtonHit(const QPoint &pos) const
{
    return closeButtonRect().contains(pos);
}

bool FocusTimerWidget::playButtonHit(const QPoint &pos) const
{
    return buttonRect().contains(pos);
}

bool FocusTimerWidget::dotsButtonHit(const QPoint &pos) const
{
    return dotsButtonRect().contains(pos);
}

void FocusTimerWidget::setDurations(double workSec, double restSec)
{
    if (workSec <= 0.0 || restSec <= 0.0) {
        return;
    }
    m_workDuration = workSec;
    m_restDuration = restSec;

    const double duration = (m_state == 0) ? m_workDuration : m_restDuration;
    if (m_elapsed >= duration) {
        // 新时长已小于等于已用时长：当前阶段立即结束，进入下一阶段
        m_state = 1 - m_state;
        m_elapsed = 0.0;
        if (!m_timerStopped) {
            emit phaseChanged(m_state);
        }
    }
    // 否则续跑：剩余 = 新时长 - 已用（m_elapsed 不动即天然满足）
    update();
}

void FocusTimerWidget::setCenterMode(CenterMode mode)
{
    if (m_centerMode != mode) {
        m_centerMode = mode;
        update();
    }
}

double FocusTimerWidget::remainingSeconds() const
{
    const double duration = (m_state == 0) ? m_workDuration : m_restDuration;
    return qMax(0.0, duration - m_elapsed);
}

QSize FocusTimerWidget::sizeHint() const
{
    return QSize(100, 100);
}

void FocusTimerWidget::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.fillRect(rect(), m_bgColor);

    drawRing(p);
    drawCenterContent(p);
    drawButton(p);
    drawDotsButton(p);
    drawCloseButton(p);
}

void FocusTimerWidget::mousePressEvent(QMouseEvent *event)
{
    if (buttonRect().contains(event->pos())) {
        m_buttonPressed = true;
        update();
    } else if (dotsButtonRect().contains(event->pos())) {
        m_dotsPressed = true;
        update();
    } else {
        // 未命中按钮（含右上角 ×）：交还父窗口处理拖拽/关闭/边缘缩放
        event->ignore();
    }
}

void FocusTimerWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (m_buttonPressed) {
        m_buttonPressed = false;
        if (buttonRect().contains(event->pos())) {
            m_isRunning = !m_isRunning;
        }
        update();
    } else if (m_dotsPressed) {
        m_dotsPressed = false;
        if (dotsButtonRect().contains(event->pos())) {
            emit dotsClicked();
        }
        update();
    } else {
        event->ignore();
    }
}

void FocusTimerWidget::mouseMoveEvent(QMouseEvent *event)
{
    const bool overButton = buttonRect().contains(event->pos());
    const bool overClose = closeButtonRect().contains(event->pos());
    const bool overDots = dotsButtonRect().contains(event->pos());
    if (overButton != m_buttonHovered || overClose != m_closeHovered
        || overDots != m_dotsHovered) {
        m_buttonHovered = overButton;
        m_closeHovered = overClose;
        m_dotsHovered = overDots;
        update();
    }
    if (!overButton && !overClose && !overDots) {
        // 交还父窗口：边缘缩放光标依赖父级收到移动事件
        event->ignore();
    }
}

void FocusTimerWidget::leaveEvent(QEvent *)
{
    if (m_buttonHovered || m_closeHovered || m_dotsHovered) {
        m_buttonHovered = false;
        m_closeHovered = false;
        m_dotsHovered = false;
        update();
    }
}

void FocusTimerWidget::drawRing(QPainter &p)
{
    const TickStyle ts;
    constexpr double startAngle = 0.0; // 3 点钟方向

    const double w = width();
    const double h = height();
    const double base = qMin(w, h);
    const QPointF center(w * 0.5, h * 0.4);
    const double radius = base * ts.ringRadius;
    const double tickW = base * ts.widthRatio;
    const double tickH = base * ts.heightRatio;
    const double tickR = base * ts.cornerRatio;

    double progress = 0.0;
    if (m_state == 0) { // 工作阶段：灰 -> 蓝
        progress = qMin(1.0, m_elapsed / m_workDuration);
    } else { // 休息阶段：蓝 -> 灰
        progress = qMin(1.0, m_elapsed / m_restDuration);
    }

    for (int i = 0; i < ts.count; ++i) {
        double angle = startAngle + (360.0 / ts.count) * i;
        double rad = qDegreesToRadians(angle);
        double x = center.x() + radius * std::cos(rad);
        double y = center.y() + radius * std::sin(rad);

        // 该刻度被进度覆盖的比例（0..1），超出范围自动钳制
        const double cover = qBound(0.0, progress * ts.count - i, 1.0);
        // t 统一表示该刻度的蓝色占比：工作时随进度增大，休息时随进度减小
        const double t = (m_state == 0) ? cover : 1.0 - cover;

        p.save();
        p.translate(x, y);
        // 保持方块朝向不变：长轴仍然指向圆心（径向）
        p.rotate(angle + 90);
        p.setPen(Qt::NoPen);

        // 渐变沿局部 x 轴（即圆环顺时针切线方向），从后侧 (-tickW/2) 到前侧 (+tickW/2)
        QLinearGradient gradient(-tickW * 0.5, 0, tickW * 0.5, 0);
        if (t <= 0.0) {
            gradient.setColorAt(0.0, m_inactiveColor);
            gradient.setColorAt(1.0, m_inactiveColor);
        } else if (t >= 1.0) {
            gradient.setColorAt(0.0, m_activeColor);
            gradient.setColorAt(1.0, m_activeColor);
        } else if (m_state == 0) {
            // 工作：颜色前沿朝顺时针推进，蓝在后侧、灰在前侧
            gradient.setColorAt(0.0, m_activeColor);
            gradient.setColorAt(qMax(0.0, t - ts.frontFade), m_activeColor);
            gradient.setColorAt(qMin(1.0, t + ts.frontFade), m_inactiveColor);
            gradient.setColorAt(1.0, m_inactiveColor);
        } else {
            // 休息：灰的前沿朝顺时针推进，蓝在前侧，前沿渐变方向相应反转
            const double edge = 1.0 - t;
            gradient.setColorAt(0.0, m_inactiveColor);
            gradient.setColorAt(qMax(0.0, edge - ts.frontFade), m_inactiveColor);
            gradient.setColorAt(qMin(1.0, edge + ts.frontFade), m_activeColor);
            gradient.setColorAt(1.0, m_activeColor);
        }

        p.setBrush(gradient);
        p.drawRoundedRect(-tickW * 0.5, -tickH * 0.5, tickW, tickH, tickR, tickR);
        p.restore();
    }
}

void FocusTimerWidget::drawButton(QPainter &p)
{
    const double w = width();
    const double h = height();
    const double base = qMin(w, h);
    const QPointF center(w * 0.5, h * 0.86);
    // 按钮直径 ≈ 窗口边长 10%
    const double radius = base * 0.05;
    // 按压瞬时缩小到约 0.92 倍并随 m_pressT 回弹
    const double scale = 1.0 - 0.08 * m_pressT;
    // 悬停提亮系数
    const double lift = m_hoverT * 0.16;

    // 扁平化纯色背景，仅保留悬停提亮
    const QColor bg = m_activeColor.lighter(static_cast<int>(100 + lift * 100));

    p.save();
    p.translate(center);
    p.scale(scale, scale);
    p.translate(-center);

    p.setPen(Qt::NoPen);
    p.setBrush(bg);
    p.drawEllipse(center, radius, radius);

    // 图标平滑过渡：m_iconT 为 1 时是暂停、0 时是播放，中间按透明度交叉淡化
    if (m_iconT >= 1.0) {
        drawPauseIcon(p, center, radius, 1.0);
    } else if (m_iconT <= 0.0) {
        drawPlayIcon(p, center, radius, 1.0);
    } else {
        drawPauseIcon(p, center, radius, m_iconT);
        drawPlayIcon(p, center, radius, 1.0 - m_iconT);
    }

    p.restore();
}

void FocusTimerWidget::drawPauseIcon(QPainter &p, const QPointF &c, double r, double alpha)
{
    // 图标尺寸按按钮半径等比
    const double gap = r * 0.30;
    const double half = r * 0.42;
    const double lineWidth = qMax(1.0, r * 0.22);

    QColor iconColor(0xFF, 0xFF, 0xFF);
    iconColor.setAlphaF(alpha);

    p.setPen(QPen(iconColor, lineWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    p.setBrush(Qt::NoBrush);
    p.drawLine(c.x() - gap, c.y() - half, c.x() - gap, c.y() + half);
    p.drawLine(c.x() + gap, c.y() - half, c.x() + gap, c.y() + half);
}

void FocusTimerWidget::drawPlayIcon(QPainter &p, const QPointF &c, double r, double alpha)
{
    const double gap = r * 0.30;
    const double half = r * 0.42;

    QColor iconColor(0xFF, 0xFF, 0xFF);
    iconColor.setAlphaF(alpha);

    // 三角形顶点略右移，视觉重心居中
    QPointF points[3] = {
        QPointF(c.x() + gap * 1.3, c.y()),
        QPointF(c.x() - gap, c.y() - half),
        QPointF(c.x() - gap, c.y() + half)
    };
    p.setPen(Qt::NoPen);
    p.setBrush(iconColor);
    p.drawPolygon(points, 3);
}

void FocusTimerWidget::drawDotsButton(QPainter &p)
{
    const double base = qMin(width(), height());
    const QRectF rc = dotsButtonRect();
    const QPointF c = rc.center();
    const double r = rc.width() * 0.5;

    // 悬停提亮：浮现浅灰圆底 + 圆点变亮（复用 m_dotsHoverT 插值）
    if (m_dotsHoverT > 0.0) {
        QColor bg = QColor(0xFF, 0xFF, 0xFF);
        bg.setAlphaF(0.10 * m_dotsHoverT);
        p.setPen(Qt::NoPen);
        p.setBrush(bg);
        p.drawEllipse(c, r, r);
    }

    // 圆点纯白，与播放按钮白色图标风格统一；悬停反馈由上方浅灰圆底承担
    const QColor fg(0xFF, 0xFF, 0xFF);
    const double dotR = qMax(1.0, base * 0.008);
    const double gap = r * 0.55;
    p.setPen(Qt::NoPen);
    p.setBrush(fg);
    for (int i = -1; i <= 1; ++i) {
        p.drawEllipse(QPointF(c.x() + i * gap, c.y()), dotR, dotR);
    }
}

void FocusTimerWidget::showDotsMenu()
{
    QMenu menu(this);
    menu.setStyleSheet(QStringLiteral(
        "QMenu { background: #2B2B2B; color: #E8E8E8; border: 1px solid #3D3D3D; padding: 4px; }"
        "QMenu::item { padding: 6px 24px; }"
        "QMenu::item:selected { background: #55B2E8; color: #FFFFFF; }"));
    QAction *settingsAction = menu.addAction(QStringLiteral("设置"));

    // 弹出在「...」按钮正下方
    const QRect rc = dotsButtonRect();
    const QPoint anchor = mapToGlobal(QPoint(rc.center().x(), rc.bottom() + 4));
    QAction *chosen = menu.exec(anchor);
    if (chosen == settingsAction) {
        openSettingsDialog();
    }
}

void FocusTimerWidget::openSettingsDialog()
{
    SettingsDialog dlg(m_workDuration, m_restDuration, this);
    if (dlg.exec() == QDialog::Accepted) {
        setDurations(dlg.workSeconds(), dlg.restSeconds());
        QSettings settings(QStringLiteral("Chandao"), QStringLiteral("FocusTimer"));
        settings.setValue(QStringLiteral("workSeconds"), m_workDuration);
        settings.setValue(QStringLiteral("restSeconds"), m_restDuration);
    }
}

void FocusTimerWidget::drawCloseButton(QPainter &p)
{
    const double base = qMin(width(), height());
    const QRectF rc = closeButtonRect();
    const QPointF c = rc.center();
    const double r = rc.width() * 0.5;

    // 悬停提亮：浮现浅灰圆底 + × 变亮
    if (m_closeHoverT > 0.0) {
        QColor bg = QColor(0xFF, 0xFF, 0xFF);
        bg.setAlphaF(0.10 * m_closeHoverT);
        p.setPen(Qt::NoPen);
        p.setBrush(bg);
        p.drawEllipse(c, r, r);
    }

    QColor fg = m_inactiveColor.lighter(static_cast<int>(160 + 120 * m_closeHoverT));
    const double d = r * 0.32;
    const double lineWidth = qMax(1.0, base * 0.008);
    p.setPen(QPen(fg, lineWidth, Qt::SolidLine, Qt::RoundCap));
    p.drawLine(c.x() - d, c.y() - d, c.x() + d, c.y() + d);
    p.drawLine(c.x() - d, c.y() + d, c.x() + d, c.y() - d);
}

QRect FocusTimerWidget::buttonRect() const
{
    const double w = width();
    const double h = height();
    const double base = qMin(w, h);
    const double radius = base * 0.05;
    const QPointF center(w * 0.5, h * 0.86);
    return QRect(static_cast<int>(center.x() - radius),
                 static_cast<int>(center.y() - radius),
                 static_cast<int>(radius * 2),
                 static_cast<int>(radius * 2));
}

QRect FocusTimerWidget::dotsButtonRect() const
{
    const double w = width();
    const double h = height();
    const double base = qMin(w, h);
    const double radius = base * 0.04;
    // 与播放按钮同一水平线，位于其右侧
    const QPointF center(w * 0.5 + base * 0.11, h * 0.86);
    return QRect(static_cast<int>(center.x() - radius),
                 static_cast<int>(center.y() - radius),
                 static_cast<int>(radius * 2),
                 static_cast<int>(radius * 2));
}

QRect FocusTimerWidget::closeButtonRect() const
{
    const double base = qMin(width(), height());
    const double d = base * 0.07;
    const double m = base * 0.04;
    // 右上角
    return QRect(static_cast<int>(width() - m - d), static_cast<int>(m),
                 static_cast<int>(d), static_cast<int>(d));
}

void FocusTimerWidget::drawCenterContent(QPainter &p)
{
    switch (m_centerMode) {
    case CenterMode::TimeText:
        drawCenterTimeText(p);
        break;
    case CenterMode::Plant:
    default:
        // 植物模式预留
        break;
    }
}

void FocusTimerWidget::drawCenterTimeText(QPainter &p)
{
    const double base = qMin(width(), height());
    // 与 drawRing 同一圆心，视觉居中于圆环内
    const QPointF center(width() * 0.5, height() * 0.4);

    // 剩余秒数向上取整（减去微小量避免浮点误差多跳一格）
    const int secs = qMax(0, static_cast<int>(std::ceil(remainingSeconds() - 1e-9)));
    const int hh = secs / 3600;
    const int mm = (secs % 3600) / 60;
    const int ss = secs % 60;
    const QString text = QStringLiteral("%1:%2:%3")
        .arg(hh, 2, 10, QLatin1Char('0'))
        .arg(mm, 2, 10, QLatin1Char('0'))
        .arg(ss, 2, 10, QLatin1Char('0'));

    QFont font = p.font();
    // HH:MM:SS 共 8 字符，字号比纯秒数显示缩小以适配圆环内宽
    font.setPixelSize(static_cast<int>(base * 0.075));
    font.setBold(true);
    p.setFont(font);
    // 工作阶段亮色，休息阶段与圆环同蓝
    p.setPen(m_state == 0 ? QColor(0xE8, 0xE8, 0xE8) : m_activeColor);

    const double half = base * 0.22;
    p.drawText(QRectF(center.x() - half, center.y() - half, half * 2, half * 2),
               Qt::AlignCenter, text);
}

double FocusTimerWidget::approach(double value, double target, double step)
{
    if (value < target) {
        return qMin(value + step, target);
    }
    return qMax(value - step, target);
}

void FocusTimerWidget::updateTimer()
{
    if (m_isRunning) {
        m_elapsed += 0.016;

        if (m_state == 0) {
            if (m_elapsed >= m_workDuration) {
                m_state = 1;
                m_elapsed = 0.0;
                emit phaseChanged(m_state);
            }
        } else {
            if (m_elapsed >= m_restDuration) {
                m_state = 0;
                m_elapsed = 0.0;
                emit phaseChanged(m_state);
            }
        }
    }

    // 截图模式（setTime 停表后）不再推进动画，保证画面确定
    if (!m_timerStopped) {
        m_hoverT = approach(m_hoverT, m_buttonHovered ? 1.0 : 0.0, kHoverStep);
        m_pressT = approach(m_pressT, m_buttonPressed ? 1.0 : 0.0, kPressStep);
        m_iconT = approach(m_iconT, m_isRunning ? 1.0 : 0.0, kIconStep);
        m_closeHoverT = approach(m_closeHoverT, m_closeHovered ? 1.0 : 0.0, kHoverStep);
        m_dotsHoverT = approach(m_dotsHoverT, m_dotsHovered ? 1.0 : 0.0, kHoverStep);
    }

    update();
}
