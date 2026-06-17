#ifndef INTEGER_PACKET_FIELD_H
#define INTEGER_PACKET_FIELD_H

#include "base_packet_field.h"

class IntegerPacketField : public BasePacketField
{
public:
    explicit IntegerPacketField(const QJsonObject &obj);

    static std::unique_ptr<IntegerPacketField> fromJson(const QJsonObject &obj,
                                                        QString *outError = nullptr);

private:
    qint64 m_max; //  maximum value
    qint64 m_min; // minimum value
    QMap<QString, quint32> m_sizeOfType = {{"quint8", 1}, {"quint16", 2}, {"quint32", 4}};
};

#endif // INTEGER_PACKET_FIELD_H
