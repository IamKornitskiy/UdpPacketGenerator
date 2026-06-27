#include "packet_builder.h"

QByteArray PacketBuilder::buildPacket(const std::vector<std::unique_ptr<BasePacketField>> &fields)
{
    QByteArray packet;
    for (auto &field : fields) {
        packet.append(field->bytes());
        if (field->source() == FieldSource::Counter) {
            field->incrementCounter();
        }
    }
    return packet;
}
