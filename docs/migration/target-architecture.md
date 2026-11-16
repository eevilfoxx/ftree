# Target Architecture - ftree New Implementation

This document describes the target architecture and dependency direction for the new ftree implementation.

## Architecture Overview

The new implementation will preserve all functional capabilities of the old project while following modern C++17 practices and the architectural constraints listed in AGENT_RULES.md:

- **No ECS** - Traditional object-oriented design with clear separation of concerns
- **No multiprocessing** - Single-threaded simulation (as in old version)
- **No unnecessary interface layers** - Minimal abstraction
- **No Singleton for simulation state** - Simulation state passed explicitly
- **No mixing SFML with domain model** - SFML only in visualization layer
- **Typed IDs and events** - Proper type-safe IDs and event types instead of strings

## Layered Architecture

```
┌─────────────────────────────────────────────────────┐
│            Application / Main Loop                   │
│  (SFML window, event handling, game loop)           │
└─────────────────────────────────────────────────────┘
              ▲
              │
┌─────────────────────────────────────────────────────┐
│          Visualization Layer (SFML)                 │
│  - Visualizer (window, render loop)                 │
│  - Camera (view, zoom, pan)                         │
│  - Scene (tree layout, drawing)                     │
│  - UI (info panels, controls, status)               │
│  - VisualState (pause, year, auto-advance)          │
└─────────────────────────────────────────────────────┘
              ▲  ▲  ▲  ▲  ▲  ▲  ▲  ▲  ▲  ▲  ▲  ▲
              │  │  │  │  │  │  │  │  │  │  │  │
              │  │  │  │  │  │  │  │  │  │  │  └── Input events (sf::Event)
              │  │  │  │  │  │  │  │  │  │  └──── Simulation state queries
              │  │  │  │  │  │  │  │  │  └───────── Config access
              │  │  │  │  │  │  │  │  └─────────── Event system
              │  │  │  │  │  │  │  └─────────────── Domain model
              │  │  │  │  │  │  └────────────────── Services (Aging, Death, etc.)
              │  │  │  │  │  └──────────────────── NameManager (singleton, OK)
              │  │  │  │  └────────────────────── PopulationManager
              │  │  │  └──────────────────────── FamilyManager
              │  │  └──────────────────────────── RandomEngine (singleton, OK)
              │  └──────────────────────────────── Config (singleton, OK - but may need refactor)
              └──────────────────────────────── Entities (Person, Family)
```

## Dependency Direction

**Dependencies point inward (toward core domain):**

- `Visualization` → `Domain` (read-only access to entities, services, state)
- `Camera` → `Visualization` (SFML types only, no domain knowledge)
- `UI` → `Domain` (read-only access for info panels)
- `Visualizer` → `All lower layers` (orchestrates)
- `Simulation` → `Domain`, `Services`, `EventDispatcher`
- `Services` → `PopulationManager`, `FamilyManager`, `PersonRepository`, `EventDispatcher`, `Config`
- `PopulationManager` → `PersonRepository`, `EventDispatcher`, `Config`
- `FamilyManager` → `FamilyRepository`, `PersonRepository`, `EventDispatcher`
- `RandomEngine` → (no deps, standalone)
- `Config` → (no deps, singleton)
- `Person` → (no deps, plain data class)
- `Family` → (no deps, plain data class)

**No backward dependencies:**
- Domain entities → Visualization (SFML types not in entities)
- Services → Visualization/SFML (pure business logic)
- Config → any layer except as read access

## Core Classes and Responsibilities

