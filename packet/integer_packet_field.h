#ifndef INTEGER_PACKET_FIELD_H
#define INTEGER_PACKET_FIELD_H

#include "base_packet_field.h"

class IntegerPacketField : public BasePacketField
{
public:
    explicit IntegerPacketField(const QString &name,
                                const QString &type,
                                quint32 size,
                                qint64 min,
                                qint64 max,
                                qint64 initialValue = 0,
                                FieldSource source = FieldSource::Constant,
                                QDataStream::ByteOrder byteOrder = QDataStream::LittleEndian);
    QByteArray bytes() const override; // return value in bytes
    void incrementCounter() override;

    // static function for checking the validity of JSON object data, and pre-constructor
    static std::unique_ptr<IntegerPacketField> fromJson(const QJsonObject &obj,
                                                        QString *outError = nullptr);

    void setValue(qint64 newValue);
    qint64 value() const;

    qint64 max() const { return m_max; }
    qint64 min() const { return m_min; }

private:
    qint64 m_max; //  maximum value
    qint64 m_min; // minimum value

    qint64 m_value = 0;
    quint64 m_startValue = 0; // for counter
};

#endif // INTEGER_PACKET_FIELD_H
