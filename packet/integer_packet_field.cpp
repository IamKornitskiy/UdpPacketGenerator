#include "integer_packet_field.h"

IntegerPacketField::IntegerPacketField(const QJsonObject &obj)
    : BasePacketField(obj)
{
    m_max = obj["max"].toInteger();
    m_min = obj["min"].toInteger();
    m_size = m_sizeOfType["m_type"];
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

    if (!obj.contains("max") || !obj["max"].isString()) {
        if (outError)
            *outError = QString("Field '%1' missing 'max'").arg(name);
        return nullptr;
    }
    qint64 max = obj["max"].toInteger();

    if (!obj.contains("min") || !obj["min"].isString()) {
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
