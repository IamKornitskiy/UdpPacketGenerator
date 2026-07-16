#include "json_packet_field.h"
#include <QJsonParseError>

JsonPacketField::JsonPacketField(const QString &name,
                                 const QString &type,
                                 quint32 size,
                                 FieldSource source,
                                 const QString &initialValue)
    : StringPacketField(name, type, size, source, QString())
{}

std::optional<QString> JsonPacketField::isValid(const QString &value)
{
    QString errorMsg;
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(value.toUtf8(), &parseError);
    if (parseError.error != QJsonParseError::NoError) {
        return parseError.errorString();
    }
    return std::nullopt;
}

std::unique_ptr<StringPacketField> JsonPacketField::fromJson(const QJsonObject &obj,
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

    QString value{};
    if (obj.contains("value")) {
        auto error = isValid(obj["value"].toString());

        if (error) {
            if (outError)
                *outError = error.value();
        } else {
            value = obj["value"].toString();
        }
    }

    auto field = std::make_unique<JsonPacketField>(name, type, size, source, value);
    return field;
}
