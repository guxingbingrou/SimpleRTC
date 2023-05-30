#include "mainwidget.h"
#include "ui_mainwidget.h"
#include "SessionDescription/simplesessiondescription.h"
#include "SessionDescription/SessionDesciptionOption.h"
#include <QtDebug>

MainWidget::MainWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MainWidget)
{
    ui->setupUi(this);
    std::unique_ptr<SimpleSessionDescription> sdp = std::make_unique<SimpleSessionDescription>();

    SessionDescriptionOption option;

    sdp->GenerateMediaDescriptions(&option);

    qDebug() << QString::fromStdString( sdp->SdpSerilalize() );

}

MainWidget::~MainWidget()
{

    delete ui;
}


