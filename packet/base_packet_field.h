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

const QMap<QString, FieldSource> kSourceMap
    = {{"input", FieldSource::Input},       // for changing values
       {"constant", FieldSource::Constant}, // for constant or reserved fields
       {"counter", FieldSource::Counter}};  // for counters

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

    static std::optional<QString> jsonIsValid(const QJsonObject &obj); // checks required fields

protected:
    // must be called in the constructor of the subclass, after calling isValid
    explicit BasePacketField(const QString &name,
                             const QString &type,
                             quint32 size,
                             FieldSource source = FieldSource::Constant,
                             QDataStream::ByteOrder byteOrder = QDataStream::LittleEndian);

    mutable QMutex m_mutex;
    QMutexLocker<QMutex> lock() const { return QMutexLocker<QMutex>(&m_mutex); }

    QString m_name;                               // name of field
    QString m_type;                               // uint8, int8, float and etc.
    quint32 m_size = 0;                           // in bytes
    FieldSource m_source = FieldSource::Constant; // see m_sourceMap
    QDataStream::ByteOrder m_byteOrder = QDataStream::LittleEndian;

    template<typename T>
    QByteArray serializeValue(const T &value, int size) const
    {
        QByteArray bytes(size, '\0');
        if constexpr (std::is_same_v<T, QString>) {
            // for future reference: Strings are serialized as UTF-8, and byte order does not matter
            QByteArray data = value.toUtf8();
            bytes = data.leftJustified(size, '\0'); // adds zeros to the desired size
        } else {
            // number types (int, float, double, uint64 and etc.)
            memcpy(bytes.data(), &value, size);

            // reverse the byte order, if required
            if (m_byteOrder == QDataStream::BigEndian && size > 1) {
                std::reverse(bytes.begin(), bytes.end());
            }
        }
        return bytes;
    }
};

#endif // PACKET_FIELD_H
