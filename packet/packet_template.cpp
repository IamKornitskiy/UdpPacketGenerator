#include "packet_template.h"
#include <QJsonArray>
#include <QJsonParseError>
#include <QJsonValue>

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
        auto field = createField(obj);
        if (!field)
            return QString("Unknown type: %1").arg(obj["type"].toString());
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

std::unique_ptr<BasePacketField> PacketTemplate::createField(const QJsonObject &obj)
{
    QString type = obj["type"].toString();
    static const QHash<QString, std::function<std::unique_ptr<BasePacketField>(const QJsonObject &)>>
        factory = {
            // {"uint8", IntegerField::fromJson},
            // {"uint16", IntegerField::fromJson},
            // {"float32", FloatField::fromJson},
            // {"float64", FloatField::fromJson},
            // {"json", JsonField::fromJson},
            // ... etc.
        };
    auto it = factory.find(type);
    return (it != factory.end()) ? it.value()(obj) : nullptr;
}
