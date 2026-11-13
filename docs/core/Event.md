# Документация класса Event

## Обзор

Класс `Event` представляет событие в системе симуляции. Каждое событие имеет определенный тип и может содержать дополнительные данные в виде пар "ключ-значение". Это гибкая система для отслеживания и реагирования на различные происшествия в симуляции.

## Перечисление EventType

Типы событий, поддерживаемые системой:

- **PersonBorn** - рождение нового человека
- **PersonDied** - смерть человека
- **PersonMoved** - перемещение человека
- **Marriage** - заключение брака
- **Pregnancy** - наступление беременности
- **Childbirth** - роды
- **FamilyCreated** - создание новой семьи
- **FamilyDissolved** - распад семьи

## Декларация класса

```cpp
class Event {
public:
    // Конструктор
    Event(EventType type);
    
    // Методы для работы с данными события
    void setData(const std::string& key, const std::string& value);
    std::optional<std::string> getData(const std::string& key) const;
    std::string getDataOrDefault(const std::string& key, const std::string& defaultValue = "") const;
    
    // Получение типа события
    EventType getType() const;

private:
    EventType type;
    std::unordered_map<std::string, std::string> data;
};
```

## Детали методов

### Конструктор

#### `Event(EventType type)`
Создает событие указанного типа.

**Параметры**:
- `type`: Тип создаваемого события

### Методы работы с данными

#### `void setData(const std::string& key, const std::string& value)`
Устанавливает дополнительные данные для события.

**Параметры**:
- `key`: Ключ для сохранения данных
- `value`: Значение в строковом формате

#### `std::optional<std::string> getData(const std::string& key) const`
Получает данные по ключу. Возвращает `std::nullopt` если ключ не найден.

**Параметры**:
- `key`: Ключ для поиска данных

**Возвращает**: `std::optional` с найденным значением или `std::nullopt`

#### `std::string getDataOrDefault(const std::string& key, const std::string& defaultValue = "") const`
Получает данные по ключу или значение по умолчанию если ключ не найден.

**Параметры**:
- `key`: Ключ для поиска данных
- `defaultValue`: Значение, возвращаемое если ключ не найден (по умолчанию пустая строка)

**Возвращает**: Найденное значение или `defaultValue`

#### `EventType getType() const`
Возвращает тип события.

**Возвращает**: `EventType` - тип текущего события

## Примеры использования

### Создание события

```cpp
// Создание события рождения
Event birthEvent(EventType::PersonBorn);

// Добавление данных о событии
birthEvent.setData("person_id", "12345");
birthEvent.setData("birth_date", "2024-01-15");
birthEvent.setData("location", "City_A");
birthEvent.setData("gender", "male");
```

### Чтение события

```cpp
// Обработка события
void handleEvent(const Event& event) {
    switch(event.getType()) {
        case EventType::PersonBorn:
            // Безопасное получение данных
            auto personId = event.getData("person_id");
            if (personId) {
                std::cout << "Родился человек: " << *personId << std::endl;
            }
            
            // Получение с значением по умолчанию
            auto location = event.getDataOrDefault("location", "Неизвестно");
            std::cout << "Место: " << location << std::endl;
            break;
    }
}
```

### Примеры данных для разных типов событий

```cpp
// Событие смерти
Event deathEvent(EventType::PersonDied);
deathEvent.setData("person_id", "12345");
deathEvent.setData("death_date", "2024-12-01");
deathEvent.setData("cause", "old_age");
deathEvent.setData("age", "85");

// Событие брака
Event marriageEvent(EventType::Marriage);
marriageEvent.setData("person1_id", "12345");
marriageEvent.setData("person2_id", "67890");
marriageEvent.setData("marriage_date", "2024-06-15");
marriageEvent.setData("location", "City_B");

// Событие беременности
Event pregnancyEvent(EventType::Pregnancy);
pregnancyEvent.setData("mother_id", "12345");
pregnancyEvent.setData("start_date", "2024-03-01");
pregnancyEvent.setData("expected_birth_date", "2024-12-01");
```

## Рекомендации по использованию

### Ключи данных

Для обеспечения консистентности рекомендуется использовать стандартные ключи:

- `person_id` - идентификатор человека
- `family_id` - идентификатор семьи
- `location` - местоположение
- `date` - дата события
- `age` - возраст
- `cause` - причина события

### Обработка событий

```cpp
// Пример обработчика событий
class EventProcessor {
public:
    void process(const Event& event) {
        auto type = event.getType();
        
        // Использование getData для проверки наличия данных
        if (auto personId = event.getData("person_id")) {
            std::cout << "Событие для человека: " << *personId << std::endl;
        }
        
        // Использование getDataOrDefault для гарантированного получения значения
        std::string date = event.getDataOrDefault("date", "Неизвестно");
    }
};
```

## Примечания

- Класс не является потокобезопасным
- Данные хранятся только в строковом формате, преобразование к другим типам должно выполняться на стороне обработчика
- Для сложных структур данных рекомендуется использовать сериализацию в JSON или другой формат