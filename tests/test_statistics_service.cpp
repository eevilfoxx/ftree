#include <gtest/gtest.h>
#include <utils/StatisticsService.hpp>
#include <managers/PopulationManager.hpp>
#include <managers/FamilyManager.hpp>
#include <entities/repos/MemoryPersonRepository.hpp>
#include <entities/repos/MemoryFamilyRepository.hpp>
#include <core/EventDispatcher.hpp>
#include <entities/Person.hpp>
#include <entities/Gender.hpp>

class StatisticsServiceTest : public ::testing::Test {
protected:
    std::shared_ptr<IPersonRepository> personRepo;
    std::shared_ptr<IFamilyRepository> familyRepo;
    std::shared_ptr<EventDispatcher> eventDispatcher;
    std::shared_ptr<PopulationManager> popManager;
    std::shared_ptr<FamilyManager> famManager;
    std::shared_ptr<StatisticsService> statsService;

    void SetUp() override {
        personRepo = std::make_shared<MemoryPersonRepository>();
        familyRepo = std::make_shared<MemoryFamilyRepository>();
        eventDispatcher = std::make_shared<EventDispatcher>();
        popManager = std::make_shared<PopulationManager>(personRepo, eventDispatcher);
        famManager = std::make_shared<FamilyManager>(familyRepo, personRepo, eventDispatcher);
        statsService = std::make_shared<StatisticsService>(popManager, famManager);
    }
};

TEST_F(StatisticsServiceTest, PrintStatisticsEmpty) {
    // Should not crash with empty population
    EXPECT_NO_FATAL_FAILURE(statsService->printStatistics(0));
}

TEST_F(StatisticsServiceTest, PrintStatisticsWithPopulation) {
    popManager->addPerson(std::make_shared<Person>(1, Gender::Male, 10));
    popManager->addPerson(std::make_shared<Person>(2, Gender::Female, 25));
    popManager->addPerson(std::make_shared<Person>(3, Gender::Male, 65));

    // Should not crash
    EXPECT_NO_FATAL_FAILURE(statsService->printStatistics(1));
}

TEST_F(StatisticsServiceTest, PrintStatisticsYearZero) {
    popManager->addPerson(std::make_shared<Person>(1, Gender::Male, 25));

    EXPECT_NO_FATAL_FAILURE(statsService->printStatistics(0));
}