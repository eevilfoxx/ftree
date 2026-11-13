#include <gtest/gtest.h>
#include <services/DeathService.hpp>
#include <managers/PopulationManager.hpp>
#include <entities/repos/MemoryPersonRepository.hpp>
#include <core/EventDispatcher.hpp>
#include <core/Config.hpp>
#include <entities/Person.hpp>
#include <entities/Gender.hpp>

class DeathServiceTest : public ::testing::Test {
protected:
    std::shared_ptr<IPersonRepository> personRepo;
    std::shared_ptr<EventDispatcher> eventDispatcher;
    std::shared_ptr<PopulationManager> popManager;
    std::shared_ptr<DeathService> deathService;

    void SetUp() override {
        personRepo = std::make_shared<MemoryPersonRepository>();
        eventDispatcher = std::make_shared<EventDispatcher>();
        popManager = std::make_shared<PopulationManager>(personRepo, eventDispatcher);
        deathService = std::make_shared<DeathService>(popManager, eventDispatcher);

        auto& config = Config::getInstance();
        config.set("mortality_a", "0.0");
        config.set("mortality_b", "0.0693");
    }
};

TEST_F(DeathServiceTest, ZeroMortalityNoDeaths) {
    popManager->addPerson(std::make_shared<Person>(1, Gender::Male, 25));
    popManager->addPerson(std::make_shared<Person>(2, Gender::Female, 30));
    popManager->addPerson(std::make_shared<Person>(3, Gender::Male, 100));

    deathService->processDeaths();

    auto living = popManager->getLivingPopulation();
    EXPECT_EQ(living.size(), 3);
    for (const auto& p : living) {
        EXPECT_TRUE(p->isAlive());
    }
}

TEST_F(DeathServiceTest, EmptyPopulation) {
    // Should not crash with no people
    deathService->processDeaths();
}