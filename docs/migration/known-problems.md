# Known Problems - ftree Old Project

This document fixes discovered errors, contradictions, and unfinished functions in the old implementation.

## Errors and Contradictions

| # | Problem | File | Description | Impact |
|---|---------|------|-------------|--------|
| 1 | `tryDie()` mortality rate may not match expected probability | Person.cpp | The `tryDie()` method uses `mortality_a * exp(mortality_b * age)` with default values `mortality_a=0.001, mortality_b=0.0693`. For age=80: `0.001 * exp(0.0693*80) = 0.001 * exp(5.544) ≈ 0.001 * 255.7 ≈ 0.256`. This means ~25.6% annual death rate for 80-year-olds, which may be higher than intended. | High - affects realism of simulation |
| 2 | `tryConceive()` age limits are strict | Person.cpp | `tryConceive()` returns `false` if `age > 45`, but the fertility function also returns `0.0` for age >= 45. However, there's no explicit upper bound check for males - a male of any age can attempt conception, but fertility function only handles ages up to 45 properly. | Medium - may cause unexpected behavior |
| 3 | Pregnancy flag not reset for male persons | Person.cpp | `setPregnant(bool)` only sets the flag if `gender == Gender::Female`; otherwise it's a no-op. However, `tryDie()` always sets `pregnant = false` regardless of gender. If a male is somehow marked pregnant (shouldn't happen but could be a bug), it wouldn't be cleared. | Low - edge case |
| 4 | `incrementAge()` does not age dead persons | Person.cpp | `incrementAge()`: `if (alive) { age++; }`. This means dead persons don't age, which is correct behavior, but the comment says "Age should not change when dead" - this is consistent. | Low - intentional behavior |
| 5 | MemoryRepository `getLiving()` method | MemoryPersonRepository.hpp/cpp | Not found in codebase search; `getLivingPopulation()` in PopulationManager calls `personRepository->getLiving()`. Need to verify this method exists in the repository interface. | Medium - may break compilation |
| 6 | `findEligiblePartners` may return relatives in some edge cases | PopulationManager.cpp | `areRelatives()` only checks shared mother or shared father via direct weak_ptr access. Does not check more distant relatives (e.g., cousins, uncle-niece). This may allow some related persons to marry. | Low - cultural/realism concern |
| 7 | Marriage service may not find partners when population is small | MarriageService.cpp | When `singlePersons.size() > 1` but all are same gender or all already married, no partners found. The shuffle + findEligiblePartners logic may not handle all edge cases. | Low - may result in no marriages when some should occur |
| 8 | Config file loading may fail silently | Config.cpp | `loadFromFile()` outputs warning but returns without error if file can't be opened. The simulation continues with default values, which may be unexpected. | Medium - users may wonder why settings aren't applied |
| 9 | Person name/surname not updated after certain operations | NameManager.hpp/cpp | Names are assigned once at construction via NameManager::getRandomName(); no mechanism to rename persons later. If family surname changes, individual names don't. | Low - cosmetic limitation |
| 10 | `Simulation::simulateYears()` is empty | Simulation.cpp | `simulateYears(int years)` has empty body: `// visualization optional - called from outside`. The actual year processing happens via `mainLoop()` or `processSingleYear()` called externally. | Medium - users may expect this method to run the simulation |

## Unfinished Functions

| # | Feature | File | Status | Notes |
|---|---------|------|--------|-------|
| 1 | Camera keyboard handling | Camera.hpp/cpp | Partially implemented | `handleKeyboardEvents` has `case sf::Keyboard::N: break;` and `case sf::Keyboard::M: break;` with no implementation. No key actions defined. |
| 2 | Visualizer auto-advance integration | Visualizer.hpp/cpp | Not fully integrated | `autoAdvance_` flag exists in Simulation but Visualizer's main loop doesn't call Simulation::mainLoop() or processSingleYear() automatically. |
| 3 | Event handlers for all event types | EventDispatcher.hpp/cpp | Complete | All 8 event types are defined and the dispatcher works, but no default event handlers are registered anywhere in the codebase. |
| 4 | Simulation main loop integration | Simulation.hpp/cpp | Not complete | `mainLoop()` exists but is not called automatically from Visualizer; requires external driver. |
| 5 | Config validation on load | Config.hpp/cpp | Not implemented | No validation of config values after loading; invalid values silently use defaults. |
| 6 | Death cause variability | DeathService.cpp | Partially implemented | Death cause is always "natural causes" hardcoded; no variability based on age or other factors. |
| 7 | Marriage event data completeness | MarriageService.cpp | Complete but limited | Marriage event has male_id, female_id, male_name, female_name but lacks marriage_date, location, etc. |
| 8 | Family dissolution UI | Visualizer.hpp/cpp | Not implemented | FamilyDissolved event published but no UI action to trigger dissolution. |
| 9 | PersonMoved event usage | Various | Not used | PersonMoved event published when children removed from family but no UI or further logic connects to it. |
| 10 | Gender enum extension capability | Gender.hpp | Not tested | Only Male=0, Female=1 defined; no mechanism for additional gender types. |

## UI/Visualization Gaps

| # | Gap | File | Impact |
|---|-----|------|--------|
| 1 | No keyboard navigation for person selection beyond H key | Visualizer.hpp | Users cannot navigate persons via keyboard alone |
| 2 | No search/filter functionality in UI | UI.hpp | No way to search for specific persons |
| 3 | No tooltips or detailed on-hover info beyond name/age | Scene.cpp | Hover only highlights; no additional info displayed |
| 4 | No print/export functionality | Visualizer.hpp/cpp | No way to save or print the family tree |
| 5 | Camera boundary limits | Camera.cpp | No min/max zoom beyond 0.1/5.0; no view boundary clamping |
| 6 | Double-click to select person | Visualizer.hpp/cpp | No double-click selection implemented |
| 7 | Right-click context menu | Visualizer.hpp/cpp | No context menu implemented |

