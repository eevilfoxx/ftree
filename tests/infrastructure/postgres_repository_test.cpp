#include <gtest/gtest.h>

#include "core/Random.hpp"
#include "domain/World.hpp"
#include "infrastructure/postgres/PostgresSimulationRepository.hpp"

#include <cstdlib>
#include <algorithm>
#include <vector>
#include <string>

using namespace ftree;
using namespace ftree::application::ports;

TEST(PostgresSimulationRepositoryTest, CreateLoadUpdateListDeleteRoundTrip) {
    const char* connection = std::getenv("FTREE_TEST_DATABASE_URL");
    if (!connection || !*connection) {
        GTEST_SKIP() << "Set FTREE_TEST_DATABASE_URL to run PostgreSQL integration tests";
    }

    infrastructure::postgres::PostgresSimulationRepository repository(connection);
    domain::World world;
    const auto mother = world.createPerson("Test mother", domain::Gender::Female, 30);
    const auto father = world.createPerson("Test father", domain::Gender::Male, 32);
    const auto family = world.createFamily(mother, father, "Test");
    const auto child = world.createPerson("Test child", domain::Gender::Female, 2);
    world.getPersonRef(child).motherId = mother;
    world.getPersonRef(child).fatherId = father;
    world.getFamilyRef(family).childrenIds.push_back(child);

    core::Random random(12345);
    (void)random.randomInt(0, 100);

    SimulationSnapshot snapshot;
    snapshot.name = "integration-test-" + std::to_string(std::rand());
    snapshot.currentYear = 7;
    snapshot.world = world.createSnapshot();
    snapshot.config = core::SimulationConfig{};
    snapshot.randomSeed = random.seed();
    snapshot.randomEngineState = random.serializeState();
    snapshot.nameDatabase.maleNames = {"A"};
    snapshot.nameDatabase.femaleNames = {"B"};
    snapshot.nameDatabase.surnames = {"C"};

    const auto id = repository.createSimulation(snapshot);
    ASSERT_GT(id, 0);

    auto loaded = repository.loadSimulation(id);
    EXPECT_EQ(loaded.currentYear, 7);
    EXPECT_EQ(loaded.world.people.size(), 3u);
    EXPECT_EQ(loaded.world.families.size(), 1u);
    EXPECT_EQ(loaded.world.families.front().childrenIds,
              (std::vector<domain::PersonId>{child}));
    EXPECT_EQ(loaded.nameDatabase.femaleNames, (std::vector<std::string>{"B"}));

    loaded.currentYear = 8;
    loaded.world.people.front().age += 1;
    repository.saveSimulation(loaded);
    EXPECT_EQ(repository.loadSimulation(id).currentYear, 8);

    const auto saves = repository.listSimulations();
    EXPECT_NE(std::find_if(saves.begin(), saves.end(), [id](const SimulationSummary& item) {
        return item.id == id;
    }), saves.end());

    repository.deleteSimulation(id);
    EXPECT_THROW(repository.loadSimulation(id), std::runtime_error);
}
