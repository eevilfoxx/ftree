#pragma once

#include "application/ports/ISimulationRepository.hpp"

#include <memory>
#include <string>

struct pg_conn;
typedef struct pg_conn PGconn;

namespace ftree::infrastructure::postgres {

class PostgresSimulationRepository final
    : public application::ports::ISimulationRepository {
public:
    explicit PostgresSimulationRepository(const std::string& connectionString);
    ~PostgresSimulationRepository() override;

    PostgresSimulationRepository(const PostgresSimulationRepository&) = delete;
    PostgresSimulationRepository& operator=(const PostgresSimulationRepository&) = delete;

    static std::string connectionStringFromEnvironment();

    application::ports::SimulationId createSimulation(
        const application::ports::SimulationSnapshot& snapshot) override;
    application::ports::SimulationSnapshot loadSimulation(
        application::ports::SimulationId id) override;
    void saveSimulation(
        const application::ports::SimulationSnapshot& snapshot) override;
    std::vector<application::ports::SimulationSummary> listSimulations() override;
    void deleteSimulation(application::ports::SimulationId id) override;

private:
    PGconn* connection_ = nullptr;
    void applyMigrations();
};

} // namespace ftree::infrastructure::postgres
