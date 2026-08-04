#ifndef JSON_PACKET_FIELD_H
#define JSON_PACKET_FIELD_H

#include "string_packet_field.h"

struct JsonValidationError
{
    int offset = -1;
    QString message;
    int line = -1;
    int column = -1;

    explicit operator bool() const { return offset != -1; }
    QString value() { return message; }
};

class JsonPacketField : public StringPacketField
{
public:
    JsonPacketField(const QString &name,
                    const QString &type,
                    quint32 size,
                    FieldSource source = FieldSource::Constant,
                    const QString &initialValue = QString());

    static JsonValidationError isValid(const QString &value);

    // static function for checking the validity of JSON object data, and pre-constructor
    static std::unique_ptr<JsonPacketField> fromJson(const QJsonObject &obj,
                                                     QString *outError = nullptr);

    QString lastError() const;

private:
    QString m_error{};
};

#endif // JSON_PACKET_FIELD_H
