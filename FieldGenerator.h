#ifndef FIELDGENERATOR_H
#define FIELDGENERATOR_H

#include <QByteArray>
#include "PacketTemplate.h"
#include <memory>

class FieldGenerator
{
public:
    virtual ~FieldGenerator() = default;
    virtual QByteArray generate() = 0;
    virtual void reset() {}
};

class ConstantGenerator : public FieldGenerator
{
public:
    explicit ConstantGenerator(const QByteArray &value)
        : m_value(value)
    {}
    QByteArray generate() override { return m_value; }
    void setValue(const QByteArray &v) { m_value = v; }

private:
    QByteArray m_value;
};

class CounterGenerator : public FieldGenerator
{
public:
    CounterGenerator(int size, quint64 start);
    QByteArray generate() override;
    void reset() override { m_current = m_start; }

private:
    int m_size;
    quint64 m_current;
    quint64 m_start;
};

class ReservedGenerator : public FieldGenerator
{
public:
    explicit ReservedGenerator(const QByteArray &value)
        : m_value(value)
    {}
    QByteArray generate() override { return m_value; }

private:
    QByteArray m_value;
};

class PacketBuilder
{
public:
    void setup(const QVector<PacketField> &fields, const QHash<QString, QByteArray> &userValues);
    QByteArray buildPacket();
    void updateField(const QString &name, const QByteArray &newValue);

private:
    std::vector<std::unique_ptr<FieldGenerator>> generators;
    QVector<QString> fieldNames;
};

#endif // FIELDGENERATOR_H
