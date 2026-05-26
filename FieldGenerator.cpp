#include "FieldGenerator.h"
#include <cstring>

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
        qToLittleEndian(v, buf.data());
        break;
    }
    case 4: {
        quint32 v = static_cast<quint32>(m_current);
        qToLittleEndian(v, buf.data());
        break;
    }
    case 8: {
        quint64 v = m_current;
        qToLittleEndian(v, buf.data());
        break;
    }
    default:
        break;
    }
    m_current++;
    return buf;
}

void PacketBuilder::setup(const QVector<PacketField> &fields,
                          const QHash<QString, QByteArray> &userValues)
{
    generators.clear();
    fieldNames.clear();
    for (const auto &f : fields) {
        fieldNames.append(f.name);
        switch (f.source) {
        case FieldSource::Constant: {
            QByteArray val = userValues.value(f.name);
            if (val.isEmpty())
                val = f.defaultValue;
            if (val.size() != f.size)
                val.resize(f.size);
            generators.push_back(std::make_unique<ConstantGenerator>(val));
            break;
        }
        case FieldSource::Counter: {
            generators.push_back(std::make_unique<CounterGenerator>(f.size, f.counterStart));
            break;
        }
        case FieldSource::Reserved: {
            generators.emplace_back(new ReservedGenerator(f.defaultValue));
            break;
        }
        case FieldSource::Input:
            QByteArray val = userValues.value(f.name);
            if (val.size() != f.size)
                val.resize(f.size);                              // дополнить нулями
            generators.emplace_back(new ConstantGenerator(val)); // Лишний генератор не нужен
            break;
        }
    }
}

QByteArray PacketBuilder::buildPacket()
{
    QByteArray packet;
    for (auto &gen : generators) {
        packet.append(gen->generate());
    }
    return packet;
}

void PacketBuilder::updateField(const QString &name, const QByteArray &newValue)
{
    for (int i = 0; i < fieldNames.size(); ++i) {
        if (fieldNames[i] == name) {
            if (auto *cgen = dynamic_cast<ConstantGenerator *>(generators[i].get())) {
                cgen->setValue(newValue);
                break;
            }
        }
    }
}
