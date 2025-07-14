#include <QApplication>
#include <QStyleFactory>
#include "EQMinecraftFishingBot.h"
#include <QTranslator>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
	
    QApplication::setStyle(QStyleFactory::create("Fusion"));

    QTranslator translator;
    if (translator.load(QLocale(), "", "", "resources", ".qm"))
    {
        a.installTranslator(&translator);
    }

    EQMinecraftFishingBot w;
    w.show();
    return QApplication::exec();
}