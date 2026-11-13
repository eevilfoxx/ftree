#include <gtest/gtest.h>
#include <services/MarriageService.hpp>
#include <managers/PopulationManager.hpp>
#include <managers/FamilyManager.hpp>
#include <entities/repos/IFamilyRepository.hpp>
#include <entities/repos/IPersonRepository.hpp>
#include <entities/repos/MemoryPersonRepository.hpp>
#include <entities/repos/MemoryFamilyRepository.hpp>
#include <core/EventDispatcher.hpp>
#include <core/Config.hpp>
#include <entities/Person.hpp>
#include <entities/Gender.hpp>

class MarriageServiceTest : public ::testing::Test {
protected:
    std::shared_ptr<IFamilyRepository> familyRepo;
    std::shared_ptr<IPersonRepository> personRepo;
    std::shared_ptr<EventDispatcher> eventDispatcher;
    std::shared_ptr<PopulationManager> popManager;
    std::shared_ptr<FamilyManager> famManager;
    std::shared_ptr<MarriageService> marriageService;

    void SetUp() override {
        familyRepo = std::make_shared<MemoryFamilyRepository>();
        personRepo = std::make_shared<MemoryPersonRepository>();
        eventDispatcher = std::make_shared<EventDispatcher>();
        popManager = std::make_shared<PopulationManager>(personRepo, eventDispatcher);
        famManager = std::make_shared<FamilyManager>(familyRepo, personRepo, eventDispatcher);
        marriageService = std::make_shared<MarriageService>(famManager, popManager, personRepo, eventDispatcher);

        auto& config = Config::getInstance();
        config.set("age_diff_factor", "0.0");  // Probability = exp(0) = 1.0 for all
        config.set("min_marriage_age", "18");
        config.set("max_person_age", "100");
    }
};

TEST_F(MarriageServiceTest, NoSinglePeopleNoMarriages) {
    // Should not crash with no people
    marriageService->processMarriages();
}

TEST_F(MarriageServiceTest, OnlyOnePersonNoMarriage) {
    auto person = std::make_shared<Person>(1, Gender::Male, 25);
    popManager->addPerson(person);

    marriageService->processMarriages();

    // Single person cannot marry themselves
    EXPECT_FALSE(person->isMarried());
}

TEST_F(MarriageServiceTest, UnderageNoMarriage) {
    auto male = std::make_shared<Person>(1, Gender::Male, 25);
    auto minor = std::make_shared<Person>(2, Gender::Female, 16);
    popManager->addPerson(male);
    popManager->addPerson(minor);

    marriageService->processMarriages();

    EXPECT_FALSE(male->isMarried());
    EXPECT_FALSE(minor->isMarried());
}