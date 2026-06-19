#ifndef TRAFFIC_GENERATOR_H
#define TRAFFIC_GENERATOR_H

#include <QHostAddress>
#include <QObject>
#include <QTimer>
#include <QUdpSocket>
#include "packet/packet_generator.h"
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
    QUdpSocket *m_socket = nullptr;
    QTimer *m_timer = nullptr;
    QHostAddress m_destAddress;
    quint16 m_destPort = 0;
    QHostAddress m_localAddress;
    quint16 m_localPort = 0;
    std::shared_ptr<PacketBuilder> m_builder;
    int m_sentCount = 0;
    int m_intervalMs = 1000;
};

#endif // TRAFFIC_GENERATOR_H