| Class | Responsibility | New Location |
|-------|---------------|--------------|
| `Simulation` | Orchestrate yearly processing (pause, step, auto-advance); manage year counter; enable/disable visualization | `include/core/Simulation.hpp` |
| `Person` | Person data (uuid, gender, age, mother, father, family, pregnant, alive); `tryConceive()`, `tryDie()`, `incrementAge()` | `include/entities/Person.hpp` |
| `Family` | Family data (id, mother, father, children vector, surname); `addChild()`, `removeChild()` | `include/entities/Family.hpp` |
| `PopulationManager` | Manage persons: add/remove/age/find; findEligiblePartners(); getLivingPopulation(); age range queries | `include/managers/PopulationManager.hpp` |
| `FamilyManager` | Manage families: create/dissolve; add/remove children; getAllFamilies(); getFamilyMembers | `include/managers/FamilyManager.hpp` |
| `NameManager` | Load/store names and surnames; random name/surname selection (singleton) | `include/managers/NameManager.hpp` |
| `RandomEngine` | Mersenne twister random number engine (singleton) with randomInt, randomBool, randomDouble | `include/core/RandomEngine.hpp` |
| `Config` | Key-value configuration with type-safe getters; loadFromFile; singleton | `include/core/Config.hpp` |
| `EventDispatcher` | Subscribe/publish event system with typed EventType enum | `include/core/EventDispatcher.hpp` |
| `AgingService` | Process aging for all living persons | `include/services/AgingService.hpp` |
| `DeathService` | Process deaths for all living persons; publish PersonDied events | `include/services/DeathService.hpp` |
| `MarriageService` | Find eligible singles; attempt marriage based on probability; publish events | `include/services/MarriageService.hpp` |
| `PregnancyService` | Process pregnancies for all families; attempt conception; publish events | `include/services/PregnancyService.hpp` |
| `BirthService` | Process births for pregnant women; create children (1-3); publish events | `include/services/BirthService.hpp` |
| `StatisticsService` | Print population statistics by age group (children/adults/elders) | `include/utils/StatisticsService.hpp` |
| `Camera` | SFML view management: zoom, pan, reset; keyboard/mouse event handling | `include/vizualization/Camera.hpp` |
| `Scene` | Tree layout calculation; draw persons and connections; person at position hit-test | `include/vizualization/Scene.hpp` |
| `UI` | Info panels, controls hint, simulation status; resize handling | `include/vizualization/UI.hpp` |
| `Visualizer` | Main SFML window, render loop, event handling, simulation state connection | `include/vizualization/Visualizer.hpp` |

## Key Algorithms to Preserve

### Mortality
`P(death) = mortality_a * exp(mortality_b * age)` with `mortality_a=0.001, mortality_b=0.0693`

### Fertility (for conception)
```
fertility(age) = 
  0, if age < 18
  1.0, if 18 <= age <= 30
  0.7, if 30 < age < 40
  0.3, if 40 <= age < 45
  0.0, if age >= 45
```

### Conception probability
`P(conceive) = ((fertility(mother_age) + fertility(father_age)) / 2.0) * base_conception_prob`

### Marriage probability
`P(marry) = exp(-age_diff_factor * ageDiff)` with `age_diff_factor=0.1`

### Birth multiplicity
- If `randomDouble(0,1) < prob_triple` (0.01): triplets (3 children)
- Else if `randomDouble(0,1) < prob_triple + prob_twin` (0.01+0.09=0.10): twins (2 children)
- Else: single child (1 child)

### Child gender
`randomBool() ? Male : Female` (50/50)

## Configuration Keys (must be preserved)

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

## Migration Plan - Dependency Order

1. **Foundational** (no deps from other new classes):
   - `RandomEngine` - standalone
   - `Config` - singleton, can be refactored later
   - `Person` - pure data
   - `Family` - pure data

2. **Managers** (depend on foundational):
   - `NameManager` - singleton, minor refactor possible
   - `EventDispatcher` - standalone event system
   - `PopulationManager` - depends on RandomEngine, EventDispatcher, PersonRepository
   - `FamilyManager` - depends on FamilyRepository, PersonRepository, EventDispatcher

3. **Services** (depend on managers):
   - `AgingService` - depends on PopulationManager
   - `DeathService` - depends on PopulationManager, EventDispatcher
   - `MarriageService` - depends on FamilyManager, PopulationManager, PersonRepository, EventDispatcher
   - `PregnancyService` - depends on FamilyManager, PersonRepository, EventDispatcher
   - `BirthService` - depends on PopulationManager, FamilyManager, PersonRepository, EventDispatcher
   - `StatisticsService` - depends on PopulationManager, FamilyManager

4. **Visualization** (depend on all above):
   - `Camera` - SFML types only
   - `Scene` - depends on FamilyManager, PopulationManager, PersonRepository (read-only)
   - `UI` - depends on PopulationManager, FamilyManager, IPersonRepository (read-only)
   - `Visualizer` - depends on all above (window, camera, scene, ui)

5. **Application layer**:
   - `Simulation` - ties together domain + visualization
   - `main()` - entry point

## What MUST NOT Change (from behavior-contract.md)

- All formulas (mortality, fertility, marriage probability, birth multiplicity)
- All probabilities (default config values)
- All age constraints (founder age, marriage age, conception age limits)
- Annual cycle sequence (aging → deaths → marriages → pregnancies → births → statistics)
- Event types and their data keys
- All UI control mappings (Space=pause, Right Arrow=step, A=auto-advance, etc.)
- Visual rendering behavior (colors, layout algorithm)
- Configuration file format and key names
