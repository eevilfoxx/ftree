cmake_minimum_required(VERSION 3.15)
project(FamilyTree VERSION 1.0.0 LANGUAGES CXX)

# Установка стандарта C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Поиск SFML
find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)

# Настройка флагов компиляции
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0")
    else()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")
    endif()
endif()

# Создаем цель исполняемого файла с правильным именем
add_executable(FamilyTree.exe
    src/main.cpp
    src/core/RandomEngine.cpp
    src/entites/Person.cpp
    src/entites/Family.cpp
    src/core/Simulation.cpp
    src/core/Config.cpp
    src/core/EventDispatcher.cpp
    src/managers/FamilyManager.cpp
    src/managers/PopulationManager.cpp
    src/managers/NameManager.cpp
    src/vizualization/Visualizer.cpp
    src/vizualization/Camera.cpp
    src/vizualization/UI.cpp
    src/vizualization/Scene.cpp
    src/services/AgingService.cpp
    src/services/BirthService.cpp
    src/services/DeathService.cpp
    src/services/MarriageService.cpp
    src/services/PregnancyService.cpp
    src/utils/StatisticsService.cpp
)

# Устанавливаем директории включения для цели
target_include_directories(FamilyTree.exe PRIVATE include)

# Подключаем SFML к цели
target_link_libraries(FamilyTree.exe PRIVATE 
    sfml-graphics 
    sfml-window 
    sfml-system
)

# Устанавливаем свойства цели
set_target_properties(FamilyTree.exe PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
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

# Опционально: установка исполняемого файла
install(TARGETS FamilyTree.exe DESTINATION bin)