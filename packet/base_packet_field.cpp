#include "base_packet_field.h"

std::optional<QString> BasePacketField::isValid(const QJsonObject &obj)
{
    if (!obj.contains("name") || !obj["name"].isString()) {
        return "Field missing 'name'";
    }
    QString name = obj["name"].toString();

    if (!obj.contains("type") || !obj["type"].isString()) {
        return QString("Field '%1' missing 'type'").arg(name);
    }

    return std::nullopt;
}

BasePacketField::BasePacketField(const QJsonObject &obj)
{
    m_name = obj["name"].toString();
    m_type = obj["type"].toString();

    if (obj.contains("value_source")) {
        QString sourceName = obj["value_source"].toString();
        if (m_sourceMap.contains(sourceName))
            m_source = m_sourceMap[sourceName];
    }

    if (obj.contains("order")) {
        QString sourceName = obj["order"].toString();
        if (sourceName == "be")
            m_byteOrder = QDataStream::BigEndian;
    }
}
