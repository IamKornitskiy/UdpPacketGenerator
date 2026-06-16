## [En](README.md) Ru

# UDP Packet Generator [![GitHub release](https://img.shields.io/github/v/release/IamKornitskiy/UdpPacketGenerator)](https://github.com/IamKornitskiy/UdpPacketGenerator/releases) [![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)
[![Qt 6.8+](https://img.shields.io/badge/Qt-6.8%2B-lightgray?logo=Qt&logoColor=white&labelColor=brightgreen
)](https://www.qt.io/)

Гибкий генератор UDP-пакетов, настраиваемый с помощью JSON-шаблонов.

## О проекте

UDP Packet Generator создаёт и отправляет пользовательские UDP-пакеты по JSON-шаблону.  
Поля пакета могут быть разных типов — константы, счётчики или редактируемые значения.

## Возможности

- **JSON-шаблон пакета** – поля, типы, размеры и источники значений в одном файле
- **Разные источники значений** – `constant`, `counter`, `reserved` и `input` с настраиваемыми диапазонами

## Скриншот

![Главное окно](screenshots/mainWindow.jpg)

## Сборка

### Требования

- **Qt 6.8+** (модули Core, Gui, Widgets, Network)
- **CMake 3.16+**
- **Компилятор C++17** (MSVC 2019+, GCC 9+, Clang 10+)

```bash
git clone https://github.com/IamKornitskiy/UdpPacketGenerator.git
cd UdpPacketGenerator
mkdir build && cd build
cmake .. -DCMAKE_PREFIX_PATH=/path/to/Qt/6.8.0/gcc_64
cmake --build .
```

## Использование

1. **Загрузите JSON-шаблон** – нажмите `Load` и выберите файл `.json` с описанием пакета.
2. **Отредактируйте поля ввода** – если шаблон содержит поля `input`, задайте их значения в правой панели.
3. **Укажите адрес назначения** – IP и порт получателя.
4. **Укажите локальный порт (опционально)** – порт отправителя, или оставьте `0` для автоматического выбора.
5. **Выберите интервал** – период отправки в миллисекундах.
6. **Запустите** – нажмите `Start`, начнётся отправка пакетов.

## JSON-шаблон

Структура пакета описывается в JSON-файле.  
Примеры и подробное руководство по созданию шаблонов смотрите в [`templates/`](templates/).

Если вы создали JSON-шаблон популярного протокола или пакета — отправляйте pull request, я с удовольствием добавлю его в коллекцию.

## Лицензия

Проект распространяется под лицензией GNU General Public License v3.0 – подробнее в файле [LICENSE](LICENSE).
