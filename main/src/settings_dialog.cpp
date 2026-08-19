#include "settings_dialog.h"

#include <QTimeEdit>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPainter>
#include <QMouseEvent>
#include <QTime>

SettingsDialog::SettingsDialog(double workSec, double restSec, QWidget *parent)
    : QDialog(parent, Qt::Dialog | Qt::FramelessWindowHint)
{
    setFixedSize(320, 220);
    setStyleSheet(QStringLiteral(
        "QDialog { background: #2B2B2B; }"
        "QLabel { color: #E8E8E8; background: transparent; }"
        "QTimeEdit { background: #3D3D3D; color: #E8E8E8; border: none;"
        "            padding: 4px 8px; selection-background-color: #55B2E8; }"
        "QTimeEdit::up-button, QTimeEdit::down-button { width: 16px; background: #3D3D3D; }"
        "QPushButton { background: #55B2E8; color: #FFFFFF; border: none;"
        "              padding: 8px 32px; }"
        "QPushButton:hover { background: #6CC0F0; }"
        "QPushButton:pressed { background: #4A9FD4; }"));

    QLabel *titleLabel = new QLabel(QStringLiteral("设置"), this);
    titleLabel->setAlignment(Qt::AlignCenter);

    const auto secsToTime = [](double secs) {
        const int s = qBound(0, static_cast<int>(secs), 86399); // QTimeEdit 上限 23:59:59
        return QTime(0, 0, 0).addSecs(s);
    };

    m_workEdit = new QTimeEdit(secsToTime(workSec), this);
    m_workEdit->setDisplayFormat(QStringLiteral("HH:mm:ss"));
    m_restEdit = new QTimeEdit(secsToTime(restSec), this);
    m_restEdit->setDisplayFormat(QStringLiteral("HH:mm:ss"));

    QHBoxLayout *workRow = new QHBoxLayout;
    workRow->addWidget(new QLabel(QStringLiteral("专注时间"), this));
    workRow->addStretch();
    workRow->addWidget(m_workEdit);

    QHBoxLayout *restRow = new QHBoxLayout;
    restRow->addWidget(new QLabel(QStringLiteral("休息时间"), this));
    restRow->addStretch();
    restRow->addWidget(m_restEdit);

    QPushButton *okButton = new QPushButton(QStringLiteral("确定"), this);
    connect(okButton, &QPushButton::clicked, this, &QDialog::accept);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(24, 12, 24, 20);
    layout->addWidget(titleLabel);
    layout->addSpacing(12);
    layout->addLayout(workRow);
    layout->addSpacing(8);
    layout->addLayout(restRow);
    layout->addStretch(); // 「确定」落在中间偏下
    layout->addWidget(okButton, 0, Qt::AlignHCenter);

    if (parent) {
        // 弹出时居中于父窗口
        const QRect pg = parent->window()->geometry();
        move(pg.center() - rect().center());
    }
}

double SettingsDialog::workSeconds() const
{
    return QTime(0, 0, 0).secsTo(m_workEdit->time());
}

double SettingsDialog::restSeconds() const
{
    return QTime(0, 0, 0).secsTo(m_restEdit->time());
}

QRect SettingsDialog::closeButtonRect() const
{
    const int d = 24;
    const int m = 8;
    return QRect(width() - m - d, m, d, d);
}

QRect SettingsDialog::titleRect() const
{
    // 顶部标题区（不含右上角 × 热区）
    return QRect(0, 0, closeButtonRect().left(), 40);
}

void SettingsDialog::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.fillRect(rect(), QColor(0x2B, 0x2B, 0x2B));

    // 右上角自绘 ×
    const QPointF c = closeButtonRect().center();
    const double d = 6.0;
    p.setPen(QPen(QColor(0xE8, 0xE8, 0xE8), 1.6, Qt::SolidLine, Qt::RoundCap));
    p.drawLine(QPointF(c.x() - d, c.y() - d), QPointF(c.x() + d, c.y() + d));
    p.drawLine(QPointF(c.x() - d, c.y() + d), QPointF(c.x() + d, c.y() - d));
}

void SettingsDialog::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        if (closeButtonRect().contains(event->pos())) {
            reject(); // × 关闭：取消不保存
            return;
        }
        if (titleRect().contains(event->pos())) {
            m_moving = true;
            m_moveOffset = event->globalPosition().toPoint() - frameGeometry().topLeft();
            return;
        }
    }
    QDialog::mousePressEvent(event);
}

void SettingsDialog::mouseMoveEvent(QMouseEvent *event)
{
    if (m_moving) {
        move(event->globalPosition().toPoint() - m_moveOffset);
        return;
    }
    QDialog::mouseMoveEvent(event);
}

void SettingsDialog::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_moving = false;
    }
    QDialog::mouseReleaseEvent(event);
}
