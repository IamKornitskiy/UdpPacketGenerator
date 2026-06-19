#include "integer_packet_field.h"

IntegerPacketField::IntegerPacketField(const QJsonObject &obj)
    : BasePacketField(obj)
{
    m_max = obj["max"].toInteger();
    m_min = obj["min"].toInteger();
    m_size = m_sizeOfType["m_type"];

    if (obj.contains("start_value"))
        m_startValue = obj["start_value"].toInteger();
}

std::unique_ptr<IntegerPacketField> IntegerPacketField::fromJson(const QJsonObject &obj,
                                                                 QString *outError)
{
    auto validationError = BasePacketField::isValid(obj);
    if (validationError) {
        if (outError)
            *outError = validationError.value();
        return nullptr;
    }

    QString name = obj["name"].toString();

    if (!obj.contains("max") || obj["max"].isNull()) {
        if (outError)
            *outError = QString("Field '%1' missing 'max'").arg(name);
        return nullptr;
    }
    qint64 max = obj["max"].toInteger();

    if (!obj.contains("min") || obj["min"].isNull()) {
        if (outError)
            *outError = QString("Field '%1' missing 'min'").arg(name);
        return nullptr;
    }
    qint64 min = obj["min"].toInteger();

    if (min >= max) {
        if (outError)
            *outError = QString("'min' greater than or equal to 'max'");
        return nullptr;
    }

    auto field = std::make_unique<IntegerPacketField>(obj);
    return field;
}

QByteArray IntegerPacketField::integerToBytes(qint64 value, qint64 byteSize)
{
    QByteArray bytes(byteSize, '\0');
    switch (byteSize) {
    case 1:
        bytes[0] = static_cast<char>(value);
        break;
    case 2:
        qToLittleEndian<quint16>(value, bytes.data());
        break;
    case 4:
        qToLittleEndian<quint32>(value, bytes.data());
        break;
    case 8:
        qToLittleEndian<quint64>(value, bytes.data());
        break;
    }
    return bytes;
}

QByteArray IntegerPacketField::valueToBytes() const
{
    return integerToBytes(m_value, m_size);
}

std::unique_ptr<FieldGenerator> IntegerPacketField::createCounterGenerator() const
{
    return std::make_unique<CounterGenerator>(m_size, m_startValue);
}
