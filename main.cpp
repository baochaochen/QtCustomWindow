#include "CustomMainWindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    CustomMainWindow w;
    w.show();

    return a.exec();
}
