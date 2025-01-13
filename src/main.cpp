#include <QApplication>
#include <QStyleFactory>
#include "EQMinecraftFishingBot.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    EQMinecraftFishingBot w;
    w.show();
    return QApplication::exec();
}