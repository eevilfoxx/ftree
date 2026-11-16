# Feature Inventory - ftree Old Project

This document lists all capabilities of the old ftree project that must be preserved in the new implementation.

## Simulation Core

| # | Feature | Old File | Current Behavior | Inputs | Result | Related Settings | Check Method | Status |
|---|---------|----------|------------------|--------|--------|-------------------|--------------|--------|
| 1 | `simulateYears(int years)` | Simulation.hpp | Simulates the specified number of years, processing each year's events | `years` (int) | Advances simulation by N years; triggers aging, deaths, marriages, pregnancies, births | `simulateYears` calls `processSingleYear()` internally | Run simulation and verify year counter advances | Works |
| 2 | `initialize(int initialPopulation, RandomNumberGenerator& rng)` | Simulation.hpp | Initializes population with specified number of founders (pairs of male/female) | `initialPopulation` (int), `rng` (RandomReference) | Creates `initialPopulation/2` families with founder couples | `min_founder_age`, `max_founder_age` from config | Check person count and family count after initialize | Works |
| 3 | `processSingleYear()` | Simulation.cpp | Process one complete year: aging → deaths → marriages → pregnancies → births → statistics | None (internal state) | Year increments; all services process in order; statistics printed | All service configs (mortality, conception, marriage) | Run single year and verify all services execute | Works |
| 4 | `enableVisualization(bool enable)` | Simulation.hpp | Enable or disable visualization mode | `enable` (bool) | Toggles visualization on/off; deferred to outside | Visualizer creation deferred to outside | Call enableVisualization and check if visualization mode changes | Works |
| 5 | Pause/Resume control | Simulation.hpp | `simulationPaused_` flag controls whether simulation advances | `simulationPaused_` (bool, default true) | When paused, mainLoop does not advance simulation; Space key toggles | None directly; handled via events | Press Space in running app and verify pause/unpause | Works |
| 6 | Step forward functionality | Simulation.hpp | `stepRequested_` flag for single-step advancement | `stepRequested_` (bool) | Right Arrow key triggers one year advance; used in paused state | None directly | Press Right Arrow and verify one year advances | Works |
| 7 | Auto-advance mode | Simulation.hpp | `autoAdvance_` flag for continuous simulation | `autoAdvance_` (bool, default false) | A key toggles auto-advance; simulation advances when not paused | None directly | Press A key and verify auto-advance on/off | Works |

## Population Management

| # | Feature | Old File | Current Behavior | Inputs | Result | Related Settings | Check Method | Status |
|---|---------|----------|------------------|--------|--------|-------------------|--------------|--------|
| 8 | `addPerson(std::shared_ptr<Person>)` | PopulationManager.hpp | Adds person to repository; publishes PersonBorn event | `person` (shared_ptr) | Person stored; event dispatched | None | Add person and verify in repository + event | Works |
| 9 | `removePerson(int uuid)` | PopulationManager.hpp | Removes person from repository; publishes PersonDied event | `uuid` (int) | Person removed; event dispatched | None | Remove person and verify removal + event | Works |
| 10 | `agePerson(int uuid)` | PopulationManager.hpp | Ages a specific person by 1 year | `uuid` (int) | Person's age increments | None | Call agePerson and verify age incremented | Works |
| 11 | `find(predicate)` | PopulationManager.hpp | Find all persons matching predicate function | `predicate` (function<bool(const Person&)>) | Returns matching persons | None | Test with various predicates (gender, age, pregnant) | Works |
| 12 | `findEligiblePartners(std::shared_ptr<Person>)` | PopulationManager.hpp | Find potential marriage partners for a person | `person` (shared_ptr) | Returns vector of eligible partners (unmarried, alive, not relatives) | `min_marriage_age`, `max_person_age`, `min_marriage_age` from config | Test with different persons and verify eligibility | Works |
| 13 | `getLivingPopulation()` | PopulationManager.hpp | Get all living persons | None | Vector of living persons | None | Call and verify only alive persons returned | Works |
| 14 | `getByAgeRange(int minAge, int maxAge)` | PopulationManager.hpp | Get persons within age range | `minAge`, `maxAge` (int) | Vector of persons within range | None | Call with ranges and verify correct persons returned | Works |
| 15 | `getByGender(Gender)` | PopulationManager.hpp (via repo) | Get persons of specific gender | `Gender` (enum) | Vector of persons of that gender | None | Call and verify gender filtering | Works |
| 16 | `areRelatives(std::shared_ptr<Person>, std::shared_ptr<Person>)` | PopulationManager.cpp | Check if two persons are relatives (share mother or father) | `p1`, `p2` (shared_ptr) | `true` if share mother/father, `false` otherwise | None | Test with known relatives and non-relatives | Works |

