#include <QApplication>
#include <QPixmap>
#include <QImage>
#include <QStringList>
#include "focus_timer_widget.h"
#include "frameless_window.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // 命令行解析：-t <秒数>（支持小数）输出该时刻截图；-s <边长> 控制截图尺寸（默认 400）
    const QStringList args = app.arguments();
    QString timeArg;
    int shotSize = 400;
    for (int i = 1; i < args.size(); ++i) {
        if (args.at(i) == QStringLiteral("-t") && i + 1 < args.size()) {
            timeArg = args.at(i + 1);
            ++i;
        } else if (args.at(i) == QStringLiteral("-s") && i + 1 < args.size()) {
            shotSize = args.at(i + 1).toInt();
            ++i;
        }
    }

    if (!timeArg.isEmpty()) {
        bool ok = false;
        const double t = timeArg.toDouble(&ok);
        if (!ok || shotSize <= 0) {
            return 1;
        }

        FocusTimerWidget widget;
        widget.resize(shotSize, shotSize);
        widget.setTime(t);

        const QString fileName = QStringLiteral("shot_%1.png").arg(timeArg);
        QPixmap pixmap = widget.grab();
        QImage image = pixmap.toImage();
        // 高 DPI 下 grab() 返回物理像素，需归一化到 -s 指定的逻辑尺寸
        if (image.size() != QSize(shotSize, shotSize)) {
            image = image.scaled(shotSize, shotSize,
                                 Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        }
        if (!image.save(fileName, "PNG")) {
            return 1;
        }
        return 0;
    }

    // 交互模式：无边框窗口（尺寸/居中在 FramelessWindow 构造中完成）
    FramelessWindow window;
    window.setWindowTitle(QStringLiteral("专注"));
    window.show();
    return app.exec();
}
