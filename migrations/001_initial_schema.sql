CREATE TABLE IF NOT EXISTS schema_migrations (
    version INTEGER PRIMARY KEY,
    applied_at TIMESTAMPTZ NOT NULL DEFAULT NOW()
);

CREATE TABLE IF NOT EXISTS simulations (
    id BIGSERIAL PRIMARY KEY,
    name TEXT NOT NULL,
    current_year INTEGER NOT NULL CHECK (current_year >= 0),
    config_data TEXT NOT NULL,
    rng_seed BIGINT NOT NULL CHECK (rng_seed BETWEEN 0 AND 4294967295),
    rng_state TEXT NOT NULL,
    names_data TEXT NOT NULL,
    next_person_id BIGINT NOT NULL CHECK (next_person_id BETWEEN 0 AND 4294967295),
    next_family_id BIGINT NOT NULL CHECK (next_family_id BETWEEN 0 AND 4294967295),
    schema_version INTEGER NOT NULL DEFAULT 1,
    created_at TIMESTAMPTZ NOT NULL DEFAULT NOW(),
    updated_at TIMESTAMPTZ NOT NULL DEFAULT NOW()
);
ALTER TABLE simulations ADD COLUMN IF NOT EXISTS schema_version INTEGER NOT NULL DEFAULT 1;

CREATE TABLE IF NOT EXISTS persons (
    simulation_id BIGINT NOT NULL REFERENCES simulations(id) ON DELETE CASCADE,
    person_id BIGINT NOT NULL CHECK (person_id BETWEEN 1 AND 4294967295),
    name TEXT NOT NULL,
    gender SMALLINT NOT NULL CHECK (gender IN (0,1)),
    age INTEGER NOT NULL CHECK (age >= 0),
    alive BOOLEAN NOT NULL,
    pregnant BOOLEAN NOT NULL,
    mother_id BIGINT,
    father_id BIGINT,
    family_id BIGINT,
    PRIMARY KEY (simulation_id, person_id),
    FOREIGN KEY (simulation_id, mother_id) REFERENCES persons(simulation_id, person_id) DEFERRABLE INITIALLY DEFERRED,
    FOREIGN KEY (simulation_id, father_id) REFERENCES persons(simulation_id, person_id) DEFERRABLE INITIALLY DEFERRED
);

CREATE TABLE IF NOT EXISTS families (
    simulation_id BIGINT NOT NULL REFERENCES simulations(id) ON DELETE CASCADE,
    family_id BIGINT NOT NULL CHECK (family_id BETWEEN 1 AND 4294967295),
    mother_id BIGINT NOT NULL,
    father_id BIGINT NOT NULL,
    surname TEXT NOT NULL,
    PRIMARY KEY (simulation_id, family_id),
    FOREIGN KEY (simulation_id, mother_id) REFERENCES persons(simulation_id, person_id) DEFERRABLE INITIALLY DEFERRED,
    FOREIGN KEY (simulation_id, father_id) REFERENCES persons(simulation_id, person_id) DEFERRABLE INITIALLY DEFERRED
);

DO $$ BEGIN
    ALTER TABLE persons ADD CONSTRAINT persons_family_fk
        FOREIGN KEY (simulation_id, family_id) REFERENCES families(simulation_id, family_id)
        DEFERRABLE INITIALLY DEFERRED;
EXCEPTION WHEN duplicate_object THEN NULL;
END $$;

CREATE TABLE IF NOT EXISTS family_children (
    simulation_id BIGINT NOT NULL,
    family_id BIGINT NOT NULL,
    child_id BIGINT NOT NULL,
    position INTEGER NOT NULL CHECK (position >= 0),
    PRIMARY KEY (simulation_id, family_id, child_id),
    UNIQUE (simulation_id, family_id, position),
    FOREIGN KEY (simulation_id, family_id) REFERENCES families(simulation_id, family_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED,
    FOREIGN KEY (simulation_id, child_id) REFERENCES persons(simulation_id, person_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED
);

INSERT INTO schema_migrations(version) VALUES (1) ON CONFLICT DO NOTHING;
