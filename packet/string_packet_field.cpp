#include "string_packet_field.h"

StringPacketField::StringPacketField(const QString &name,
                                     const QString &type,
                                     quint32 size,
                                     FieldSource source,
                                     const QString &initialValue)
    : BasePacketField(name, type, size, source)
    , m_value(initialValue)
{}

QByteArray StringPacketField::bytes() const
{
    auto locker = lock();
    quint32 size;
    if (m_size == 0)
        size = m_value.toUtf8().size();
    else
        size = m_size;
    return serializeValue(m_value, size);
}

QString StringPacketField::value() const
{
    auto locker = lock();
    return m_value;
}

void StringPacketField::setValue(const QString &newValue)
{
    auto locker = lock();
    m_value = newValue;
}

std::unique_ptr<StringPacketField> StringPacketField::fromJson(const QJsonObject &obj,
                                                               QString *outError)
{
    auto validationError = BasePacketField::jsonIsValid(obj);
    if (validationError) {
        if (outError)
            *outError = validationError.value();
        return nullptr;
    }

    auto name = obj["name"].toString();
    auto type = obj["type"].toString();
    auto source = FieldSource::Constant;
    auto size = 0;

    if (obj.contains("source")) {
        QString sourceName = obj["source"].toString();
        if (kSourceMap.contains(sourceName))
            source = kSourceMap[sourceName];
    }

    if (obj.contains("size")) {
        size = obj["size"].toInt();
    }

    auto field = std::make_unique<StringPacketField>(name, type, size, source);
    return field;
}
