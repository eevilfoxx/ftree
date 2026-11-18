# PostgreSQL persistence

PostgreSQL is the persistent source of truth. `domain::World` is loaded into memory for simulation work and a complete snapshot is committed after every completed simulation year.

## Requirements

- PostgreSQL 14 or newer (the supplied Compose file uses PostgreSQL 17)
- PostgreSQL client development library (`libpq`)
- CMake and a C++17 compiler
- SFML 2.5+ for the graphical executable

The C++ adapter uses PostgreSQL's supported `libpq` C client API. This keeps the dependency explicit and avoids introducing an ORM.

## Start a local database with Docker Compose

```bash
cp .env.example .env
# Edit .env and choose a non-default password.
docker compose up -d postgres
```

Set the connection string in the shell where the application will run:

```bash
export FTREE_DATABASE_URL='postgresql://ftree:YOUR_PASSWORD@localhost:5432/ftree'
```

Do not commit `.env`, connection strings, or credentials.

For an installed build, set `FTREE_MIGRATIONS_DIR` to the installed `share/ftree/migrations` directory if the original source tree is not present.

## Build

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

The graphical executable is built when SFML is found. The headless executable does not require SFML, but it does require PostgreSQL.

## Run

Interactive save selection:

```bash
./build/FamilyTree
./build/FamilyTreeHeadless --years 20
```

Create a simulation without an interactive prompt:

```bash
./build/FamilyTree --new "My world"
./build/FamilyTreeHeadless --new "Batch world" --years 100
```

Load an existing simulation by ID:

```bash
./build/FamilyTree --load 12
./build/FamilyTreeHeadless --load 12 --years 50
```

Delete a simulation before showing the selection list:

```bash
./build/FamilyTree --delete 12
```

When the interactive selection list is shown, enter `n` to create a world, an ID to load it, or `d ID` to delete a saved world and refresh the list.

## Persistence guarantees

- The initial world is saved when it is created.
- Every completed year is saved in one database transaction.
- A failed transaction leaves the previously committed year intact; the application reports the error and exits rather than continuing against an unsaved state.
- Loading reads metadata, people, families, and child links in a repeatable-read transaction.
- Person and family IDs, including exhausted ID counters, are preserved.
- The MT19937 engine state, seed, simulation config, and loaded name lists are preserved.
- History by year is not implemented in this version; each simulation row represents its latest committed state.

## Integration tests

Set `FTREE_TEST_DATABASE_URL` to a disposable test database before running the test suite. The PostgreSQL repository integration test is skipped when this variable is not set.

```bash
export FTREE_TEST_DATABASE_URL='postgresql://ftree:YOUR_PASSWORD@localhost:5432/ftree_test'
ctest --test-dir build --output-on-failure
```

The test creates and deletes its own simulation record. Do not point it at a production database.

The random engine state is stored using the standard stream representation of
`std::mt19937`. Exact continuation is intended for compatible builds using the
same standard-library implementation; cross-toolchain long-term reproducibility
is not guaranteed. The current repository writes a complete world snapshot per
year (transactionally). Incremental persistence can be introduced later if
population size makes full snapshots too expensive.
