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
