#include <QApplication>
#include <QStyleFactory>
#include "../Headers/EQMinecraftFishingBot.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QApplication::setStyle(QStyleFactory::create("Fusion"));
    EQMinecraftFishingBot w;
    w.show();
    return a.exec();
}