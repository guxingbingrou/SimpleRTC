#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include <thread>
#include "VideoDecoder/videodecoder.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWidget; }
QT_END_NAMESPACE

class MainWidget : public QWidget
{
    Q_OBJECT

public:
    MainWidget(QWidget *parent = nullptr);
    ~MainWidget();

private:
    Ui::MainWidget *ui;
    std::shared_ptr<VideoDecoder> mVideoDecoder;

    void ReadH264AndDecode(const QString srcPath, const QString destPath);
    static void SaveFrame(void* user, uint8_t* data, int width, int height, FrameFormat format);

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
#endif // MAINWIDGET_H
