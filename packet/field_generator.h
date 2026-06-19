#ifndef FIELD_GENERATOR_H
#define FIELD_GENERATOR_H

#include <QByteArray>

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

#endif // FIELD_GENERATOR_H
