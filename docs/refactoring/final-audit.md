# Финальный аудит архитектуры — ftree

## 1. Сборка

| Проверка | Статус | Комментарий |
|----------|--------|-------------|
| Чистая сборка (CMake configure) | ✅ Pass | SFML не найден — приложение FamilyTree не собирается, тесты доступны |
| Сборка тестов (FamilyTreeTests) | ✅ Pass | Все 76 тестов проходят без предупреждений |
| Запуск полного набора тестов (ctest) | ✅ Pass | 76/76 тестов прошли, 0 неудач |
| Предупреждения компилятора (-Wall -Wextra -Wpedantic) | ✅ Pass | Нет предупреждений |

Приложение `FamilyTree` не собирается из-за отсутствия SFML (`libsfml-dev`). Это ожидаемо — SFML опционален, тесты собираются независимо.

## 2. Архитектурный обзор

### Текущая структура

Проект содержит две параллельные реализации:

**Старая реализация (компилируется и тестируется):**
- `src/entities/` — `Person`, `Family` (с `int uuid`, `weak_ptr` для родителей/семьи)
- `src/managers/` — `PopulationManager`, `FamilyManager`, `NameManager` (Singleton)
- `src/services/` — `AgingService`, `DeathService`, `MarriageService`, `PregnancyService`, `BirthService`, `StatisticsService`
- `src/core/` — `RandomEngine` (Singleton), `Config` (Singleton), `EventDispatcher`
- `src/utils/` — `StatisticsService`
- Внедрены через singletons: `RandomEngine::getInstance()`, `Config::getInstance()`, `NameManager::getInstance()`

**Новая реализация (не компилируется, частично реализована):**
- `include/domain/` — `Person` (с `PersonId = uint32_t`), `Family`, `World`, `Gender`
- `include/core/Random.hpp` — `ftree::core::Random` (не Singleton)
- `include/core/SimulationConfig.hpp`, `NameDatabase.hpp`, `Event.hpp`, `SimulationClock.hpp`
- `src/core/Random.cpp`, `Event.cpp`, `NameDatabase.cpp`, `SimulationClock.cpp`, `SimulationConfig.cpp`
- `src/simulation/` — `AgingSystem`, `BirthSystem`, `DeathSystem`, `PregnancySystem` (MarriageSystem, StatisticsSystem отсутствуют)
- `src/application/Application.cpp`, `src/presentation/` — не компилируются (не в CMakeLists.txt)
- `tests/domain/domain_test.cpp`, `tests/core/core_test.cpp` — не компилируются (не в CMakeLists.txt)

### Направление зависимостей

Зависимости направлены к предметной модели (domain):
- Visualization → Domain (только для чтения)
- Simulation → Domain, Services, EventDispatcher
- Services → Managers, Repositories, EventDispatcher, Config
- Managers → Repositories, EventDispatcher
- Config → без зависимостей (Singleton)
- Entities → без зависимостей (plain data)

SFML не смешан с предметной моделью — визуализация в отдельном слое.

## 3. Качество реализации

### Что хорошо

- Типобезопасные ID (`PersonId = uint32_t`, `FamilyId = uint32_t`) в доменном слое
- Внедрение зависимостей (Dependency Injection) для сервисов и менеджеров
- Паттерн репозитория с `MemoryPersonRepository`/`MemoryFamilyRepository`
- EventDispatcher с 8 типами событий
- Потокобезопасность репозиториев через `std::mutex`
- Детерминированный генератор случайных чисел (`ftree::core::Random` с seed)
- Разделение целей сборки: приложение и тесты независимы
- SFML опционален, тесты не зависят от него

### Технический долг

| # | Проблема | Файл | Риск |
|---|----------|------|------|
| 1 | `src/core/Simulation.cpp` ссылается на несуществующие типы `PersonId` и `RandomNumberGenerator` | `src/core/Simulation.cpp` | Мёртвый код, не компилируется |
| 2 | Отсутствуют реализации `MarriageSystem.cpp`, `StatisticsSystem.cpp` | `src/simulation/` | Заголовки есть, но нет cpp-файлов |
| 3 | Новые исходные файлы не в CMakeLists.txt | `src/core/Random.cpp`, `src/simulation/*.cpp`, `src/application/`, `src/presentation/` | Новая реализация не собирается |
| 4 | Новые тестовые файлы не в CMakeLists.txt | `tests/domain/domain_test.cpp`, `tests/core/core_test.cpp` | Новые тесты не запускаются |
| 5 | Два класса `EventDispatcher` (глобальный и `ftree::core`) | `include/core/EventDispatcher.hpp`, `include/core/Event.hpp` | Путаница, дублирование |
| 6 | Два класса `Person` (старые `entities/Person.hpp` и новые `domain/Person.hpp`) | `include/entities/Person.hpp`, `include/domain/Person.hpp` | Дублирование сущностей |
| 7 | Относительные пути к ассетам и конфигу | `src/vizualization/Visualizer.cpp`, `src/application/Application.cpp` | Может не работать из другого рабочего каталога |