## Family Management

| # | Feature | Old File | Current Behavior | Inputs | Result | Related Settings | Check Method | Status |
|---|---------|----------|------------------|--------|--------|-------------------|--------------|--------|
| 17 | `createFamily(std::shared_ptr<Mother>, std::shared_ptr<Father>` | FamilyManager.hpp | Create new family with mother and father | `mother`, `father` (shared_ptr) | New family object; events published; parents' family link updated | Gender check (F/M), alive check | Create family and verify mother/father links + events | Works |
| 18 | `dissolveFamily(int familyId)` | FamilyManager.hpp | dissolve/destroy a family | `familyId` (int) | Family removed; parents' family links cleared; events published | None | Dissolve family and verify removal + parent updates | Works |
| 19 | `addChildToFamily(int familyId, std::shared_ptr<Person>)` | FamilyManager.hpp | Add child to existing family | `familyId`, `child` (shared_ptr) | Child added to family; events published | None | Add child and verify in family + events | Works |
| 20 | `getAllFamilies()` | FamilyManager.hpp | Get all families | None | Vector of all families | None | Call and verify count | Works |
| 21 | `getFamilyMembers(int familyId)` | FamilyManager.hpp | Get all members of a family (parents + children) | `familyId` (int) | Vector of family members | None | Call and verify members include parents + children | Works |

## Person Entity

| # | Feature | Old File | Current Behavior | Inputs | Result | Related Settings | Check Method | Status |
|---|---------|----------|------------------|--------|--------|-------------------|--------------|--------|
| 22 | `Person(uuid, gender, age)` | Person.hpp | Constructor with uuid, gender, age | `uuid`, `gender`, `age` (int) | Person initialized with age, gender, alive=true, pregnant=false | None | Create person and verify getters | Works |
| 23 | `Person(uuid, gender, mother, father, age)` | Person.hpp | Constructor with parents | `uuid`, `gender`, `mother`, `father`, `age` | Person linked to parents via weak_ptr | None | Create with parents and verify getMother/getFather | Works |
| 24 | `incrementAge()` | Person.hpp | Increase age by 1 (if alive) | None | Age increments by 1; dead persons don't age | `alive` flag | Call incrementAge on alive/dead person; verify dead doesn't age | Works |
| 25 | `tryDie()` | Person.hpp | Attempt death based on mortality probability | None | Dies with probability `mortality_a * exp(mortality_b * age)`; if dies, `alive=false`, `pregnant=false` | `mortality_a`, `mortality_b` from config | Run many iterations on old person; verify death rate matches probability | Works |
| 26 | `tryConceive(int partnerAge)` | Person.hpp | Attempt conception with partner | `partnerAge` (int) | Conception succeeds with probability based on both partners' ages; sets `pregnant=true` | `base_conception_prob`, fertility function from config | Test conception with various age combinations; verify probability | Works |
| 27 | `setPregnant(bool)` | Person.hpp | Set pregnancy state | `isPregnant` (bool) | Sets pregnant flag; only for female gender | `gender` check | Set pregnant on male/female; verify only females can be pregnant | Works |
| 28 | `setAlive(bool)` | Person.hpp | Set alive/dead state | `isAlive` (bool) | Sets alive flag; resets pregnancy if becoming dead | None | SetAlive(false) then check pregnant reset; setAlive(true) | Works |
| 29 | `isMarried()` | Person.hpp | Check if person is married | None | Returns true if person is in a family as father or mother | None | Test married/unmarried persons | Works |
| 30 | `getMother()`, `getFather()`, `getFamily()` | Person.hpp | Get parent/family references (weak_ptr) | None | Weak pointers to mother, father, family | None | Get and verify weak_ptr behavior | Works |

## Services

