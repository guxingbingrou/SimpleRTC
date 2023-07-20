#include "mainwidget.h"
#include "testvideodecoder.h"
#include "Utils/Logger.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Logger::GetInstance()->InitLogger();
    Logger::GetInstance()->SetLogLevel(kDebug);


//    TestVideoDecoder testVideoDecoder;
//    testVideoDecoder.show();

    MainWidget w;
    w.show();
    return a.exec();
}
