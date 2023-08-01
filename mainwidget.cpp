#include "mainwidget.h"
#include "ui_mainwidget.h"
#include "SessionDescription/SessionDescriptionFactory.h"
#include "SessionDescription/SessionDescriptionOption.h"
#include <QtDebug>
#include "Utils/Logger.h"

MainWidget::MainWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MainWidget)
{
    ui->setupUi(this);


    std::unique_ptr<SessionDescriptionFactory> sdp_factory = std::make_unique<SessionDescriptionFactory>();

    SessionDescriptionOption option;

    auto offer = sdp_factory->CreateOffer(&option);

    auto sdp_str = offer->ToString();

    DEBUG("sdp111:\n%s", offer->ToString().c_str());

    auto sdp = sdp_factory->SdpDeSerilaize(sdp_str);
    DEBUG("sdp222:\n%s", sdp->ToString().c_str());

}

MainWidget::~MainWidget()
{

    delete ui;
}


