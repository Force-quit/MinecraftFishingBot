#include <QApplication>
#include <QStyleFactory>
#include "../Headers/EQMinecraftFishingBot.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QApplication::setStyle(QStyleFactory::create("Fusion"));
    EQMinecraftFishingBot w(QCoreApplication::applicationDirPath());
    w.show();
    return a.exec();
}