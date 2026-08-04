#include "packet_template.h"
#include <QJsonArray>
#include <QJsonParseError>
#include <QJsonValue>
#include "csv_packet_field.h"
#include "float_packet_field.h"
#include "integer_packet_field.h"
#include "json_packet_field.h"
#include "nmea_packet_field.h"
#include "string_packet_field.h"

QString PacketTemplate::loadFromJson(const QByteArray &jsonData)
{
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(jsonData, &parseError);
    if (parseError.error != QJsonParseError::NoError)
        return QString("JSON parse error: %1").arg(parseError.errorString());

    QJsonObject root = doc.object();
    QJsonArray fieldsArray = root["fields"].toArray();
    if (fieldsArray.isEmpty())
        return "No 'fields' array in JSON";

    m_fields.clear();
    for (const QJsonValue &val : fieldsArray) {
        QJsonObject obj = val.toObject();
        QString error = QString("Unknown type: %1").arg(obj["type"].toString());
        auto field = createField(obj, &error);
        if (!field)
            return error;
        m_fields.push_back(std::move(field));
    }
    return validate();
}

QString PacketTemplate::validate() const
{
    QSet<QString> names;
    for (const auto &f : m_fields) {
        if (names.contains(f->name()))
            return QString("Duplicate field name: %1").arg(f->name());
        names.insert(f->name());
    }
    return {}; // OK
}

std::unique_ptr<BasePacketField> PacketTemplate::createField(const QJsonObject &obj, QString *error)
{
    QString type = obj["type"].toString();
    static const QHash<QString,
                       std::function<std::unique_ptr<BasePacketField>(const QJsonObject &, QString *)>>
        factory = {
            {"int8", IntegerPacketField::fromJson},
            {"int16", IntegerPacketField::fromJson},
            {"int32", IntegerPacketField::fromJson},
            {"float32", FloatPacketField::fromJson},
            {"float64", FloatPacketField::fromJson},
            {"text", StringPacketField::fromJson},
            {"json", JsonPacketField::fromJson},
            {"nmea", NmeaPacketField::fromJson},
            {"csv", CsvPacketField::fromJson}
        };
    auto it = factory.find(type);

    return (it != factory.end()) ? it.value()(obj, error) : nullptr;
}
