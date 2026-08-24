#ifndef TRAFFIC_GENERATOR_H
#define TRAFFIC_GENERATOR_H

#include <QHostAddress>
#include <QObject>
#include <QTimer>
#include <QUdpSocket>
#include "base_packet_field.h"
#include <memory>

class TrafficGenerator : public QObject
{
    Q_OBJECT
public:
    explicit TrafficGenerator(QObject *parent = nullptr);
    ~TrafficGenerator();

    void configure(const QString &destAddrsStr,
                   quint16 destPort,
                   const QHostAddress &localAddr,
                   quint16 localPort,
                   int intervalMs,
                   const std::vector<std::unique_ptr<BasePacketField>> &fields);

public slots:
    void start();
    void stop();
    void setIntervalMs(int newIntervalMs);

signals:
    void packetSent(int totalPackets);
    void errorOccurred(const QString &msg);

private:
    QUdpSocket *m_socket = nullptr;
    QTimer *m_timer = nullptr;
    QVector<QHostAddress> m_destAddresses;
    quint16 m_destPort = 0;
    QHostAddress m_localAddress;
    quint16 m_localPort = 0;
    int m_sentCount = 0;
    int m_intervalMs = 1000;
    const std::vector<std::unique_ptr<BasePacketField>> *m_fields{};
};

#endif // TRAFFIC_GENERATOR_H
