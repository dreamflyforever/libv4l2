#include "widget.h"
#include <QApplication>
#include "color.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget w;
    initLut();

    w.show();

    return a.exec();
}
