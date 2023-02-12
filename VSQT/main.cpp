#include <QApplication>
#include "../Qt/InputRecorder/EQInputRecorder.h"

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);
    EQInputRecorder w;
    w.show();
    return a.exec();
}