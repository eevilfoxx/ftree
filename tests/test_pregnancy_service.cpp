#include <gtest/gtest.h>
#include <services/PregnancyService.hpp>
#include <managers/FamilyManager.hpp>
#include <managers/PopulationManager.hpp>
#include <entities/repos/IFamilyRepository.hpp>
#include <entities/repos/IPersonRepository.hpp>
#include <entities/repos/MemoryPersonRepository.hpp>
#include <entities/repos/MemoryFamilyRepository.hpp>
#include <core/EventDispatcher.hpp>
#include <core/Config.hpp>
#include <entities/Person.hpp>
#include <entities/Gender.hpp>
#include <managers/NameManager.hpp>

class PregnancyServiceTest : public ::testing::Test {
protected:
    std::shared_ptr<IFamilyRepository> familyRepo;
    std::shared_ptr<IPersonRepository> personRepo;
    std::shared_ptr<EventDispatcher> eventDispatcher;
    std::shared_ptr<FamilyManager> famManager;
    std::shared_ptr<PregnancyService> pregnancyService;

    void SetUp() override {
        familyRepo = std::make_shared<MemoryFamilyRepository>();
        personRepo = std::make_shared<MemoryPersonRepository>();
        eventDispatcher = std::make_shared<EventDispatcher>();
        famManager = std::make_shared<FamilyManager>(familyRepo, personRepo, eventDispatcher);
        pregnancyService = std::make_shared<PregnancyService>(famManager, personRepo, eventDispatcher);

        auto& config = Config::getInstance();
        config.set("base_conception_prob", "0.0");

        NameManager::getInstance().loadNames(
            "/home/evilfox/ftree/config/male_names.txt",
            "/home/evilfox/ftree/config/female_names.txt"
        );
        NameManager::getInstance().loadSurnames(
            "/home/evilfox/ftree/config/surnames.txt"
        );
    }
};

TEST_F(PregnancyServiceTest, EmptyFamiliesNoCrash) {
    // Should not crash with no families
    pregnancyService->processPregnancies();
}

TEST_F(PregnancyServiceTest, ZeroConceptionProbNoPregnancy) {
    auto mother = std::make_shared<Person>(1, Gender::Female, 25);
    auto father = std::make_shared<Person>(2, Gender::Male, 30);
    personRepo->add(mother);
    personRepo->add(father);

    famManager->createFamily(mother, father);

    pregnancyService->processPregnancies();

    auto retrievedMother = personRepo->getById(1);
    ASSERT_NE(retrievedMother, nullptr);
    EXPECT_FALSE(retrievedMother->isPregnant());
}

TEST_F(PregnancyServiceTest, DeadMotherNoPregnancy) {
    auto mother = std::make_shared<Person>(1, Gender::Female, 25);
    auto father = std::make_shared<Person>(2, Gender::Male, 30);
    mother->setAlive(false);
    personRepo->add(mother);
    personRepo->add(father);

    famManager->createFamily(mother, father);

    pregnancyService->processPregnancies();

    auto retrievedMother = personRepo->getById(1);
    ASSERT_NE(retrievedMother, nullptr);
    EXPECT_FALSE(retrievedMother->isPregnant());
}