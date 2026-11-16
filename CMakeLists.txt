cmake_minimum_required(VERSION 3.15)
project(FamilyTree VERSION 1.0.0 LANGUAGES CXX)

# Установка стандарта C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Поиск SFML (опционально — тесты не требуют SFML)
find_package(SFML 2.5 COMPONENTS graphics window system QUIET)
set(SFML_FOUND ${SFML_FOUND} CACHE INTERNAL "SFML found")

# Настройка флагов компиляции
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0")
    else()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")
    endif()
endif()

# Исходные файлы предметной области (без SFML)
set(DOMAIN_SOURCES
    src/core/RandomEngine.cpp
    src/entites/Person.cpp
    src/entites/Family.cpp
    src/core/Config.cpp
    src/core/EventDispatcher.cpp
    src/managers/FamilyManager.cpp
    src/managers/PopulationManager.cpp
    src/managers/NameManager.cpp
    src/services/AgingService.cpp
    src/services/BirthService.cpp
    src/services/DeathService.cpp
    src/services/MarriageService.cpp
    src/services/PregnancyService.cpp
    src/utils/StatisticsService.cpp
)

# Исходные файлы визуализации (требуют SFML)
set(VIZ_SOURCES
    src/vizualization/Visualizer.cpp
    src/vizualization/Camera.cpp
    src/vizualization/UI.cpp
    src/vizualization/Scene.cpp
)

# Создаем цель исполняемого файла
if(SFML_FOUND)
    add_executable(FamilyTree.exe
        src/main.cpp
        ${DOMAIN_SOURCES}
        ${VIZ_SOURCES}
    )

    target_include_directories(FamilyTree.exe PRIVATE include)

    target_link_libraries(FamilyTree.exe PRIVATE
        sfml-graphics
        sfml-window
        sfml-system
    )

    # Для Windows: копируем DLL файлы SFML рядом с исполняемым файлом
    if(WIN32)
        get_target_property(SFML_LIB_DIR SFML::Graphics LOCATION)
        get_filename_component(SFML_LIB_DIR ${SFML_LIB_DIR} DIRECTORY)

        add_custom_command(TARGET FamilyTree.exe POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${SFML_LIB_DIR}/sfml-graphics-2.dll"
                "${SFML_LIB_DIR}/sfml-window-2.dll"
                "${SFML_LIB_DIR}/sfml-system-2.dll"
                $<TARGET_FILE_DIR:FamilyTree>
        )
    endif()
else()
    message(WARNING "SFML не найден — основное приложение FamilyTree.exe не будет собрано. Тесты будут собраны без SFML.")
endif()

# Устанавливаем свойства цели основного приложения
if(SFML_FOUND)
    set_target_properties(FamilyTree.exe PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
    )
endif()

# Опционально: установка исполняемого файла
if(SFML_FOUND)
    install(TARGETS FamilyTree.exe DESTINATION bin)
endif()

# ============================================================================
# Тестирование (GoogleTest)
# ============================================================================
option(BUILD_TESTING "Собрать тесты" ON)

if(BUILD_TESTING)
    enable_testing()
    find_package(GTest REQUIRED)

    # Отдельная цель для тестов — без зависимости от SFML
    add_executable(FamilyTreeTests
        tests/test_gender.cpp
        tests/test_person.cpp
        tests/test_family.cpp
        tests/test_repositories.cpp
        tests/test_population_manager.cpp
        tests/test_family_manager.cpp
        tests/test_aging_service.cpp
        tests/test_death_service.cpp
        tests/test_pregnancy_service.cpp
        tests/test_birth_service.cpp
        tests/test_marriage_service.cpp
        tests/test_statistics_service.cpp
        tests/test_formulas.cpp
        # Исходные файлы предметной области (без SFML и без main)
        src/core/RandomEngine.cpp
        src/entites/Person.cpp
        src/entites/Family.cpp
        src/core/Config.cpp
        src/core/EventDispatcher.cpp
        src/managers/PopulationManager.cpp
        src/managers/FamilyManager.cpp
        src/managers/NameManager.cpp
        src/services/AgingService.cpp
        src/services/DeathService.cpp
        src/services/MarriageService.cpp
        src/services/PregnancyService.cpp
        src/services/BirthService.cpp
        src/utils/StatisticsService.cpp
    )

    target_include_directories(FamilyTreeTests PRIVATE include)

    target_link_libraries(FamilyTreeTests PRIVATE
        GTest::gtest_main
        GTest::gtest
    )

    set_target_properties(FamilyTreeTests PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
    )

    # Регистрация тестов в CTest
    include(GoogleTest)
    gtest_discover_tests(FamilyTreeTests)
endif()