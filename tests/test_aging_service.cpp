#include <gtest/gtest.h>
#include <services/AgingService.hpp>
#include <managers/PopulationManager.hpp>
#include <entities/repos/MemoryPersonRepository.hpp>
#include <core/EventDispatcher.hpp>
#include <entities/Person.hpp>
#include <entities/Gender.hpp>

class AgingServiceTest : public ::testing::Test {
protected:
    std::shared_ptr<IPersonRepository> personRepo;
    std::shared_ptr<EventDispatcher> eventDispatcher;
    std::shared_ptr<PopulationManager> popManager;
    std::shared_ptr<AgingService> agingService;

    void SetUp() override {
        personRepo = std::make_shared<MemoryPersonRepository>();
        eventDispatcher = std::make_shared<EventDispatcher>();
        popManager = std::make_shared<PopulationManager>(personRepo, eventDispatcher);
        agingService = std::make_shared<AgingService>(popManager, personRepo);
    }
};

TEST_F(AgingServiceTest, ProcessAgingIncreasesAge) {
    popManager->addPerson(std::make_shared<Person>(1, Gender::Male, 25));
    popManager->addPerson(std::make_shared<Person>(2, Gender::Female, 30));

    agingService->processAging();

    auto p1 = personRepo->getById(1);
    auto p2 = personRepo->getById(2);
    ASSERT_NE(p1, nullptr);
    ASSERT_NE(p2, nullptr);
    EXPECT_EQ(p1->getAge(), 26);
    EXPECT_EQ(p2->getAge(), 31);
}

TEST_F(AgingServiceTest, ProcessAgingEmptyPopulation) {
    // Should not crash with no people
    agingService->processAging();
}

TEST_F(AgingServiceTest, ProcessAgingDeadPersonNotAged) {
    auto person = std::make_shared<Person>(1, Gender::Male, 25);
    person->setAlive(false);
    popManager->addPerson(person);

    agingService->processAging();

    auto retrieved = personRepo->getById(1);
    ASSERT_NE(retrieved, nullptr);
    EXPECT_EQ(retrieved->getAge(), 25);  // Dead person should not age
}