| # | Feature | Old File | Current Behavior | Inputs | Result | Related Settings | Check Method | Status |
|---|---------|----------|------------------|--------|--------|-------------------|--------------|--------|
| 31 | `AgingService::processAging()` | AgingService.hpp | Age all living persons | None | All living persons' age increments by 1 | None | Call processAging(); verify ages increment | Works |
| 32 | `DeathService::processDeaths()` | DeathService.hpp | Process deaths for all living persons | None | Persons may die based on mortality probability; death events published | `mortality_a`, `mortality_b` from config | Call processDeaths(); verify deaths + events | Works |
| 33 | `MarriageService::processMarriages()` | MarriageService.hpp | Find eligible singles and attempt marriage | None | Marriages created between eligible persons; marriage events published | `age_diff_factor`, `min_marriage_age`, `max_person_age` from config | Call processMarriages(); verify marriages + events | Works |
| 34 | `PregnancyService::processPregnancies()` | PregnancyService.hpp | Process pregnancies for all families | None | Conception attempted; pregnancy events published | `base_conception_prob` from config | Call processPregnancies(); verify pregnancies + events | Works |
| 35 | `BirthService::processBirths()` | BirthService.hpp | Process births for all pregnant women | None | Children born (1/2/3 based on probabilities); birth/childbirth events published | `prob_triple`, `prob_twin` from config | Call processBirths(); verify children count + events | Works |
| 36 | `StatisticsService::printStatistics(int year)` | StatisticsService.hpp | Print population statistics | `year` (int) | Population count, family count, children/adults/elders breakdown | None | Call printStatistics(); verify output format | Works |

## Event System

| # | Feature | Old File | Current Behavior | Inputs | Result | Related Settings | Check Method | Status |
|---|---------|----------|------------------|--------|--------|-------------------|--------------|--------|
| 37 | Event types: PersonBorn, PersonDied, PersonMoved, Marriage, Pregnancy, Childbirth, FamilyCreated, FamilyDissolved | EventDispatcher.hpp | Enum of all event types | None | 8 event types available for subscription | None | Subscribe to each type and verify events can be published | Works |
| 38 | Event data storage (key-value string map) | EventDispatcher.hpp | Events can carry additional data | `key`, `value` (string) | Data stored and retrievable via getData/getDataOrDefault | None | Publish event with data; retrieve and verify | Works |
| 39 | EventDispatcher::subscribe(EventType, handler) | EventDispatcher.hpp | Subscribe to event type with handler function | `type`, `handler` (EventHandler) | Returns handlerId for unsubscription | None | Subscribe to type; publish event; verify handler called | Works |
| 40 | EventDispatcher::publish(Event) | EventDispatcher.hpp | Publish event to all subscribers | `event` (Event) | All subscribed handlers called | None | Publish event; verify handlers execute | Works |
| 41 | EventDispatcher::unsubscribe(EventType, handlerId) | EventDispatcher.hpp | Remove a previously subscribed handler | `type`, `handlerId` (int) | Handler removed; future publishes won't call it | None | Unsubscribe; publish; verify handler not called | Works |

## Configuration

| # | Feature | Old File | Current Behavior | Inputs | Result | Related Settings | Check Method | Status |
|---|---------|----------|------------------|--------|--------|-------------------|--------------|--------|
| 42 | `Config::getInstance()` | Config.hpp | Get singleton instance | None | Single Config instance | None | Call multiple times; verify same reference | Works |
| 43 | `Config::loadFromFile(filename)` | Config.hpp | Load configuration from file | `filename` (string) | Parses key=value lines; loads names/surnames from files | `simulation.cfg` format | Load from simulation.cfg; verify settings applied | Works |
| 44 | `Config::getDouble(key, default)` | Config.hpp | Get double value | `key`, `default` (double) | Returns parsed double or default | All double config keys | Get various keys; verify values match | Works |
| 45 | `Config::getInt(key, default)` | Config.hpp | Get int value | `key`, `default` (int) | Returns parsed int or default | All int config keys | Get various keys; verify values match | Works |
| 46 | `Config::getString(key, default)` | Config.hpp | Get string value | `key`, `default` (string) | Returns string or default | All string config keys | Get various keys; verify values match | Works |
| 47 | `Config::set(key, value)` | Config.hpp | Programmatically set config value | `key`, `value` (string) | Stores value in internal map | None | Set key; getDouble/getInt and verify | Works |

## Visualization

