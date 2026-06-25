#include "integer_packet_field.h"

IntegerPacketField::IntegerPacketField(const QString &name,
                                       const QString &type,
                                       quint32 size,
                                       qint64 min,
                                       qint64 max,
                                       qint64 initialValue,
                                       FieldSource source,
                                       QDataStream::ByteOrder byteOrder)
    : BasePacketField(name, type, size, source, byteOrder)
    , m_min(min)
    , m_max(max)
    , m_value(initialValue)
{}

std::unique_ptr<IntegerPacketField> IntegerPacketField::fromJson(const QJsonObject &obj,
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
    auto size = kSizeOfType.value(type);

    if (obj.contains("value_source")) {
        QString sourceName = obj["value_source"].toString();
        if (kSourceMap.contains(sourceName))
            source = kSourceMap[sourceName];
    }

    auto byteOrder = QDataStream::LittleEndian;
    if (obj.contains("order")) {
        QString sourceName = obj["order"].toString();
        if (sourceName == "be")
            byteOrder = QDataStream::BigEndian;
    }

    auto defaultBounds = kTypeBounds.value(type);
    auto defMax = defaultBounds.second;
    auto defMin = defaultBounds.first;
    auto max = defMax;
    auto min = defMin;

    if (obj.contains("max") && !obj["max"].isNull()) {
        max = obj["max"].toInteger();
        if (max > defMax) {
            if (outError)
                *outError = QString("'max' greater than max limit of %1").arg(type);
            return nullptr;
        }
    }

    if (obj.contains("min") && !obj["min"].isNull()) {
        min = obj["min"].toInteger();
        if (min < defMin) {
            if (outError)
                *outError = QString("'min' less than min limit of %1").arg(type);
            return nullptr;
        }
    }

    if (min >= max) {
        if (outError)
            *outError = QString("'min' greater than or equal to 'max'");
        return nullptr;
    }

    auto field
        = std::make_unique<IntegerPacketField>(name, type, size, min, max, 0, source, byteOrder);
    return field;
}

QByteArray IntegerPacketField::bytes() const
{
    auto locker = lock();
    return serializeValue(m_value, m_size);
}

void IntegerPacketField::incrementCounter()
{
    auto locker = lock();
    m_value++;
}
void IntegerPacketField::setValue(qint64 newValue)
{
    auto locker = lock();
    m_value = newValue;
}

qint64 IntegerPacketField::value() const
{
    auto locker = lock();
    return m_value;
}
