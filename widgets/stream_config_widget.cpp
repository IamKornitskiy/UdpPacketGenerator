#include "stream_config_widget.h"
#include "ui_stream_config_widget.h"

StreamConfigWidget::StreamConfigWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::StreamConfigWidget)
{
    ui->setupUi(this);
}

StreamConfigWidget::~StreamConfigWidget()
{
    delete ui;
}

void StreamConfigWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);

    emit sendAddressXPos(ui->address->pos().x());
    emit sendSourcePortXPos(ui->srcPort->pos().x());
    emit sendDestPortXPos(ui->destPort->pos().x());
    emit sendIntervalXPos(ui->interval->pos().x());
}
