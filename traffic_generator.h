#ifndef TRAFFIC_GENERATOR_H
#define TRAFFIC_GENERATOR_H

#include <QHostAddress>
#include <QObject>
#include <QTimer>
#include <QUdpSocket>
#include "field_generator.h"
#include <memory>

class TrafficGenerator : public QObject
{
    Q_OBJECT
public:
    explicit TrafficGenerator(QObject *parent = nullptr);
    ~TrafficGenerator();

    void configure(const QHostAddress &destAddr,
                   quint16 destPort,
                   const QHostAddress &localAddr,
                   quint16 localPort,
                   int intervalMs,
                   std::shared_ptr<PacketBuilder> builder);

public slots:
    void start();
    void stop();
    void updateFields(const QHash<QString, QByteArray> &values);
    void setIntervalMs(int newIntervalMs);

signals:
    void packetSent(int totalPackets);
    void errorOccurred(const QString &msg);

private:
    QUdpSocket *socket = nullptr;
    QTimer *timer = nullptr;
    QHostAddress destAddress;
    quint16 destPort = 0;
    QHostAddress localAddress;
    quint16 localPort = 0;
    std::shared_ptr<PacketBuilder> builder;
    int sentCount = 0;
    int intervalMs = 1000;
};

#endif // TRAFFIC_GENERATOR_H
