#include "dialog.h"
#include <QApplication>
#include <QFont>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Dialog w;
    w.show();

    QFont font;
    font.setFamily("楷体"); // Tahoma 宋体
    font.setPointSize(14);
    qApp->setFont(font);

    QFile qssFile(":/sytle.qss");
    qssFile.open(QFile::ReadOnly);
    qApp->setStyleSheet(QLatin1String(qssFile.readAll()));
    qssFile.close();

    return a.exec();
}
