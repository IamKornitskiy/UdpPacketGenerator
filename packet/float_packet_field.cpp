#include "float_packet_field.h"
namespace {
const QHash<QString, std::pair<double, double>> kTypeFloatBounds
    = {{"float32", {std::numeric_limits<float>::lowest(), std::numeric_limits<float>::max()}},
       {"float64", {std::numeric_limits<double>::lowest(), std::numeric_limits<double>::max()}}};

const QMap<QString, quint32> kSizeOfFloatType = {{"float32", 4}, {"float64", 8}};
} // namespace

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
    if (m_type == "float32") {
        return serializeValue(static_cast<float>(m_value), m_size);
    } else if (m_type == "float64") {
        return serializeValue(m_value, m_size);
    }
    return {};
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

    auto defaultBounds = kTypeFloatBounds.value(type);
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
    auto locker = lock();
    return m_value;
}

void FloatPacketField::setValue(double newValue)
{
    if (newValue <= m_max && newValue >= m_min) {
        auto locker = lock();
        m_value = newValue;
    }
}
