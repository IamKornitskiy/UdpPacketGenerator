#include "float_packet_field.h"

FloatPacketField::FloatPacketField(const QString &name,
                                   const QString &type,
                                   quint32 size,
                                   double min,
                                   double max,
                                   double initialValue,
                                   quint8 decimals,
                                   FieldSource source,
                                   QDataStream::ByteOrder byteOrder)
    : BasePacketField(name, type, size, source, byteOrder)
    , m_min(min)
    , m_max(max)
    , m_value(initialValue)
    , m_decimals(decimals)
{}

QByteArray FloatPacketField::bytes() const
{
    auto locker = lock();
    return serializeValue(m_value, m_size);
}

std::unique_ptr<FloatPacketField> FloatPacketField::fromJson(const QJsonObject &obj,
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
    auto size = kSizeOfFloatType.value(type);

    if (obj.contains("source")) {
        QString sourceName = obj["source"].toString();
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
        max = obj["max"].toDouble();
        if (max > defMax) {
            if (outError)
                *outError = QString("'max' greater than max limit of %1").arg(type);
            return nullptr;
        }
    }

    if (obj.contains("min") && !obj["min"].isNull()) {
        min = obj["min"].toDouble();
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

    auto decimals = 2;
    if (obj.contains("decimals")) {
        decimals = obj["decimals"].toInt();
    }

    auto field = std::make_unique<FloatPacketField>(name,
                                                    type,
                                                    size,
                                                    min,
                                                    max,
                                                    0,
                                                    decimals,
                                                    source,
                                                    byteOrder);
    return field;
}

double FloatPacketField::min() const
{
    return m_min;
}

double FloatPacketField::max() const
{
    return m_max;
}

quint8 FloatPacketField::decimals() const
{
    return m_decimals;
}

double FloatPacketField::value() const
{
    return m_value;
}

void FloatPacketField::setValue(double newValue)
{
    m_value = newValue;
}
