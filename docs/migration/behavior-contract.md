# Behavior Contract - ftree Old Project

This document fixes the behavioral specifications that must be preserved in the new implementation.

## Formulas

| Formula | Description | Equation | Source File |
|---------|-------------|----------|-----------|
| Mortality probability | Probability of death in a year for a person of age `a` | `P(death) = mortality_a * exp(mortality_b * a)` | Person::tryDie(), Config: mortality_a=0.001, mortality_b=0.0693 |
| Conception probability | Probability of conception during a year | `P(conceive) = fert * base_conception_prob` | Person::tryConceive(), where `fert = (fertility(age) + fertility(partnerAge)) / 2.0` |
| Fertility function | Age-dependent fertility factor | `fertility(age) = 0` if age < 18; `1.0` if 18 <= age <= 30; `0.7` if 30 < age < 40; `0.3` if 40 <= age < 45; `0.0` if age >= 45 | Person::tryConceive(), Config: base_conception_prob=0.3 |
| Marriage probability | Probability two eligible persons marry | `P(marry) = exp(-age_diff_factor * ageDiff)` | MarriageService::processMarriages(), Config: age_diff_factor=0.1 |
| Twin probability | Probability of twins in a birth | Fixed probability from config | BirthService::processBirths(), Config: prob_twin=0.09 |
| Triplet probability | Probability of triplets in a birth | Fixed probability from config | BirthService::processBirths(), Config: prob_triple=0.01 |

## Probabilities

| Probability | Default Value | Config Key | Affects |
|-------------|--------------|------------|---------|
| base conception probability | 0.3 | `base_conception_prob` | Person::tryConceive(), PregnancyService |
| mortality_a | 0.001 | `mortality_a` | Person::tryDie(), DeathService |
| mortality_b | 0.0693 | `mortality_b` | Person::tryDie(), DeathService |
| age_diff_factor | 0.1 | `age_diff_factor` | MarriageService::processMarriages() |
| min marriage age | 18 | `min_marriage_age` | MarriageService::processMarriages() |
| max person age | 100 | `max_person_age` | MarriageService, PopulationManager |
| founder min age | 20 | `min_founder_age` | Simulation::initialize() |
| founder max age | 40 | `max_founder_age` | Simulation::initialize() |
| prob_triple | 0.01 | `prob_triple` | BirthService::processBirths() |
| prob_twin | 0.09 | `prob_twin` | BirthService::processBirths() |

## Age Constraints

| Constraint | Minimum | Maximum | Source |
|------------|---------|---------|--------|
| Founder age (male/female at initialization) | 20 | 40 | Config: min_founder_age, max_founder_age |
| Marriage eligibility (minimum) | 18 | N/A | Config: min_marriage_age |
| Marriage eligibility (maximum person age) | N/A | 100 | Config: max_person_age |
| Conception eligibility (female minimum age) | 18 | 45 | Person::tryConceive() |
| Conception eligibility (female maximum age) | N/A | 45 | Person::tryConceive() |
| Person maximum age (lifespan cap) | N/A | 100 | Config: max_person_age (used in getByAgeRange) |

## Annual Cycle Sequence

The simulation processes one year in the following order (via `Simulation::processSingleYear()`):

1. **Aging**: `AgingService::processAging()` - all living persons increment age by 1
2. **Deaths**: `DeathService::processDeaths()` - persons may die based on mortality probability; death events published
3. **Marriages**: `MarriageService::processMarriages()` - eligible singles matched; marriages created; marriage + family creation events published
4. **Pregnancies**: `PregnancyService::processPregnancies()` - conception attempted for all families; pregnancy events published
5. **Births**: `BirthService::processBirths()` - pregnant women give birth; children added (1-3 based on prob_twin/prob_triple); birth + childbirth events published
6. **Statistics**: `StatisticsService::printStatistics(year)` - population stats printed to console

## Family Creation Rules

| Rule | Description | Implementation |
|------|-------------|----------------|
| `createFamily(mother, father)` | Creates new family | FamilyManager::createFamily(); checks: mother is Female, father is Male, both alive; assigns surname; links parents; publishes FamilyCreated event |
| Dissolve family | Removes family links | FamilyManager::dissolveFamily(); clears mother/father weak_ptrs; publishes FamilyDissolved event |
| Add child to family | Child joins family | FamilyManager::addChildToFamily(); adds child to family's children vector; person's family weak_ptr set; publishes PersonBorn event |
| Remove child from family | Child leaves family | FamilyManager::removeChildFromFamily(); removes from family; person's family set to nullptr; publishes PersonMoved event |

## Partner Selection Rules

| Rule | Description | Implementation |
|------|-------------|----------------|
| Eligibility check | Person must be alive, not married, within age range | PopulationManager::findEligiblePartners(); checks: alive, not married, age >= min_marriage_age, age <= max_person_age |
| Gender matching | Partner must be opposite gender | findEligiblePartners(); targetGender = (person is Male ? Female : Male) |
| Relative check | Partners who are relatives are excluded | findEligiblePartners(); calls areRelatives() to filter out close relatives |
| Age difference probability | Marriage probability decreases with age difference | MarriageService::processMarriages(); probability = exp(-age_diff_factor * ageDiff); randomDouble(0,1) < probability |

## Pregnancy and Birth Rules

