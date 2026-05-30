#include "TrafficGenerator.h"

TrafficGenerator::TrafficGenerator(QObject *parent)
    : QObject(parent)
{}

TrafficGenerator::~TrafficGenerator()
{
    stop();
}

void TrafficGenerator::configure(const QHostAddress &destAddr,
                                 quint16 destPort,
                                 const QHostAddress &localAddr,
                                 quint16 localPort,
                                 int intervalMs,
                                 std::shared_ptr<PacketBuilder> builder)
{
    this->destAddress = destAddr;
    this->destPort = destPort;
    this->localAddress = localAddr;
    this->localPort = localPort;
    this->intervalMs = intervalMs;
    this->builder = builder;
}

void TrafficGenerator::start()
{
    if (timer)
        return;

    socket = new QUdpSocket(this);
    if (!localAddress.isNull() || localPort != 0) {
        if (!socket->bind(localAddress, localPort)) {
            emit errorOccurred("Failed to bind UDP socket: " + socket->errorString());
            delete socket;
            socket = nullptr;
            return;
        }
    }

    timer = new QTimer(this);
    timer->setInterval(intervalMs);
    connect(timer, &QTimer::timeout, this, [this]() {
        if (!builder)
            return;
        QByteArray packet = builder->buildPacket();
        qint64 ret = socket->writeDatagram(packet, destAddress, destPort);
        if (ret == -1) {
            emit errorOccurred("Send error: " + socket->errorString());
        }
        sentCount++;
        emit packetSent(sentCount);
    });
    timer->start();
}

void TrafficGenerator::stop()
{
    if (timer) {
        timer->stop();
        delete timer;
        timer = nullptr;
    }
    if (socket) {
        socket->close();
        delete socket;
        socket = nullptr;
    }
    sentCount = 0;
}

void TrafficGenerator::updateFields(const QHash<QString, QByteArray> &values)
{
    if (!builder)
        return;
    for (auto it = values.begin(); it != values.end(); ++it) {
        builder->updateField(it.key(), it.value());
    }
}

void TrafficGenerator::setIntervalMs(int newIntervalMs)
{
    intervalMs = newIntervalMs;
    if (timer)
        timer->setInterval(intervalMs);
}