| # | Feature | Old File | Current Behavior | Inputs | Result | Related Settings | Check Method | Status |
|---|---------|----------|------------------|--------|--------|-------------------|--------------|--------|
| 48 | Scene::calculateTreeLayout() | Scene.hpp | Layout person positions in tree structure | None | Calculates x,y positions for all persons based on family hierarchy | `verticalSpacing_`, `horizontalSpacing_`, `nodeRadius_` | Call update(); verify positions assigned to persons | Works |
| 49 | Scene::draw(sf::RenderWindow&, sf::View&) | Scene.hpp | Draw scene including persons and family connections | `window`, `view` | Renders family tree with persons as circles, connections as lines | Colors (male/female/deceased) | Render frame; verify persons drawn with correct colors | Works |
| 50 | Scene::drawPerson(...) | Scene.hpp | Draw individual person | `person`, `position`, `window` | Circle with gender color; age name displayed if font loaded | `maleColor_`, `femaleColor_`, `deceasedColor_` | Draw person; verify color and text | Works |
| 51 | Scene::getPersonAtPosition(const sf::Vector2f&, float) | Scene.hpp | Find person at screen position | `worldPos`, `zoomLevel` | Returns person UUID or -1 if none | `nodeRadius_` | Click on person; verify correct person selected | Works |
| 52 | Scene::getPersonPosition(int personId) | Scene.hpp | Get position of person | `personId` (int) | Optional<sf::Vector2f> position or null | None | Get position; verify matches layout | Works |
| 53 | Scene::loadFont(const std::string&) | Scene.hpp | Load font for text rendering | `fontPath` (string) | Sets fontLoaded_ flag; enables text drawing | font.otf asset | Load font; verify text appears on screen | Works |
| 54 | Camera::handleMouseEvents(const sf::Event&) | Camera.hpp | Handle mouse clicks, drags, wheel | `event` (sf::Event) | Drag to pan, wheel to zoom | `zoomLevel_` clamps [0.1, 5.0] | Mouse interactions; verify pan/zoom | Works |
| 55 | Camera::handleKeyboardEvents(const sf::Event&) | Camera.hpp | Handle keyboard input | `event` (sf::Event) | Currently N and M cases (no-op in old) | None | Press N/M; verify behavior | Works |
| 56 | Camera::update() | Camera.hpp | Update view with current offset and zoom | None | Applies offset and zoomLevel to SFML view | `offset_`, `zoomLevel_` | Camera update; verify view center/zoom | Works |
| 57 | Camera::zoom(float delta) | Camera.hpp | Zoom in/out | `delta` (int sign) | Multiply/divide zoomLevel by 1.1; clamp [0.1, 5.0] | None | Zoom with +/++; verify clamp | Works |
| 58 | Camera::reset() | Camera.hpp | Reset camera state | None | offset = (0,0), zoomLevel = 1.0 | None | Reset; verify view center and zoom | Works |
| 59 | UI::drawInfoPanel(...) | UI.hpp | Draw side panel with statistics | `populationManager`, `familyManager`, `selectedPersonId`, `personRepository` | Population and family counts displayed; person info if selected | None | Render UI; verify stats displayed | Works |
| 60 | UI::drawPersonInfo(...) | UI.hpp | Draw selected person's info panel | `selectedPersonId`, `personRepository` | Name, age, gender, status, family ID displayed | None | Select person; verify info panel updates | Works |
| 61 | UI::drawControlsHint() | UI.hpp | Draw controls hint at bottom | None | Lists mouse/keyboard controls | None | Render UI; verify controls listed | Works |
| 62 | UI::drawSimulationStatus(const SimulationState&) | UI.hpp | Draw year/pause/auto-advance status | `state` (SimulationState) | Year, [PAUSED]/[RUNNING], [AUTO] displayed | `currentYear`, `isPaused`, `autoAdvance` | Render UI; verify status text | Works |
| 63 | UI::handleResize(const sf::Event&) | UI.hpp | Handle window resize | `event` (sf::Event) | Update uiView_ size and center | None | Resize window; verify UI resizes | Works |
| 64 | Visualizer::handleEvents() | Visualizer.hpp | Main event loop - camera, keyboard, window events | `sf::Event` loop | Processes all window events; keyboard controls; mouse handling | All above | Run visualizer; verify all keys work | Works |
| 65 | Visualizer::handleKeyboardEvents(const sf::Event&) | Visualizer.hpp | Handle key presses | `event` (sf::Event) | Escape: close; R: reset camera; C: toggle controls; H: select hovered; Comma/Period: zoom | All above | Press each key; verify action | Works |
| 66 | Visualizer::setSimulationState(const SimulationState&) | Visualizer.hpp | Set simulation state for UI display | `state` (SimulationState) | Updates display with year/pause/auto status | `currentYear`, `isPaused`, `autoAdvance` | Set state; verify UI updates | Works |
| 67 | Visualizer::setCurrentYear(int year) | Visualizer.hpp | Set current year display | `year` (int) | Updates simulationState_.currentYear | None | Set year; verify UI shows correct year | Works |
| 68 | Visualizer::isOpen() | Visualizer.hpp | Check if window is open | None | Returns window_.isOpen() | None | Run visualizer; verify window stays open | Works |

