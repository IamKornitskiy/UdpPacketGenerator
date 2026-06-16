#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>
#include <QPushButton>
#include <QSpinBox>
#include <QThread>
#include "packet_template.h"
#include "titlebar.h"
#include "traffic_generator.h"
#include <memory>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onLoadJson();
    void onStart();
    void onStop();
    void onPacketSent(int count);
    void onError(const QString &msg);
    void onFieldChanged();

private:
    Ui::MainWindow *ui;
    TitleBar *m_titleBar = nullptr;

    void clearDynamicFields();
    void buildDynamicFields();

    void setRunning(bool running);

    QHash<QString, QByteArray> collectConstantValues();
    QByteArray integerToBytes(qint64 value, int size);
    QByteArray floatToBytes(double value, const QString &type);

    PacketTemplate packetTemplate;

    QFormLayout *dynamicLayout;
    QMap<QString, QWidget *> fieldEditors;

    QThread *workerThread;
    TrafficGenerator *generator;
    bool isRunning = false;
    std::shared_ptr<PacketBuilder> packetBuilder;
};

#endif // MAIN_WINDOW_H
