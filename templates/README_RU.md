## [En](README.md) Ru

# Руководство по JSON-шаблонам

В этом руководстве описано, как создавать JSON-шаблоны для **UDP Packet Generator**.
Шаблоны определяют структуру UDP-пакетов — имена полей, типы, размеры и источники значений.

Файл `.json` можно разместить в любом месте и загрузить в приложение кнопкой **Load**.
Примеры шаблонов находятся в этой же папке.

Общую информацию о проекте смотрите в [основном README](../README.md).

## Источники значений (`value_source`)

Каждое поле должно указывать `value_source` — откуда берутся данные поля.

| Источник | Описание |
|----------|----------|
| `constant` | Фиксированное hex-значение из JSON. Не редактируется в GUI. |
| `counter` | Автоинкрементируемый счётчик. Стартует с `start_value` (по умолчанию 0). Little-endian. |
| `reserved` | Фиксированная последовательность байт из JSON. Не редактируется в GUI. |
| `input` | Редактируемое поле. Отображается как SpinBox (целые) или DoubleSpinBox (дробные). |

## Типы полей (`type`)

| Тип | Размер (байт) | Виджет ввода |
|------|:------------:|--------------|
| `uint8` | 1 | SpinBox |
| `uint16` | 2 | SpinBox |
| `uint32` | 4 | SpinBox |
| `uint64` | 8 | None |
| `int8` | 1 | SpinBox |
| `int16` | 2 | SpinBox |
| `int32` | 4 | SpinBox |
| `int64` | 8 | None |
| `float32` | 4 | DoubleSpinBox |
| `float64` | 8 | DoubleSpinBox |
| `bytes` | задаётся ключом `size` | – (используйте `constant` или `reserved`) |

## Справочник ключей

| Ключ | Обязателен | Применяется к | Описание |
|------|:----------:|---------------|----------|
| `name` | да | всем | Уникальное имя поля |
| `type` | да | всем | Тип данных поля (см. таблицу выше) |
| `size` | только для `bytes` | `bytes` | Размер поля в байтах |
| `value_source` | да | всем | `constant`, `counter`, `reserved` или `input` |
| `default_value_hex` | для `constant`, `reserved` | `constant`, `reserved` | Hex-строка (например, `"AABB"`) |
| `start_value` | для `counter` | `counter` | Начальное значение (по умолчанию 0) |
| `minInt` | для `input` с целыми типами | `input` | Минимальное значение SpinBox |
| `maxInt` | для `input` с целыми типами | `input` | Максимальное значение SpinBox |
| `minDouble` | для `input` с дробными типами | `input` | Минимальное значение DoubleSpinBox |
| `maxDouble` | для `input` с дробными типами | `input` | Максимальное значение DoubleSpinBox |

## Examples

### Simple packet with constant and counter

```json
{
  "fields": [
    { "name": "sync",
      "type": "uint8",
      "value_source": "constant", "default_value_hex": "AA" 
    },
    { "name": "counter",
      "type": "uint16",
      "value_source": "counter",
      "start_value": 0 
    },
    { "name": "reserve",
      "type": "bytes",
      "size": 4,
      "value_source": "reserved", "default_value_hex": "00000000" 
    },
    { "name": "voltage",
      "type": "float32", 
      "value_source": "input",
      "minDouble": 0.0, "maxDouble": 5.0, 
    },
    { "name": "mode",
      "type": "uint8",
      "value_source": "input",
      "minInt": 0,
      "maxInt": 3,
    }
  ]
}
```

## 💡 Есть чем поделиться?
Если вы создали JSON-шаблон известного протокола или пакета — отправляйте pull request!