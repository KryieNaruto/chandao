#include <QApplication>
#include <QMainWindow>
#include "focus_timer_widget.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QMainWindow window;
    window.setWindowTitle(QStringLiteral("专注"));

    auto *timer = new FocusTimerWidget(&window);
    window.setCentralWidget(timer);

    window.adjustSize();
    window.setFixedSize(window.size());

    window.show();
    return app.exec();
}
