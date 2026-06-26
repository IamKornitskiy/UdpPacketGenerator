#include "base_packet_field.h"

std::optional<QString> BasePacketField::jsonIsValid(const QJsonObject &obj)
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

BasePacketField::BasePacketField(const QString &name,
                                 const QString &type,
                                 quint32 size,
                                 FieldSource source,
                                 QDataStream::ByteOrder byteOrder)
    : m_name(name)
    , m_type(type)
    , m_size(size)
    , m_source(source)
    , m_byteOrder(byteOrder)
{}
