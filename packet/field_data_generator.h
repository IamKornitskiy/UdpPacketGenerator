#ifndef FIELD_DATA_GENERATOR_H
#define FIELD_DATA_GENERATOR_H

#include <QByteArray>

class FieldDataGenerator
{
public:
    virtual ~FieldDataGenerator() = default;
    virtual QByteArray generate() = 0;
    virtual void reset() {}
};

class ConstantGenerator : public FieldDataGenerator
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

class CounterGenerator : public FieldDataGenerator
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

class ReservedGenerator : public FieldDataGenerator
{
public:
    explicit ReservedGenerator(const QByteArray &value)
        : m_value(value)
    {}
    QByteArray generate() override { return m_value; }

private:
    QByteArray m_value;
};

#endif // FIELD_DATA_GENERATOR_H
