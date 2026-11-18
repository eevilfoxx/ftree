cmake_minimum_required(VERSION 3.15)

project(FamilyTree VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(BUILD_TESTING "Build tests" ON)

find_package(PostgreSQL REQUIRED)

function(ftree_enable_warnings target)
    if(MSVC)
        target_compile_options(${target} PRIVATE /W4 /permissive-)
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic)
    endif()
endfunction()

# The simulation and domain library has no SFML dependency. It is always
# available, including on headless machines and CI runners without graphics.
set(FTREE_CORE_SOURCES
    src/core/RandomEngine.cpp
    src/core/Random.cpp
    src/core/SimulationClock.cpp
    src/core/NameDatabase.cpp
    src/core/SimulationConfig.cpp
    src/core/Config.cpp
    src/core/EventDispatcher.cpp
    src/core/Event.cpp

    src/entities/Person.cpp
    src/entities/Family.cpp

    src/managers/FamilyManager.cpp
    src/managers/PopulationManager.cpp
    src/managers/NameManager.cpp

    src/services/AgingService.cpp
    src/services/BirthService.cpp
    src/services/DeathService.cpp
    src/services/MarriageService.cpp
    src/services/PregnancyService.cpp

    src/simulation/AgingSystem.cpp
    src/simulation/DeathSystem.cpp
    src/simulation/PregnancySystem.cpp
    src/simulation/MarriageSystem.cpp
    src/simulation/BirthSystem.cpp
    src/simulation/StatisticsSystem.cpp

    src/utils/StatisticsService.cpp
    src/presentation/WorldPresentation.cpp
    src/presentation/TreeLayout.cpp

    src/application/Application.cpp
    src/application/StartupPrompt.cpp
)

add_library(ftree_core STATIC ${FTREE_CORE_SOURCES})
target_include_directories(ftree_core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)
ftree_enable_warnings(ftree_core)

# The headless executable is always available and has no SFML dependency.
add_executable(FamilyTreeHeadless
    src/headless_main.cpp
)
add_library(ftree_infrastructure_postgres STATIC
    src/infrastructure/postgres/PostgresSimulationRepository.cpp
)
target_include_directories(ftree_infrastructure_postgres PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)
target_link_libraries(ftree_infrastructure_postgres PUBLIC
    ftree_core
    PostgreSQL::PostgreSQL
)
target_compile_definitions(ftree_infrastructure_postgres PRIVATE
    FTREE_MIGRATIONS_DIR="${PROJECT_SOURCE_DIR}/migrations"
)
ftree_enable_warnings(ftree_infrastructure_postgres)

# The headless executable also uses PostgreSQL as its persistent source of truth.
target_link_libraries(FamilyTreeHeadless PRIVATE
    ftree_core
    ftree_infrastructure_postgres
)
ftree_enable_warnings(FamilyTreeHeadless)

# SFML is optional: visualization is built only when SFML is available.
find_package(SFML 2.5 COMPONENTS graphics window system CONFIG QUIET)

if(SFML_FOUND)
    set(FTREE_SFML_TARGETS)
    foreach(component Graphics Window System)
        string(TOLOWER "${component}" component_lower)

        if(TARGET SFML::${component})
            list(APPEND FTREE_SFML_TARGETS SFML::${component})
        elseif(TARGET sfml-${component_lower})
            list(APPEND FTREE_SFML_TARGETS sfml-${component_lower})
        else()
            message(FATAL_ERROR
                "SFML was found, but target for ${component} is missing."
            )
        endif()
    endforeach()

    add_library(ftree_visualization STATIC
        src/visualization/SFMLVisualizer.cpp
        src/visualization/Camera.cpp
        src/visualization/UI.cpp
        src/visualization/Scene.cpp
    )
    target_include_directories(ftree_visualization PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    )
    target_link_libraries(ftree_visualization PUBLIC
        ftree_core
        ${FTREE_SFML_TARGETS}
    )
    ftree_enable_warnings(ftree_visualization)

    add_executable(FamilyTree src/main.cpp)
    target_link_libraries(FamilyTree PRIVATE
        ftree_core
        ftree_visualization
        ftree_infrastructure_postgres
    )
    ftree_enable_warnings(FamilyTree)

    install(TARGETS FamilyTree FamilyTreeHeadless ftree_infrastructure_postgres DESTINATION bin)
    install(DIRECTORY migrations/ DESTINATION share/ftree/migrations)
else()
    message(WARNING
        "SFML was not found: the graphical FamilyTree executable will not be built. "
        "FamilyTreeHeadless, ftree_core, and available tests can still be built."
    )
    install(TARGETS FamilyTreeHeadless ftree_infrastructure_postgres DESTINATION bin)
    install(DIRECTORY migrations/ DESTINATION share/ftree/migrations)
endif()

if(BUILD_TESTING)
    enable_testing()
    find_package(GTest QUIET)

    if(GTest_FOUND)
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
            tests/test_StatisticsSystem.cpp
            tests/domain/domain_test.cpp
            tests/core/core_test.cpp
            tests/test_PersonDisplay.cpp
            tests/test_FamilyDisplay.cpp
            tests/test_WorldPresentation.cpp
            tests/test_TreeLayout.cpp
            tests/domain/world_snapshot_test.cpp
            tests/core/random_state_test.cpp
            tests/infrastructure/postgres_repository_test.cpp
            tests/application/application_persistence_test.cpp
        )

        target_link_libraries(FamilyTreeTests PRIVATE
            ftree_core
            ftree_infrastructure_postgres
            GTest::gtest
            GTest::gtest_main
        )
        ftree_enable_warnings(FamilyTreeTests)
        
        target_compile_definitions(FamilyTreeTests PRIVATE
        FTREE_TEST_SOURCE_DIR="${PROJECT_SOURCE_DIR}"
        )

        include(GoogleTest)
        gtest_discover_tests(FamilyTreeTests)
    else()
        message(WARNING
            "GoogleTest was not found: FamilyTreeTests will not be built. "
            "Install GoogleTest or configure with BUILD_TESTING=OFF."
        )
    endif()
endif()
