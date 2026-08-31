#pragma once

#include <QWidget>

namespace Ui {
class StreamConfigWidget;
}

class StreamConfigWidget : public QWidget
{
    Q_OBJECT

public:
    explicit StreamConfigWidget(QWidget *parent = nullptr);
    ~StreamConfigWidget();

private:
    Ui::StreamConfigWidget *ui;
};
