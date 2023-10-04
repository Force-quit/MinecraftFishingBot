#include <QApplication>
#include "EQTests.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    EQTests w;
    w.show();
    return a.exec();
}