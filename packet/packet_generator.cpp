#include "packet_generator.h"

void PacketBuilder::setup(const std::vector<std::unique_ptr<BasePacketField> > &fields)
{
    m_generators.clear();
    m_generators.reserve(fields.size());
    for (auto &field : fields) {
        m_generators.push_back(field->buildGenerator());
    }
}

QByteArray PacketBuilder::buildPacket()
{
    QByteArray packet;
    for (auto &gen : m_generators) {
        packet.append(gen->generate());
    }
    return packet;
}
