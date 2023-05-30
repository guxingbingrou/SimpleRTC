#include "mainwidget.h"
#include "testvideodecoder.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

//    TestVideoDecoder testVideoDecoder;
//    testVideoDecoder.show();

    MainWidget w;
    w.show();
    return a.exec();
}