## Input Controls

| # | Feature | Old File | Current Behavior | Inputs | Result | Related Settings | Check Method | Status |
|---|---------|----------|------------------|--------|--------|-------------------|--------------|--------|
| 69 | Space - Pause/Resume | UI.hpp / Visualizer.hpp | Toggle simulation pause | Key press | Pauses/resumes year advancement | `simulationPaused_` in Simulation | Press Space; verify simulation pauses/resumes | Works |
| 70 | Right Arrow - Step forward | UI.hpp / Visualizer.hpp | Advance one year | Key press | Advances one year simulation step | `stepRequested_` in Simulation | Press Right Arrow; verify one year advances | Works |
| 71 | A - Toggle auto-advance | UI.hpp / Visualizer.hpp | Toggle auto-advance mode | Key press | Toggles autoAdvance_ flag | `autoAdvance_` in Simulation | Press A; verify auto-advance on/off | Works |
| 72 | Up/Down - Speed control | UI.hpp | Adjust simulation speed | Key press | Changes visualization speed | None (UI only) | Press Up/Down; verify speed change | Works |
| 73 | Mouse Drag - Pan | UI.hpp / Camera.hpp | Drag mouse to pan view | Mouse drag | Offset changes; view pans | `offset_` in Camera | Drag mouse; verify view pans | Works |
| 74 | Mouse Wheel - Zoom | UI.hpp / Camera.hpp | Scroll wheel to zoom | Mouse wheel | zoomLevel changes; clamp [0.1, 5.0] | `zoomLevel_` in Camera | Scroll wheel; verify zoom in/out | Works |
| 75 | R - Reset view | UI.hpp / Visualizer.hpp | Reset camera to default | Key press | Camera resets to center, zoom 1.0 | None | Press R; verify camera resets | Works |
| 76 | C - Toggle controls hint | UI.hpp / Visualizer.hpp | Show/hide controls list | Key press | showControls_ flag toggles | `showControls_` in Visualizer | Press C; verify hint appears/disappears | Works |
| 77 | H - Select hovered person | UI.hpp / Visualizer.hpp | Select person under mouse | Key press | Selected person = currently hovered person | `hoveredPersonId_` in Scene | Hover over person; Press H; verify selection | Works |
| 78 | <>/Comma - Zoom in | Visualizer.hpp | Zoom in | Key press | zoomLevel_ *= 1.1 | None | Press < or +; verify zoom in | Works |
| 79 | ./Period - Zoom out | Visualizer.hpp | Zoom out | Key press | zoomLevel_ /= 1.1 | None | Press . or -; verify zoom out | Works |

## Assets and Resources

| # | Feature | Old File | Current Behavior | Inputs | Result | Related Settings | Check Method | Status |
|---|---------|----------|------------------|--------|--------|-------------------|--------------|--------|
| 80 | male_names.txt loading | NameManager.cpp | Load male names from file | File path | Vector of male names loaded | `male_names_file` in config | Load from config path; verify names available | Works |
| 81 | female_names.txt loading | NameManager.cpp | Load female names from file | File path | Vector of female names loaded | `female_names_file` in config | Load from config path; verify names available | Works |
| 82 | surnames.txt loading | NameManager.cpp | Load surnames from file | File path | Vector of surnames loaded | `surnames_file` in config | Load from config path; verify surnames available | Works |
| 83 | font.otf for text rendering | Visualizer.cpp / Scene.hpp | Load OTF font for displaying text | `../assets/font.otf` | Font loaded; enables text in UI and scene | None | Load font; verify text rendered | Works |
