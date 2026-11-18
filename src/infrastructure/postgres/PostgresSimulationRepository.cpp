#include "infrastructure/postgres/PostgresSimulationRepository.hpp"
#include "domain/World.hpp"
#include "core/Random.hpp"

#include <libpq-fe.h>

#include <cstdlib>
#include <iomanip>
#include <fstream>
#include <algorithm>
#include <type_traits>
#include <limits>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>

namespace ftree::infrastructure::postgres {
namespace {
using Snapshot = application::ports::SimulationSnapshot;
using Summary = application::ports::SimulationSummary;
using Id = application::ports::SimulationId;

struct ResultDeleter { void operator()(PGresult* value) const noexcept { if (value) PQclear(value); } };
using Result = std::unique_ptr<PGresult, ResultDeleter>;

[[noreturn]] void fail(PGconn* connection, const std::string& operation) {
    throw std::runtime_error(operation + ": " +
        (connection ? PQerrorMessage(connection) : "PostgreSQL connection is null"));
}

Result execute(PGconn* connection, const std::string& sql) {
    Result result(PQexec(connection, sql.c_str()));
    if (!result || (PQresultStatus(result.get()) != PGRES_COMMAND_OK
        && PQresultStatus(result.get()) != PGRES_TUPLES_OK)) {
        fail(connection, "PostgreSQL query failed");
    }
    return result;
}

Result executeParams(PGconn* connection, const std::string& sql,
                     const std::vector<std::optional<std::string>>& parameters) {
    std::vector<const char*> values;
    values.reserve(parameters.size());
    for (const auto& parameter : parameters) {
        values.push_back(parameter ? parameter->c_str() : nullptr);
    }
    Result result(PQexecParams(connection, sql.c_str(),
        static_cast<int>(values.size()), nullptr, values.data(), nullptr, nullptr, 0));
    if (!result || (PQresultStatus(result.get()) != PGRES_COMMAND_OK
        && PQresultStatus(result.get()) != PGRES_TUPLES_OK)) {
        fail(connection, "PostgreSQL parameterized query failed");
    }
    return result;
}

template <typename T, typename = std::enable_if_t<std::is_integral<T>::value>>
std::string number(T value) { return std::to_string(value); }
std::string serializeConfig(const core::SimulationConfig& c) {
    std::ostringstream out;
    out << c.min_founder_age << ' ' << c.max_founder_age << ' '
        << std::setprecision(17) << c.mortality_a << ' ' << c.mortality_b << ' '
        << c.base_conception_prob << ' ' << c.min_marriage_age << ' '
        << c.age_diff_factor << ' ' << c.prob_triple << ' ' << c.prob_twin << ' '
        << c.max_person_age << ' ' << std::quoted(c.male_names_file) << ' '
        << std::quoted(c.female_names_file) << ' ' << std::quoted(c.surnames_file);
    return out.str();
}

core::SimulationConfig deserializeConfig(const std::string& data) {
    core::SimulationConfig c;
    std::istringstream in(data);
    if (!(in >> c.min_founder_age >> c.max_founder_age >> c.mortality_a >> c.mortality_b
        >> c.base_conception_prob >> c.min_marriage_age >> c.age_diff_factor
        >> c.prob_triple >> c.prob_twin >> c.max_person_age
        >> std::quoted(c.male_names_file) >> std::quoted(c.female_names_file)
        >> std::quoted(c.surnames_file))) {
        throw std::runtime_error("Stored simulation configuration is malformed");
    }
    c.validate();
    return c;
}

std::string serializeNames(const core::NameDatabaseSnapshot& names) {
    std::ostringstream out;
    const auto write = [&out](const std::vector<std::string>& values) {
        out << values.size() << '\n';
        for (const auto& value : values) out << std::quoted(value) << '\n';
    };
    write(names.maleNames); write(names.femaleNames); write(names.surnames);
    return out.str();
}

core::NameDatabaseSnapshot deserializeNames(const std::string& data) {
    std::istringstream in(data);
    core::NameDatabaseSnapshot result;
    const auto read = [&in](std::vector<std::string>& values) {
        std::size_t count = 0;
        if (!(in >> count) || count > 1000000) throw std::runtime_error("Stored name database is malformed");
        values.reserve(count);
        for (std::size_t i = 0; i < count; ++i) {
            std::string value;
            if (!(in >> std::quoted(value))) throw std::runtime_error("Stored name database is malformed");
            values.push_back(std::move(value));
        }
    };
    read(result.maleNames); read(result.femaleNames); read(result.surnames);
    return result;
}

std::optional<std::string> nullableId(std::uint32_t id) {
    if (id == 0) return std::nullopt;
    return std::to_string(id);
}

std::uint32_t readId(PGresult* result, int row, int column) {
    if (PQgetisnull(result, row, column)) return 0;
    const auto value = std::stoull(PQgetvalue(result, row, column));
    if (value > std::numeric_limits<std::uint32_t>::max()) throw std::runtime_error("Stored ID exceeds uint32 range");
    return static_cast<std::uint32_t>(value);
}

void validateSnapshot(const Snapshot& snapshot) {
    if (snapshot.name.empty()) throw std::invalid_argument("Simulation name cannot be empty");
    if (snapshot.currentYear < 0) throw std::invalid_argument("Simulation year cannot be negative");
    snapshot.config.validate();
    if (snapshot.randomEngineState.empty()) throw std::invalid_argument("Random engine state is empty");
    core::Random random(snapshot.randomSeed);
    random.restoreState(snapshot.randomEngineState);
    (void)domain::World::fromSnapshot(snapshot.world);
}

void saveRows(PGconn* connection, const Snapshot& snapshot) {
    validateSnapshot(snapshot);
    auto updated = executeParams(connection,
        "UPDATE simulations SET name=$2, current_year=$3, config_data=$4, rng_seed=$5, "
        "rng_state=$6, names_data=$7, next_person_id=$8, next_family_id=$9, updated_at=NOW() "
        "WHERE id=$1::bigint",
        {number(snapshot.id), snapshot.name, number(snapshot.currentYear),
         serializeConfig(snapshot.config), number(snapshot.randomSeed),
         snapshot.randomEngineState, serializeNames(snapshot.nameDatabase),
         number(snapshot.world.nextPersonId), number(snapshot.world.nextFamilyId)});
    if (std::string(PQcmdTuples(updated.get())) != "1") {
        throw std::runtime_error("Cannot save: simulation ID does not exist");
    }

    executeParams(connection, "DELETE FROM family_children WHERE simulation_id=$1::bigint", {number(snapshot.id)});
    executeParams(connection, "DELETE FROM families WHERE simulation_id=$1::bigint", {number(snapshot.id)});
    executeParams(connection, "DELETE FROM persons WHERE simulation_id=$1::bigint", {number(snapshot.id)});

    for (const auto& person : snapshot.world.people) {
        executeParams(connection,
            "INSERT INTO persons(simulation_id, person_id, name, gender, age, alive, pregnant, mother_id, father_id, family_id) "
            "VALUES($1::bigint,$2::bigint,$3,$4::smallint,$5::integer,$6::boolean,$7::boolean,$8::bigint,$9::bigint,$10::bigint)",
            {number(snapshot.id), number(person.id), person.name,
             number(static_cast<int>(person.gender)), number(person.age),
             person.alive ? "true" : "false", person.pregnant ? "true" : "false",
             nullableId(person.motherId), nullableId(person.fatherId), nullableId(person.familyId)});
    }

    for (const auto& family : snapshot.world.families) {
        executeParams(connection,
            "INSERT INTO families(simulation_id,family_id,mother_id,father_id,surname) "
            "VALUES($1::bigint,$2::bigint,$3::bigint,$4::bigint,$5)",
            {number(snapshot.id), number(family.id), number(family.motherId),
             number(family.fatherId), family.surname});
        for (std::size_t i = 0; i < family.childrenIds.size(); ++i) {
            executeParams(connection,
                "INSERT INTO family_children(simulation_id,family_id,child_id,position) "
                "VALUES($1::bigint,$2::bigint,$3::bigint,$4::integer)",
                {number(snapshot.id), number(family.id), number(family.childrenIds[i]), number(static_cast<int>(i))});
        }
    }
}

} // namespace

PostgresSimulationRepository::PostgresSimulationRepository(const std::string& connectionString) {
    connection_ = PQconnectdb(connectionString.c_str());
    if (!connection_ || PQstatus(connection_) != CONNECTION_OK) {
        const std::string message = connection_ ? PQerrorMessage(connection_) : "could not allocate PostgreSQL connection";
        if (connection_) PQfinish(connection_);
        connection_ = nullptr;
        throw std::runtime_error("Could not connect to PostgreSQL: " + message);
    }
    try {
        applyMigrations();
    } catch (...) {
        PQfinish(connection_);
        connection_ = nullptr;
        throw;
    }
}

PostgresSimulationRepository::~PostgresSimulationRepository() {
    if (connection_) PQfinish(connection_);
}

std::string PostgresSimulationRepository::connectionStringFromEnvironment() {
    const char* value = std::getenv("FTREE_DATABASE_URL");
    if (!value || !*value) throw std::runtime_error("FTREE_DATABASE_URL environment variable is not set");
    return value;
}

void PostgresSimulationRepository::applyMigrations() {
    execute(connection_, "BEGIN");
    try {
        execute(connection_,
            "CREATE TABLE IF NOT EXISTS schema_migrations (version INTEGER PRIMARY KEY, applied_at TIMESTAMPTZ NOT NULL DEFAULT NOW())");
        execute(connection_, "SELECT pg_advisory_xact_lock(74219831)");
        auto versions = execute(connection_, "SELECT COALESCE(MAX(version),0) FROM schema_migrations");
        const int currentVersion = std::stoi(PQgetvalue(versions.get(), 0, 0));
        if (currentVersion > 1) {
            throw std::runtime_error("Database schema is newer than this ftree build supports");
        }
        if (currentVersion == 0) {
#ifndef FTREE_MIGRATIONS_DIR
#define FTREE_MIGRATIONS_DIR "migrations"
#endif
            const char* configuredMigrationDirectory = std::getenv("FTREE_MIGRATIONS_DIR");
            const std::string migrationDirectory =
                configuredMigrationDirectory && *configuredMigrationDirectory
                    ? configuredMigrationDirectory
                    : FTREE_MIGRATIONS_DIR;
            const std::string migrationPath = migrationDirectory + "/001_initial_schema.sql";
            std::ifstream migrationFile(migrationPath);
            if (!migrationFile) {
                throw std::runtime_error("Could not open database migration: " + migrationPath);
            }
            std::ostringstream migrationSql;
            migrationSql << migrationFile.rdbuf();
            if (!migrationFile.good() && !migrationFile.eof()) {
                throw std::runtime_error("Failed while reading database migration: " + migrationPath);
            }
            execute(connection_, migrationSql.str());
        }
        execute(connection_, "COMMIT");
    } catch (...) {
        try { execute(connection_, "ROLLBACK"); } catch (...) {}
        throw;
    }
}

Id PostgresSimulationRepository::createSimulation(const Snapshot& source) {
    Snapshot snapshot = source;
    validateSnapshot(snapshot);
    execute(connection_, "BEGIN");
    try {
        auto result = executeParams(connection_,
            "INSERT INTO simulations(name,current_year,config_data,rng_seed,rng_state,names_data,next_person_id,next_family_id) "
            "VALUES($1,$2::integer,$3,$4::bigint,$5,$6,$7::bigint,$8::bigint) RETURNING id",
            {snapshot.name, number(snapshot.currentYear), serializeConfig(snapshot.config),
             number(snapshot.randomSeed), snapshot.randomEngineState, serializeNames(snapshot.nameDatabase),
             number(snapshot.world.nextPersonId), number(snapshot.world.nextFamilyId)});
        snapshot.id = std::stoll(PQgetvalue(result.get(), 0, 0));
        execute(connection_, "SET CONSTRAINTS ALL DEFERRED");
        // The metadata row is already inserted; saveRows updates it and persists all entities.
        saveRows(connection_, snapshot);
        execute(connection_, "COMMIT");
        return snapshot.id;
    } catch (...) {
        try { execute(connection_, "ROLLBACK"); } catch (...) {}
        throw;
    }
}

void PostgresSimulationRepository::saveSimulation(const Snapshot& snapshot) {
    if (snapshot.id <= 0) throw std::invalid_argument("Cannot save a simulation without a valid ID");
    validateSnapshot(snapshot);
    execute(connection_, "BEGIN");
    try {
        execute(connection_, "SET CONSTRAINTS ALL DEFERRED");
        saveRows(connection_, snapshot);
        execute(connection_, "COMMIT");
    } catch (...) {
        try { execute(connection_, "ROLLBACK"); } catch (...) {}
        throw;
    }
}

Snapshot PostgresSimulationRepository::loadSimulation(Id id) {
    execute(connection_, "BEGIN ISOLATION LEVEL REPEATABLE READ READ ONLY");
    try {
    auto metadata = executeParams(connection_,
        "SELECT name,current_year,config_data,rng_seed,rng_state,names_data,next_person_id,next_family_id,schema_version "
        "FROM simulations WHERE id=$1::bigint", {number(id)});
    if (PQntuples(metadata.get()) != 1) throw std::runtime_error("Simulation ID was not found");

    Snapshot snapshot;
    snapshot.id = id;
    snapshot.name = PQgetvalue(metadata.get(), 0, 0);
    snapshot.currentYear = std::stoi(PQgetvalue(metadata.get(), 0, 1));
    snapshot.config = deserializeConfig(PQgetvalue(metadata.get(), 0, 2));
    const auto storedSeed = std::stoull(PQgetvalue(metadata.get(), 0, 3));
    if (storedSeed > std::numeric_limits<std::uint32_t>::max()) {
        throw std::runtime_error("Stored random seed exceeds uint32 range");
    }
    snapshot.randomSeed = static_cast<std::uint32_t>(storedSeed);
    snapshot.randomEngineState = PQgetvalue(metadata.get(), 0, 4);
    snapshot.nameDatabase = deserializeNames(PQgetvalue(metadata.get(), 0, 5));
    snapshot.world.nextPersonId = static_cast<std::uint32_t>(std::stoull(PQgetvalue(metadata.get(), 0, 6)));
    snapshot.world.nextFamilyId = static_cast<std::uint32_t>(std::stoull(PQgetvalue(metadata.get(), 0, 7)));
    const int schemaVersion = std::stoi(PQgetvalue(metadata.get(), 0, 8));
    if (schemaVersion != 1) throw std::runtime_error("Unsupported simulation snapshot schema version");

    auto people = executeParams(connection_,
        "SELECT person_id,name,gender,age,alive,pregnant,mother_id,father_id,family_id "
        "FROM persons WHERE simulation_id=$1::bigint ORDER BY person_id", {number(id)});
    for (int row = 0; row < PQntuples(people.get()); ++row) {
        domain::PersonData p;
        p.id = readId(people.get(), row, 0);
        p.name = PQgetvalue(people.get(), row, 1);
        const int gender = std::stoi(PQgetvalue(people.get(), row, 2));
        if (gender != 0 && gender != 1) throw std::runtime_error("Stored person has invalid gender");
        p.gender = static_cast<domain::Gender>(gender);
        p.age = std::stoi(PQgetvalue(people.get(), row, 3));
        p.alive = std::string(PQgetvalue(people.get(), row, 4)) == "t";
        p.pregnant = std::string(PQgetvalue(people.get(), row, 5)) == "t";
        p.motherId = readId(people.get(), row, 6);
        p.fatherId = readId(people.get(), row, 7);
        p.familyId = readId(people.get(), row, 8);
        snapshot.world.people.push_back(std::move(p));
    }

    auto families = executeParams(connection_,
        "SELECT family_id,mother_id,father_id,surname FROM families "
        "WHERE simulation_id=$1::bigint ORDER BY family_id", {number(id)});
    for (int row = 0; row < PQntuples(families.get()); ++row) {
        domain::FamilyData f;
        f.id = readId(families.get(), row, 0);
        f.motherId = readId(families.get(), row, 1);
        f.fatherId = readId(families.get(), row, 2);
        f.surname = PQgetvalue(families.get(), row, 3);
        snapshot.world.families.push_back(std::move(f));
    }
    auto children = executeParams(connection_,
        "SELECT family_id,child_id FROM family_children WHERE simulation_id=$1::bigint ORDER BY family_id,position",
        {number(id)});
    for (int row = 0; row < PQntuples(children.get()); ++row) {
        const auto familyId = readId(children.get(), row, 0);
        const auto childId = readId(children.get(), row, 1);
        auto it = std::find_if(snapshot.world.families.begin(), snapshot.world.families.end(),
            [familyId](const domain::FamilyData& f) { return f.id == familyId; });
        if (it == snapshot.world.families.end()) throw std::runtime_error("Stored child references missing family");
        it->childrenIds.push_back(childId);
    }
    // Validate the complete persisted state before exposing it to the application.
    validateSnapshot(snapshot);
    execute(connection_, "COMMIT");
    return snapshot;
    } catch (...) {
        try { execute(connection_, "ROLLBACK"); } catch (...) {}
        throw;
    }
}

std::vector<Summary> PostgresSimulationRepository::listSimulations() {
    auto result = execute(connection_,
        "SELECT id,name,current_year,to_char(updated_at AT TIME ZONE 'UTC','YYYY-MM-DD HH24:MI:SS') "
        "FROM simulations ORDER BY updated_at DESC,id DESC");
    std::vector<Summary> items;
    items.reserve(static_cast<std::size_t>(PQntuples(result.get())));
    for (int row = 0; row < PQntuples(result.get()); ++row) {
        Summary summary;
        summary.id = std::stoll(PQgetvalue(result.get(), row, 0));
        summary.name = PQgetvalue(result.get(), row, 1);
        summary.currentYear = std::stoi(PQgetvalue(result.get(), row, 2));
        summary.updatedAt = PQgetvalue(result.get(), row, 3);
        items.push_back(std::move(summary));
    }
    return items;
}

void PostgresSimulationRepository::deleteSimulation(Id id) {
    executeParams(connection_, "DELETE FROM simulations WHERE id=$1::bigint", {number(id)});
}

} // namespace ftree::infrastructure::postgres
