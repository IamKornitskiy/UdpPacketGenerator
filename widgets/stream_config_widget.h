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
    void resizeEvent(QResizeEvent *event) override;

    Ui::StreamConfigWidget *ui;

signals:
    void sendAddressXPos(int XPos);
    void sendSourcePortXPos(int XPos);
    void sendDestPortXPos(int XPos);
    void sendIntervalXPos(int XPos);
};
