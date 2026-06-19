#include "base_packet_field.h"

std::unique_ptr<FieldDataGenerator> BasePacketField::buildGenerator() const
{
    switch (m_source) {
    case FieldSource::Constant:
    case FieldSource::Input: {
        QByteArray bytes = valueToBytes();
        return std::make_unique<ConstantGenerator>(bytes);
    }
    case FieldSource::Counter: {
        return createCounterGenerator();
    }
    default:
        return std::make_unique<ConstantGenerator>(QByteArray(m_size, '\0')); // fallback
    }
}

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

    m_DataGenerator = buildGenerator();
}

std::unique_ptr<FieldDataGenerator> BasePacketField::createCounterGenerator() const
{
    return std::make_unique<ConstantGenerator>(QByteArray(m_size, '\0'));
}
