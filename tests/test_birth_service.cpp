#include <gtest/gtest.h>
#include <services/BirthService.hpp>
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
#include <managers/NameManager.hpp>

class BirthServiceTest : public ::testing::Test {
protected:
    std::shared_ptr<IPersonRepository> personRepo;
    std::shared_ptr<IFamilyRepository> familyRepo;
    std::shared_ptr<EventDispatcher> eventDispatcher;
    std::shared_ptr<PopulationManager> popManager;
    std::shared_ptr<FamilyManager> famManager;
    std::shared_ptr<BirthService> birthService;

    void SetUp() override {
        personRepo = std::make_shared<MemoryPersonRepository>();
        familyRepo = std::make_shared<MemoryFamilyRepository>();
        eventDispatcher = std::make_shared<EventDispatcher>();
        popManager = std::make_shared<PopulationManager>(personRepo, eventDispatcher);
        famManager = std::make_shared<FamilyManager>(familyRepo, personRepo, eventDispatcher);
        birthService = std::make_shared<BirthService>(popManager, famManager, personRepo, eventDispatcher);

        auto& config = Config::getInstance();
        config.set("prob_triple", "0.0");
        config.set("prob_twin", "0.0");

        NameManager::getInstance().loadNames(
            "/home/evilfox/ftree/config/male_names.txt",
            "/home/evilfox/ftree/config/female_names.txt"
        );
        NameManager::getInstance().loadSurnames(
            "/home/evilfox/ftree/config/surnames.txt"
        );
    }
};

TEST_F(BirthServiceTest, NoPregnantWomenNoCrash) {
    // Should not crash with no pregnant women
    birthService->processBirths();
}

TEST_F(BirthServiceTest, PregnantWomanWithoutFamily) {
    auto woman = std::make_shared<Person>(1, Gender::Female, 25);
    woman->setPregnant(true);
    personRepo->add(woman);

    // Should not crash even though woman has no family
    birthService->processBirths();
}