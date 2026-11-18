ftree — family and population simulation

The application stores persistent simulations in PostgreSQL. The domain World is
loaded into memory while a simulation runs; after every completed simulation
year the complete state is committed to PostgreSQL in a single transaction.

See docs/postgresql.md for database setup, build instructions, command-line
options, persistence guarantees, and integration-test configuration.

Quick start (Docker Compose):
  cp .env.example .env
  # Change FTREE_POSTGRES_PASSWORD in .env.
  docker compose up -d postgres
  export FTREE_DATABASE_URL='postgresql://ftree:YOUR_PASSWORD@localhost:5432/ftree'
  cmake -S . -B build -DBUILD_TESTING=ON
  cmake --build build --parallel

Interactive save selection:
  ./build/FamilyTree
  ./build/FamilyTreeHeadless --years 20

Create or load directly:
  ./build/FamilyTree --new "My world"
  ./build/FamilyTree --load 1
  ./build/FamilyTreeHeadless --new "Batch world" --years 100
  ./build/FamilyTreeHeadless --load 1 --years 50

Dependencies:
  - C++17 compiler and CMake
  - PostgreSQL client development library (libpq)
  - PostgreSQL server
  - SFML 2.5+ for the graphical executable
  - GoogleTest (optional, for the test executable)
