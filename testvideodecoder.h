#ifndef TESTVIDEODECODER_H
#define TESTVIDEODECODER_H

#include <QWidget>
#include <thread>
#include "VideoDecoder/videodecoder.h"

namespace Ui {
class TestVideoDecoder;
}

class TestVideoDecoder : public QWidget
{
    Q_OBJECT

public:
    explicit TestVideoDecoder(QWidget *parent = nullptr);
    ~TestVideoDecoder();

private:
    Ui::TestVideoDecoder *ui;
    std::shared_ptr<VideoDecoder> mVideoDecoder;

    void ReadH264AndDecode(const QString srcPath, const QString destPath);
    static void ProcessFrame(void* user, uint8_t* data, int width, int height, FrameFormat format);

    uint8_t* srcData = nullptr;
    //FILE* f_dest = nullptr;
    QImage mImage;
    std::thread mThread;
    bool mRunning = false;

protected:
    void paintEvent(QPaintEvent *event) override;

public slots:
    void slotDraw();

signals:
    void signalDraw();
};

#endif // TESTVIDEODECODER_H
