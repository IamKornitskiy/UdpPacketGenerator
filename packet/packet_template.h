#ifndef PACKET_TEMPLATE_H
#define PACKET_TEMPLATE_H

#include <QString>
#include <QVector>
#include "base_packet_field.h"
#include <memory>

class PacketTemplate
{
public:
    PacketTemplate() = default;

    QString loadFromJson(const QByteArray &jsonData);
    QString validate() const;

    const std::vector<std::unique_ptr<BasePacketField>> &fields() const { return m_fields; }

private:
    std::unique_ptr<BasePacketField> createField(const QJsonObject &obj, QString *error);
    std::vector<std::unique_ptr<BasePacketField>> m_fields;
};

#endif // PACKET_TEMPLATE_H
