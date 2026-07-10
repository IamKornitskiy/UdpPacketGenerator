#ifndef JSON_PACKET_FIELD_H
#define JSON_PACKET_FIELD_H

#include "string_packet_field.h"

class JsonPacketField : public StringPacketField
{
public:
    JsonPacketField(const QString &name,
                    const QString &type,
                    quint32 size,
                    FieldSource source = FieldSource::Constant,
                    const QString &initialValue = QString());

    static std::optional<QString> isValid(const QString &value);

    // static function for checking the validity of JSON object data, and pre-constructor
    static std::unique_ptr<StringPacketField> fromJson(const QJsonObject &obj,
                                                       QString *outError = nullptr);

    QString lastError() const;

private:
    QString m_Error{};
};

#endif // JSON_PACKET_FIELD_H
