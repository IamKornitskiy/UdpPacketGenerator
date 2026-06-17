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

    if (!obj.contains("value_source") || !obj["value_source"].isString()) {
        return QString("Field '%1' missing 'value_source'").arg(name);
    }

    return std::nullopt;
}

BasePacketField::BasePacketField(const QJsonObject &obj)
{
    m_name = obj["name"].toString();
    m_type = obj["type"].toString();
    m_type = obj["value_source"].toString();
}
