#ifndef PACKET_FIELD_H
#define PACKET_FIELD_H

#include <QByteArray>
#include <QDataStream>
#include <QJsonObject>
#include <QMutex>
#include <QMutexLocker>
#include <QRegularExpression>
#include <QString>

enum class FieldSource { Input, Constant, Counter }; // see m_sourceMap

class BasePacketField
{
public:
    virtual ~BasePacketField() = default;
    virtual QByteArray bytes() const = 0; // return value in bytes

    // it is possible to rename in the future, for implementation and other operations depending on FieldSource
    virtual void incrementCounter() = 0;

    QString name() const { return m_name; }
    quint32 size() const { return m_size; }
    FieldSource source() const { return m_source; }

    static std::optional<QString> isValid(const QJsonObject &obj); // checks required fields

private:
    QMap<QString, FieldSource> m_sourceMap
        = {{"input", FieldSource::Input},       // for changing values
           {"constant", FieldSource::Constant}, // for constant or reserved fields
           {"counter", FieldSource::Counter}};  // for counters

protected:
    // must be called in the constructor of the subclass, after calling isValid
    explicit BasePacketField(const QJsonObject &obj);

    QMutexLocker<QMutex> lock() const { return QMutexLocker<QMutex>(&m_mutex); }

    QString m_name;                               // name of field
    QString m_type;                               // uint8, int8, float and etc.
    quint32 m_size = 0;                           // in bytes
    FieldSource m_source = FieldSource::Constant; // see m_sourceMap
    QDataStream::ByteOrder m_byteOrder = QDataStream::LittleEndian;
    mutable QMutex m_mutex;

signals:
};

#endif // PACKET_FIELD_H
