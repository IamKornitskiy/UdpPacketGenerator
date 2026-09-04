#pragma once

#include <QResizeEvent>
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

    int getXAddress();
    int getXSource();
    int getXDest();
    int getXInterval();

protected:
    Ui::StreamConfigWidget *ui;

signals:
};
