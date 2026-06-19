#include "field_generator.h"

CounterGenerator::CounterGenerator(int size, quint64 start)
    : m_size(size)
    , m_current(start)
    , m_start(start)
{}

QByteArray CounterGenerator::generate()
{
    QByteArray buf(m_size, '\0');
    switch (m_size) {
    case 1: {
        quint8 v = static_cast<quint8>(m_current);
        buf[0] = static_cast<char>(v);
        break;
    }
    case 2: {
        quint16 v = static_cast<quint16>(m_current);
        // qToLittleEndian(v, buf.data());
        break;
    }
    case 4: {
        quint32 v = static_cast<quint32>(m_current);
        // qToLittleEndian(v, buf.data());
        break;
    }
    case 8: {
        quint64 v = m_current;
        // qToLittleEndian(v, buf.data());
        break;
    }
    default:
        break;
    }
    m_current++;
    return buf;
}