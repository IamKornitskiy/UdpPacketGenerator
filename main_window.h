#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>
#include <QPushButton>
#include <QSpinBox>
#include <QThread>
#include "packet/packet_template.h"
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

    PacketTemplate m_packetTemplate;

    QFormLayout *m_dynamicLayout;
    QMap<QString, QWidget *> m_fieldEditors;

    QThread *m_workerThread;
    TrafficGenerator *m_generator;
    bool m_isRunning = false;
    std::shared_ptr<PacketBuilder> m_packetBuilder;
};

#endif // MAIN_WINDOW_H
