# Стиль кода (Code Style)

## RU

| Сущность                               | Стиль               | Пример                                                |
|:--------------------------------------|:--------------------|:------------------------------------------------------|
| Классы, структуры, перечисления (enum) | PascalCase          | `class UserAccount`, `struct Point3D`, `enum class ColorType` |
| Функции (глобальные и методы классов)  | camelCase           | `getUserBalance()`, `calculateTotal()`, `setName()`    |
| Локальные переменные и параметры      | camelCase           | `userName`, `totalCount`, `maxRetries`                 |
| Члены класса                          | m_ + PascalCase     | `m_Name`, `m_Count`                                   |
| Константы и constexpr                 | k + PascalCase      | `kMaxSize`                                            |
| Значения enum class                   | PascalCase      | `enum class Color { Red, Green, Blue };`           |
| Макросы                               | SNAKE_UPPER_CASE    | `TOTAL_COUNT`                                         |
| Пространства имён                     | PascalCase          | `namespace MySpace`                                   |
| Псевдонимы типов (`using`/`typedef`)  | PascalCase          | `using UserId = int32_t;`, `typedef std::vector<int> IntVector;` |
| Параметры шаблонов                    | PascalCase или одна заглавная буква | `template <typename ElementType>`, `template <class T>` |
| Директории и файлы исходного кода     | snake_lower_case    | `ui/my_form.cpp`                                      |

## EN

# Code Style Guide

| Entity                            | Style             | Example                                              |
|:---------------------------------|:------------------|:-----------------------------------------------------|
| Classes, structs, enums          | PascalCase        | `class UserAccount`, `struct Point3D`, `enum class ColorType` |
| Functions (global and methods)   | camelCase         | `getUserBalance()`, `calculateTotal()`, `setName()`   |
| Local variables and parameters   | camelCase         | `userName`, `totalCount`, `maxRetries`                |
| Class members                    | m_ + PascalCase   | `m_Name`, `m_Count`                                  |
| Constants and constexpr          | k + PascalCase    | `kMaxSize`                                           |
| Enum class values                | PascalCase    | `enum class Color { Red, Green, Blue };`          |
| Macros                           | SNAKE_UPPER_CASE  | `TOTAL_COUNT`                                        |
| Namespaces                       | PascalCase        | `namespace MySpace`                                  |
| Type aliases (`using`/`typedef`) | PascalCase        | `using UserId = int32_t;`, `typedef std::vector<int> IntVector;` |
| Template parameters              | PascalCase or single uppercase letter | `template <typename ElementType>`, `template <class T>` |
| Directories and source files     | snake_lower_case  | `ui/my_form.cpp`                                     |