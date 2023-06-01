#include "mainwidget.h"
#include "ui_mainwidget.h"
#include "SessionDescription/simplesessiondescription.h"
#include "SessionDescription/SessionDesciptionOption.h"
#include <QtDebug>
#include "Utils/logger.h"

MainWidget::MainWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MainWidget)
{
    ui->setupUi(this);


    std::unique_ptr<SimpleSessionDescription> sdp = std::make_unique<SimpleSessionDescription>();

    SessionDescriptionOption option;

    sdp->GenerateMediaDescriptions(&option);

    INFO("sdp:\n%s", sdp->SdpSerilalize().c_str());

}

MainWidget::~MainWidget()
{

    delete ui;
}


