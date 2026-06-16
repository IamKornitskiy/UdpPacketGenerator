#ifndef PACKET_TEMPLATE_H
#define PACKET_TEMPLATE_H

#include <QString>
#include <QVector>
#include "base_packet_field.h"
#include <memory>

class PacketTemplate
{
public:
    PacketTemplate() = default;

    QVector<std::unique_ptr<BasePacketField>> m_fields;
    int m_totalSize = 0;

    QString loadFromJson(const QByteArray &jsonData);
    QString validate() const;

private:
    static std::unique_ptr<BasePacketField> createField(const QString &type);
    bool parseField(const QJsonObject &obj, std::unique_ptr<BasePacketField> &field, QString &error);
};

#endif // PACKET_TEMPLATE_H
