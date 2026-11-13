# План рефакторинга — ftree

Этот план разделяет рефакторинг на небольшие независимые этапы. Каждый этап должен быть завершен, протестирован и закоммичен перед началом следующего. Полный перевод не предлагается.

## Этап 0: Подготовка (без изменений исходного кода)

- [ ] Установить SFML (`libsfml-dev`) и проверить сборку
- [ ] docs/refactoring/architecture-audit.md создано (готово)
- [ ] docs/refactoring/dependency-graph.md создано (готово)
- [ ] docs/refactoring/refactoring-roadmap.md создано (этот документ)

## Этап 1: Инфраструктура тестирования (Коммит: test(build): establish automated testing infrastructure)

- [ ] Интегрировать GoogleTest через CMake (FetchContent или пакетный менеджер)
- [ ] Добавить отдельную цель тестов `FamilyTreeTests` в CMakeLists.txt
- [ ] Проверить сборку и тесты в чистом каталоге сборки
- [ ] Создать docs/refactoring/progress.md

## Этап 2: Базовые тесты доменной логики (Коммит: test(domain): add basic unit tests for core entities)

- [ ] Тест для `Person`: старение, беременность, смерть, семейный статус
- [ ] Тест для `Family`: добавить/удалить детей, получить родителей
- [ ] Тест для `Gender`: значения перечисления
- [ ] Тесты для `PopulationManager`: `findEligiblePartners`, `areRelatives`, `getLivingPopulation`
- [ ] Тесты для `FamilyManager`: `createFamily`, `dissolveFamily`, `addChildToFamily`
- [ ] Тесты для `MemoryPersonRepository` и `MemoryFamilyRepository`: операции CRUD

## Этап 3: Рефакторинг конфигурации и случайных чисел (Коммит: refactor(core): extract config and random engine from singletons)

- [ ] Перевести `Config` из Singleton на явное внедрение зависимостей (или至少 сделать потокобезопасным с мьютексом)
- [ ] Перевести `RandomEngine` из Singleton на явное внедрение зависимостей (или сделать потокобезопасным)
- [ ] Перевести `NameManager` из Singleton на явное внедрение зависимостей (или сделать потокобезопасным)
- [ ] Все сервисы и менеджеры, использующие Singletons, перевести на инъекцию

## Этап 4: Исправление генерации ID (Коммит: refactor(domain): introduce monotonic ID generator)

- [ ] Ввести отдельный класс генератора ID для персон и семей
- [ ] Заменить `size() + 1` на монотонно растущий счётчик в:
  - `Simulation::initialize`
  - `BirthService::processBirths`
  - `FamilyManager::createFamily`
- [ ] Добавить тесты на уникальность ID после удалений

## Этап 5: Исправление DeathService (Коммит: fix(domain): remove deceased persons from repository)

- [ ] `DeathService::processDeaths()` удалять мёртвых персон из репозитория (`removePerson`)
- [ ] Либо гарантировать, что все сервисы используют только `getLiving()`
- [ ] Добавить тесты на удаление мёртвых персон

## Этап 6: Null-безопасность PregnancyService (Коммит: fix(domain): add null checks in PregnancyService)

- [ ] Добавить проверки на null для `family->getMother()` и `family->getFather()`
- [ ] Добавить тесты для семей с отсутствующими родителями

## Этап 7: Исправление условия возраста в MarriageService (Коммит: fix(domain): correct age eligibility condition)

- [ ] Исправить условие в `findEligiblePartners` с `&&` на `||`
- [ ] Добавить тесты на возрастные границы при выборе партнёра

## Этап 8: Повторная безопасность EventDispatcher (Коммит: refactor(core): make EventDispatcher reentrant-safe)

- [ ] Переделать `publish()` так, чтобы мьютекс освобождался перед вызовом обработчиков
- [ ] Сделать копию обработчиков, затем вызвать их вне мьютекса

## Этап 9: Очистка MemoryPersonRepository::update (Коммит: refactor(domain): fix MemoryPersonRepository::update formatting)

- [ ] Прояснить и исправить структуру скобок в `update()`
- [ ] Привести единое форматирование кода

## Этап 10: Исправление относительных путей (Коммит: fix(build): resolve relative paths for config and assets)

- [ ] Заменить относительные пути (`../config/...`, `../assets/...`) на абсолютные или относительно exe-пути
- [ ] Либо задокументировать зависимость от рабочего каталога

## Этап 11: Режим симуляции без интерфейса (Коммит: feat(simulation): add headless mode without user input)

- [ ] Сделать `simulateYears()` выполнимым без ввода пользователя (без `std::cin.ignore()`)
- [ ] Опционально: параметр командной строки для headless-режима

## Этап 12: Переименование каталога (Коммит: refactor(build): rename src/entites to src/entities)

- [ ] Переименовать каталог `src/entites/` в `src/entities/`
- [ ] Обновить CMakeLists.txt
- [ ] Проверить пути включений

## Этап 13: Обновление репозитория в AgingService (Коммит: refactor(domain): update repository after aging)

- [ ] `AgingService::processAging()` вызывать `personRepository->update(person)` после `incrementAge()`
- [ ] Обеспечить консистентное использование паттерна репозитория

## Этап 14: Растворение семьи при смерти (Коммит: refactor(domain): dissolve family on spouse death)

- [ ] `DeathService` при смерти супруга растворять семью или удалять партнёра
- [ ] Добавить тесты на растворение семьи при смерти

## Зависимости между этапами

```
Этап 0 (Подготовка)
    ↓
Этап 1 (Тест-инфраструктура)  ← блокирует все последующие этапы
    ↓
Этап 2 (Базовые тесты)          ← валидирует существующее поведение
    ↓
Этап 3 (Config/Random/Name)  ← может параллельно с 4, 5, 6, 7
    ↳
Этап 4 (Генерация ID)      ← зависит от 2 (тесты на уникальность ID)
    ↳
Этап 5 (DeathService)        ← зависит от 2
    ↳
Этап 6 (PregnancyService)    ← зависит от 2
    ↳
Этап 7 (MarriageService)     ← зависит от 2
    ↳
Этап 8 (EventDispatcher)     ← может работать параллельно
    ↳
Этап 9 (Формат репозитория)   ← может работать параллельно
    ↳
Этап 10 (Относительные пути)     ← может работать параллельно
    ↳
Этап 11 (Headless-режим)     ← зависит от 1 (тесты)
    ↳
Этап 12 (Переименование каталога) ← может работать параллельно
    ↳
Этап 13 (AgingService)       ← зависит от 2
    ↳
Этап 14 (Растворение семьи)  ← зависит от 2, 5
```

## Определение готовности (Definition of Done) для каждого этапа

- [ ] Код компилируется без предупреждений (-Wall -Wextra -Wpedantic)
- [ ] Все существующие тесты проходят
- [ ] Новые тесты для этапа добавлены и проходят
- [ ] `git diff` проверен
- [ ] Создан conventional commit
- [ ] docs/refactoring/progress.md обновлён
