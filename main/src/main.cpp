#include <QApplication>
#include <QLabel>
#include <QMainWindow>

#ifdef HAS_VULKAN
#include <vulkan/vulkan.h>
#endif

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QMainWindow window;
    window.setWindowTitle(QStringLiteral("禅道"));
    window.resize(720, 480);

#ifdef HAS_VULKAN
    uint32_t apiVersion = 0;
    QString vulkanText = QStringLiteral("Vulkan: 已启用");
    if (vkEnumerateInstanceVersion(&apiVersion) == VK_SUCCESS) {
        vulkanText += QStringLiteral("  API %1.%2.%3")
                          .arg(VK_API_VERSION_MAJOR(apiVersion))
                          .arg(VK_API_VERSION_MINOR(apiVersion))
                          .arg(VK_API_VERSION_PATCH(apiVersion));
    }
#else
    const QString vulkanText = QStringLiteral("Vulkan: 未启用（本机无 SDK 时仍可编译运行）");
#endif

    auto *label = new QLabel(
        QStringLiteral("禅道\n%1").arg(vulkanText),
        &window);
    label->setAlignment(Qt::AlignCenter);
    window.setCentralWidget(label);

    window.show();
    return app.exec();
}
