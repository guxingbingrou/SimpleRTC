#include "mainwidget.h"
#include "testvideodecoder.h"
#include "Utils/logger.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Logger::GetInstance()->InitLogger();
    Logger::GetInstance()->SetLogLevel(kInfo);


//    TestVideoDecoder testVideoDecoder;
//    testVideoDecoder.show();

    MainWidget w;
    w.show();
    return a.exec();
}
