#ifndef PACKET_BUILDER_H
#define PACKET_BUILDER_H

#include <QByteArray>
#include "base_packet_field.h"
#include <memory>

class PacketBuilder
{
public:
    static QByteArray buildPacket(const std::vector<std::unique_ptr<BasePacketField>> &fields);
};

#endif // PACKET_BUILDER_H
