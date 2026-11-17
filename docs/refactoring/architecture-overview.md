# Architectural Overview — ftree

## Overview

ftree is a C++17 family tree simulation with CMake build system and optional SFML visualization. The simulation models life, death, marriage, pregnancy, birth, and aging of people within family structures.

## Layers

```
┌─────────────────────────────────────────────────────┐
│            Application / Main Loop                   │
│  (SFML window, event handling, game loop)           │
└─────────────────────────────────────────────────────┘
               ▲
               │
┌─────────────────────────────────────────────────────┐
│          Visualization Layer (SFML, optional)       │
│  - Visualizer (window, render loop)                 │
│  - Camera (view, zoom, pan)                         │
│  - Scene (tree layout, drawing)                     │
│  - UI (info panels, controls, status)               │
└─────────────────────────────────────────────────────┘
               │ (read-only access to domain)
               ▼
┌─────────────────────────────────────────────────────┐
│           Simulation Layer                           │
│  - Simulation (orchestrates annual cycle)           │
│  - AgingService, DeathService, MarriageService,     │
│    PregnancyService, BirthService, StatisticsService│
└─────────────────────────────────────────────────────┘
               │
               ▼
┌─────────────────────────────────────────────────────┐
│           Manager Layer                              │
│  - PopulationManager (persons, aging, partners)      │
│  - FamilyManager (families, children, dissolution)   │
│  - NameManager (names, surnames, Singleton)          │
└─────────────────────────────────────────────────────┘
               │
               ▼
┌─────────────────────────────────────────────────────┐
│           Repository Layer                           │
│  - IPersonRepository / MemoryPersonRepository        │
│  - IFamilyRepository / MemoryFamilyRepository        │
└─────────────────────────────────────────────────────┘
               │
               ▼
┌─────────────────────────────────────────────────────┐
│           Domain Layer                               │
│  - Person, Family, Gender                            │
│  - Config (Singleton)                                │
│  - EventDispatcher (8 event types)                   │
└─────────────────────────────────────────────────────┘
```

## Dependency Direction

Dependencies point inward (toward the core domain):

- **Visualization** → Domain (read-only access to entities, services, state)
- **Simulation** → Domain, Services, EventDispatcher
- **Services** → PopulationManager, FamilyManager, IPersonRepository, EventDispatcher, Config
- **PopulationManager** → IPersonRepository, EventDispatcher, Config
- **FamilyManager** → IFamilyRepository, IPersonRepository, EventDispatcher
- **RandomEngine** → (no dependencies, standalone)
- **Config** → (no dependencies, Singleton)
- **Person** → (no dependencies, plain data class)
- **Family** → (no dependencies, plain data class)

## Key Design Decisions

1. **No ECS**: Traditional object-oriented design with clear separation of concerns
2. **No multiprocessing**: Single-threaded simulation (as in old version)
3. **No Singleton for simulation state**: State passed explicitly via constructors
4. **No mixing SFML with domain model**: SFML only in visualization layer
5. **Typed IDs**: `PersonId = uint32_t`, `FamilyId = uint32_t` in domain layer
6. **Event system**: 8 event types (PersonBorn, PersonDied, PersonMoved, Marriage, Pregnancy, Childbirth, FamilyCreated, FamilyDissolved)
7. **Repository pattern**: MemoryPersonRepository, MemoryFamilyRepository with mutex protection
8. **Dependency injection**: All services and managers receive dependencies via constructor

## Annual Simulation Cycle

`Simulation::processSingleYear()` executes in this order:
1. **AgingService** — increment age for all living persons
2. **DeathService** — process deaths based on mortality probability
3. **MarriageService** — match single persons into marriages
4. **PregnancyService** — attempt conception for married couples
5. **BirthService** — process births for pregnant women
6. **StatisticsService** — print population statistics

## Configuration Keys

| Key | Type | Default | Section |
|-----|------|---------|---------|
| `min_founder_age` | int | 20 | simulation |
| `max_founder_age` | int | 40 | simulation |
| `mortality_a` | double | 0.001 | simulation |
| `mortality_b` | double | 0.0693 | simulation |
| `base_conception_prob` | double | 0.3 | simulation |
| `min_marriage_age` | int | 18 | simulation |
| `age_diff_factor` | double | 0.1 | simulation |
| `prob_triple` | double | 0.01 | simulation |
| `prob_twin` | double | 0.09 | simulation |
| `max_person_age` | int | 100 | simulation |
| `male_names_file` | string | male_names.txt | paths |
| `female_names_file` | string | female_names.txt | paths |
| `surnames_file` | string | surnames.txt | paths |