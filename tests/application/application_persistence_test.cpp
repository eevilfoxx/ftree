#include <gtest/gtest.h>

#include "application/Application.hpp"

#include <map>
#include <memory>
#include <stdexcept>

namespace {
using namespace ftree::application::ports;

struct SharedStore {
    SimulationId nextId = 1;
    std::map<SimulationId, SimulationSnapshot> snapshots;
    int saveCalls = 0;
};

class MemorySimulationRepository final : public ISimulationRepository {
public:
    explicit MemorySimulationRepository(std::shared_ptr<SharedStore> store)
        : store_(std::move(store)) {}

    SimulationId createSimulation(const SimulationSnapshot& snapshot) override {
        auto copy = snapshot;
        copy.id = store_->nextId++;
        store_->snapshots[copy.id] = copy;
        return copy.id;
    }
    SimulationSnapshot loadSimulation(SimulationId id) override {
        return store_->snapshots.at(id);
    }
    void saveSimulation(const SimulationSnapshot& snapshot) override {
        if (store_->snapshots.find(snapshot.id) == store_->snapshots.end()) {
            throw std::runtime_error("missing simulation");
        }
        store_->snapshots[snapshot.id] = snapshot;
        ++store_->saveCalls;
    }
    std::vector<SimulationSummary> listSimulations() override { return {}; }
    void deleteSimulation(SimulationId id) override { store_->snapshots.erase(id); }
private:
    std::shared_ptr<SharedStore> store_;
};
}

TEST(ApplicationPersistenceTest, SavesAfterEveryYearAndCanResume) {
    auto store = std::make_shared<SharedStore>();
    ftree::application::StartupSelection create;
    create.mode = ftree::application::StartupMode::CreateNew;
    create.newSimulationName = "Persistence test";

    {
        ftree::application::Application app(
            nullptr, std::make_unique<MemorySimulationRepository>(store));
        ASSERT_TRUE(app.initialize(create, std::string(FTREE_TEST_SOURCE_DIR) + "/config/simulation.cfg"));
        EXPECT_EQ(app.getCurrentYear(), 0);
        ASSERT_EQ(app.runHeadless(3), 0);
        EXPECT_EQ(app.getCurrentYear(), 3);
    }

    ASSERT_EQ(store->snapshots.size(), 1u);
    EXPECT_EQ(store->saveCalls, 3);
    const auto id = store->snapshots.begin()->first;

    ftree::application::StartupSelection load;
    load.mode = ftree::application::StartupMode::LoadExisting;
    load.simulationId = id;
    {
        ftree::application::Application app(
            nullptr, std::make_unique<MemorySimulationRepository>(store));
        ASSERT_TRUE(app.initialize(load));
        EXPECT_EQ(app.getCurrentYear(), 3);
        ASSERT_EQ(app.runHeadless(2), 0);
        EXPECT_EQ(app.getCurrentYear(), 5);
    }
    EXPECT_EQ(store->snapshots.at(id).currentYear, 5);
    EXPECT_EQ(store->saveCalls, 5);
}
