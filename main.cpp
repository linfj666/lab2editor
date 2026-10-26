#include "mainwindow.h"

#include <QApplication>
#include <QLibraryInfo>
#include <QLocale>
#include <QTranslator>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QApplication::setApplicationName(QStringLiteral("lab2editor"));
    QApplication::setOrganizationName(QStringLiteral("DGUT"));

    // 载入 Qt 自带的中文翻译，让 QMessageBox / QFileDialog 的标准按钮显示“保存/不保存/取消”。
    // 找不到 .qm 时静默退化为英文，不影响功能。
    QTranslator qtTranslator;
    const QString qmName = QStringLiteral("qtbase_zh_CN");
    const bool loaded =
        qtTranslator.load(QLocale(QLocale::Chinese, QLocale::China), qmName,
                          QStringLiteral("_"),
                          QLibraryInfo::path(QLibraryInfo::TranslationsPath))
        || qtTranslator.load(qmName,
                             QCoreApplication::applicationDirPath() + QStringLiteral("/translations"));
    if (loaded)
        app.installTranslator(&qtTranslator);

    MainWindow w;
    w.show();
    return QApplication::exec();
}
