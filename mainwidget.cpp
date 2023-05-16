#include "mainwidget.h"
#include "ui_mainwidget.h"

#include <QtDebug>
#include <thread>
#include <QPainter>
MainWidget::MainWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MainWidget)
{
    ui->setupUi(this);
    connect(this, &MainWidget::signalDraw, this, &MainWidget::slotDraw);

    mVideoDecoder = VideoDecoder::CreateVideoDecoder(SimpleH264);

    mVideoDecoder->InitVideoDecoder(MainWidget::SaveFrame, this);


    mThread = std::thread(&MainWidget::ReadH264AndDecode, this, "record1.h264", "record1.yuv");
//    ReadH264AndDecode("record1.h264",
//                      "record1.yuv");
}

MainWidget::~MainWidget()
{
    mRunning = false;
    mThread.join();
    delete ui;
}

void MainWidget::ReadH264AndDecode(const QString srcPath, const QString destPath)
{
    FILE* f_src = fopen(srcPath.toUtf8().data(), "rb");
    if(!f_src){
        qDebug() << "open file faild : " << srcPath << strerror(errno);
        return;
    }

//    f_dest = fopen(destPath.toUtf8().data(), "wb");
//    if(!f_dest){
//        qDebug() << "open file faild : " << destPath;
//        return;
//    }

    size_t size = 3 * 1024 * 1024;
    srcData = static_cast<uint8_t*>(malloc(3 * 1024 * 1024));

    mRunning = true;

    while(mRunning){
        size_t real_size = fread(srcData, 1, size, f_src);
        if(real_size <= 0){
            break;
        }
        mVideoDecoder->DecodePacket(srcData, real_size);
    }
}

void MainWidget::SaveFrame(void *user, uint8_t *data, int width, int height, FrameFormat format)
{
//    qDebug() << "save frame, wxh: " << width << "x" << height << " format: " << format;

    auto handle = static_cast<MainWidget*>(user);
//    fwrite(data, 1, width*height*3/2, handle->f_dest);

    QImage image(data, width, height, QImage::Format_RGB32);
    handle->mImage = image;
    emit handle->signalDraw();

    std::this_thread::sleep_for(std::chrono::milliseconds(25));
}

void MainWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setBrush(Qt::black);
    painter.drawRect(0, 0, this->width(), this->height());

    if(mImage.width() < 0)
        return;

    QImage image = mImage.scaled(this->size(), Qt::KeepAspectRatio);
    int x = this->width() - image.width();
    int y = this->height() - image.height();

    x /= 2;
    y /= 2;
    painter.drawImage(QPoint(x,y), image);
}


void MainWidget::slotDraw(){
    update();
}

