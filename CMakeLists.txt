cmake_minimum_required(VERSION 3.15)
project(FamilyTree VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(BUILD_TESTING "Собрать тесты" ON)

function(ftree_enable_warnings target)
    if(MSVC)
        target_compile_options(${target} PRIVATE /W4 /permissive-)
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic)
    endif()
endfunction()

# SFML is optional so the domain tests can be built on machines without it.
# CONFIG prefers the package config shipped by vcpkg and SFML distributions.
find_package(SFML 2.5 COMPONENTS graphics window system CONFIG QUIET)

set(DOMAIN_SOURCES
    src/core/RandomEngine.cpp
    src/entities/Person.cpp
    src/entities/Family.cpp
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

set(VIZ_SOURCES
    src/application/Application.cpp
    src/vizualization/Visualizer.cpp
    src/vizualization/Camera.cpp
    src/vizualization/UI.cpp
    src/vizualization/Scene.cpp
)

if(SFML_FOUND)
    add_executable(FamilyTree
        src/main.cpp
        ${DOMAIN_SOURCES}
        ${VIZ_SOURCES}
    )

    target_include_directories(FamilyTree PRIVATE include)
    ftree_enable_warnings(FamilyTree)

    # SFML 2.x packages expose either namespaced or legacy target names,
    # depending on the distributor/package manager.
    foreach(component Graphics Window System)
        string(TOLOWER "${component}" component_lower)

        if(TARGET SFML::${component})
            list(APPEND SFML_LINK_TARGETS SFML::${component})
        elseif(TARGET sfml-${component_lower})
            list(APPEND SFML_LINK_TARGETS sfml-${component_lower})
        else()
            message(FATAL_ERROR
                "SFML was found, but target for ${component} is missing. "
                "Available SFML targets: ${SFML_LINK_TARGETS}"
            )
        endif()
    endforeach()

    target_link_libraries(FamilyTree PRIVATE ${SFML_LINK_TARGETS})
else()
    message(WARNING "SFML не найден — приложение FamilyTree не будет собрано. Тесты остаются доступны.")
endif()

if(SFML_FOUND)
    install(TARGETS FamilyTree DESTINATION bin)
endif()

# ============================================================================
# Тестирование (GoogleTest)
# ============================================================================
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
        src/entities/Person.cpp
        src/entities/Family.cpp
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
    ftree_enable_warnings(FamilyTreeTests)

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