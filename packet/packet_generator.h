#ifndef PACKET_GENERATOR_H
#define PACKET_GENERATOR_H

#include <QByteArray>
#include "base_packet_field.h"
#include "field__data_generator.h"
#include <memory>

class PacketBuilder
{
public:
    void setup(const std::vector<std::unique_ptr<BasePacketField>> &fields);
    QByteArray buildPacket();

private:
    std::vector<std::unique_ptr<FieldDataGenerator>> m_generators;
};

#endif // PACKET_GENERATOR_H