## 4. Регрессии

Сравнение поведения до и после изменений:

- **Формулы**: не изменены (mortality, fertility, marriage probability, conception probability)
- **Вероятности**: не изменены (базовые значения конфигурации сохранены)
- **Возрастные ограничения**: не изменены (founder age 20-40, marriage age 18, conception 18-45)
- **Последовательность операций**: не изменена (starzenie → смерти → браки → беременности → рождения → статистика)
- **Типы событий**: все 8 типов событий сохранены
- **Public API**: все публичные методы старой реализации сохранены

Нет регрессий в функциональности, которая покрыта тестами.

## 5. Выполненные исправления (ветка rewrite/architecture)

| Коммит | Описание | Дорожная карта |
|--------|----------|----------------|
| `bf70be8` | Переименовать `src/entites` в `src/entities` | Этап 12 |
| `92d5348` | Исправить форматирование скобок в `MemoryPersonRepository::update()` | Этап 9 |
| `060db79` | Добавить проверки на null для родителей в `PregnancyService` | Этап 6 |
| `14795db` | Сделать `EventDispatcher::publish()` потокобезопасным при реентратных вызовах | Этап 8 |
| `156b23f` | Вызывать `repository->update()` после `incrementAge()` в `AgingService` | Этап 13 |

## 6. Оставшиеся проблемы

| # | Проблема | Статус | Комментарий |
|---|----------|--------|-------------|
| 1 | SFML не установлен — приложение не собирается | Известно | SFML опционален, тесты unaffected |
| 2 | Новая реализация не компилируется (не в CMake) | Открыто | Требует интеграции новых исходников в сборку |
| 3 | Отсутствуют `MarriageSystem.cpp`, `StatisticsSystem.cpp` | Открыто | Заголовки существуют, реализация отсутствует |
| 4 | Относительные пути в новом коде | Открыто | Конкретно для некомпилируемого кода |
| 5 | Семья не растворяется при смерти супруга | Открыто | Этап 14 дорожной карты — изменение поведения |

## 7. Рекомендации

1. Интегрировать новые исходные файлы в `CMakeLists.txt` для сборки новой реализации
2. Добавить новые тестовые файлы (`domain_test.cpp`, `core_test.cpp`) в цель тестов
3. Реализовать отсутствующие `MarriageSystem.cpp` и `StatisticsSystem.cpp`
4. Единообразить два класса `EventDispatcher` и два класса `Person`
5. Заменить относительные пути на абсолютные или относительно исполняемого файла
6. Рассмотреть растворение семьи при смерти супруга (этап 14 дорожной карты)

## Дополнение от 2026-11-18: повторная проверка и исправления

Предыдущие выводы отражали состояние до интеграции доменной реализации. В последующем проходе:

- добавлены `src/simulation/MarriageSystem.cpp` и интеграция `MarriageSystem` в CMake;
- `StatisticsSystem.cpp` уже присутствовал и включён в `ftree_core`;
- добавлены в CMake новые доменные системы и тестовые исходники `tests/domain/domain_test.cpp`, `tests/core/core_test.cpp`;
- `ftree_core` теперь собирается отдельно от SFML;
- исправлены фиктивные/пустые реализации доменных операций смерти, зачатия и рождения;
- исправлены методы `SimulationConfig`, `Random::seed()` и `SimulationClock::reset()`;
- исправлены семейные связи и генерация ID.

**Проверенный статус в текущей среде:** `ftree_core` успешно собран GCC 14.2 с `-Wall -Wextra -Wpedantic`; принудительная линковка всех объектных файлов `ftree_core` успешна. Реальные GoogleTest и SFML отсутствуют, поэтому прежние показатели 76/76 не являются результатом текущего запуска и не подтверждают текущую версию. Подробности и ограничения перечислены в `docs/migration/progress.md`.