| Rule | Description | Implementation |
|------|-------------|----------------|
| Conception attempt | Each year, all families have conception attempted | PregnancyService::processPregnancies(); iterates all families; calls mother->tryConceive(father->getAge()) |
| Conception success | Probability based on both parents' ages and base_conception_prob | Person::tryConceive(); fertility(age) function: <18→0, 18-30→1.0, 30-40→0.7, 40-45→0.3, >=45→0.0; probability = fert * base_conception_prob |
| Multiple births | Twins or triplets based on config probabilities | BirthService::processBirths(); r = randomDouble(0,1); if r < probTriple → 3 children; else if r < probTriple + probTwin → 2 children; else → 1 child |
| Child gender | Random male/female with equal probability | BirthService::processBirths(); childGender = randomBool() ? Male : Female |
| Mother resets after birth | Pregnancy flag cleared after birth | BirthService::processBirths(); after all children born, mother->setPregnant(false) |
| Pregnancy while already pregnant | Skip; no double conception | PregnancyService::processPregnancies(); if mother->isPregnant() continue; tryConceive returns false if already pregnant |
| Dead mother prevents conception | Mother must be alive | PregnancyService::processPregnancies(); if !mother->isAlive() continue; Person::tryDie() sets pregnant=false |
| Father must be alive for conception | Father must be alive | PregnancyService::processPregnancies(); if !father->isAlive() continue; |

## Event Types and Data

| Event Type | Data Keys (key → value) | Published By |
|------------|------------------------|--------------|
| `PersonBorn` | `person_id`, `person_name`, `parent_id` (optional), `family_name` (optional) | PopulationManager::addPerson(), BirthService, FamilyManager::addChildToFamily() |
| `PersonDied` | `person_id`, `person_name`, `person_age`, `death_cause` | DeathService::processDeaths(), PopulationManager::removePerson() |
| `PersonMoved` | `family_id`, `child_id` | FamilyManager::removeChildFromFamily() |
| `Marriage` | `male_id`, `female_id`, `male_name`, `female_name` | MarriageService::processMarriages() |
| `Pregnancy` | `mother_id`, `mother_name`, `mother_age`, `father_id`, `father_name`, `father_age` | PregnancyService::processPregnancies() |
| `Childbirth` | `child_id`, `mother_id`, `father_id`, `child_name`, `family_name` | BirthService::processBirths() |
| `FamilyCreated` | `family_id`, `mother_id`, `mother_name`, `father_id`, `father_name`, `family_surname` | FamilyManager::createFamily() |
| `FamilyDissolved` | `family_id`, `mother_id`, `mother_name`, `father_id`, `father_name` | FamilyManager::dissolveFamily() |

## Visualization Behavior

| Aspect | Behavior |
|--------|----------|
| Person rendering | Alive persons: color by gender (male: RGB(100,150,255), female: RGB(255,150,200)); Deceased: RGB(150,150,150) |
| Text rendering | Name + age on each person; "Pregnant" tag for pregnant females; "Deceased" for dead persons |
| Family connections | Lines between family members (parent-child, spouse) |
| Layout algorithm | Tree layout by generations; sort by parents; center levels; horizontal/vertical spacing configurable |
| Hover selection | Click or hover over person; getPersonAtPosition returns UUID; setHoveredPerson sets highlight |
| Person selection | Click on person; setSelectedPerson; drawInfoPanel shows detailed info |
| Deceased display | Gray color; "Deceased" text; pregnant flag reset on death |
| Camera pan | Mouse drag pans the view; offset accumulates drag deltas |
| Camera zoom | Mouse wheel zooms; clamp min 0.1, max 5.0; R resets to defaults |
| Controls hint | Toggleable display at bottom listing all keys |
| Simulation status | Top panel shows: Year N [PAUSED]/[RUNNING] [AUTO] |

## Camera Management

| Operation | Key/Mouse | Effect | State Changed |
|-----------|-----------|--------|---------------|
| Pan view | Mouse drag (left button) | offset += delta in world coordinates | offset_ |
| Zoom in | Mouse wheel up / < key | zoomLevel_ *= 1.1 | zoomLevel_ (clamped [0.1, 5.0]) |
| Zoom out | Mouse wheel down / > key | zoomLevel_ /= 1.1 | zoomLevel_ (clamped [0.1, 5.0]) |
| Reset view | R key | offset_ = (0,0); zoomLevel_ = 1.0 | offset_, zoomLevel_ |
| Toggle controls | C key | showControls_ ^= true | showControls_ |
| Select hovered | H key | selectedId = hoveredId | selectedPersonId_ |
| Close window | Escape key | window_.close() | (none directly) |

## UI Management

| Component | Behavior |
|-----------|----------|
| Info panel | Right side panel: top line = population count + family count; below = selected person info (name, ID, age, gender, status, family ID) |
| Controls hint | Bottom list: Mouse Drag - Pan, Mouse Wheel - Zoom, < > Zoom, R Reset, C Toggle controls, H Select hovered, Space Pause/Resume, Right Arrow Step forward, A Toggle auto-advance, Up/Down Speed control |
| Simulation status | Top bar: "Year N [PAUSED]/[RUNNING]" + "[AUTO]" if autoAdvance |
| Resize handling | UI::handleResize updates uiView_ size and center maintains aspect ratio |
| Font loading | font.otf from assets; fallback to C:/Windows/Fonts/arial.ttf if not found |

