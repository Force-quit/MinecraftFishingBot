#include <QApplication>
#include <QStyleFactory>
#include "EQTests.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QApplication::setStyle(QStyleFactory::create("Fusion"));
    EQTests w;
    w.show();
    return a.exec();
}