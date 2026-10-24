#include "database.h"

#include <Poco/Data/LOB.h>
#include <Poco/Data/Session.h>
#include <Poco/Data/SQLite/Connector.h>
#include <Poco/Data/Transaction.h>

#include <cstddef>
#include <filesystem>
#include <string>
#include <vector>

namespace Database
{
    constexpr auto DatabaseFileName = "minizoo.db";

    SQLiteConnectorGuard::SQLiteConnectorGuard()
    {
        Poco::Data::SQLite::Connector::registerConnector();
    }

    SQLiteConnectorGuard::~SQLiteConnectorGuard() noexcept
    {
        Poco::Data::SQLite::Connector::unregisterConnector();
    }

    void CreateSchema(Poco::Data::Session& session)
    {
        using Poco::Data::Keywords::now;

        session << R"sql(
        CREATE TABLE IF NOT EXISTS Enclosures (
            id INTEGER PRIMARY KEY,
            name TEXT NOT NULL UNIQUE
        )
    )sql",
            now;

        session << R"sql(
        CREATE TABLE IF NOT EXISTS Animals (
            id INTEGER PRIMARY KEY,
            name TEXT NOT NULL,
            species TEXT NOT NULL,
            age INTEGER NOT NULL CHECK(age >= 0),
            enclosure_id INTEGER,
            FOREIGN KEY(enclosure_id) REFERENCES Enclosures(id)
        )
    )sql",
            now;

        session << R"sql(
        CREATE TABLE IF NOT EXISTS AnimalNotes (
            animal_id INTEGER,
            encrypted_note BLOB,
            FOREIGN KEY(animal_id) REFERENCES Animals(id)
        )
    )sql",
            now;

        session << R"sql(
        CREATE INDEX IF NOT EXISTS idx_animals_enclosure_id
        ON Animals(enclosure_id)
    )sql",
            now;
    }

    void PopulateDatabaseWithAnimals(Poco::Data::Session& session)
    {
        using Poco::Data::Keywords::now;

        Poco::Data::Transaction transaction(session);

        session << R"sql(
        WITH SeedEnclosures(name) AS (
            VALUES ('Savanna'), ('Aquarium'), ('Birds')
        )
        INSERT INTO Enclosures (name)
        SELECT seed.name
        FROM SeedEnclosures AS seed
        WHERE NOT EXISTS (
            SELECT 1
            FROM Enclosures AS enclosure
            WHERE enclosure.name = seed.name
        )
    )sql",
            now;

        session << R"sql(
        WITH SeedAnimals(name, species, age, enclosure_name) AS (
            VALUES
                ('Leo',   'Lion',   8,  'Savanna'),
                ('Amber', 'Tiger',  6,  'Savanna'),
                ('Finn',  'Shark',  12, 'Aquarium'),
                ('Polly', 'Parrot', 4,  'Birds')
        )
        INSERT INTO Animals (name, species, age, enclosure_id)
        SELECT
            seed.name,
            seed.species,
            seed.age,
            (
                SELECT enclosure.id
                FROM Enclosures AS enclosure
                WHERE enclosure.name = seed.enclosure_name
            )
        FROM SeedAnimals AS seed
        WHERE NOT EXISTS (
            SELECT 1
            FROM Animals AS animal
            WHERE animal.name = seed.name
              AND animal.species = seed.species
        )
    )sql",
            now;

        transaction.commit();
    }

    void Cleanup()
    {
        const std::filesystem::path databasePath(DatabaseFileName);

        if (std::filesystem::exists(databasePath))
        {
            std::filesystem::remove(databasePath);
        }
    }

    void Initialize()
    {
        Poco::Data::Session session("SQLite", DatabaseFileName);
        session << "PRAGMA foreign_keys = ON", Poco::Data::Keywords::now;

        CreateSchema(session);
        PopulateDatabaseWithAnimals(session);
    }

    std::vector<AnimalInfo> ReadAnimalsWithEnclosures()
    {
        std::vector<std::string> species, names, enclosures;

        Poco::Data::Session session("SQLite", DatabaseFileName);
        session << R"sql(
            SELECT anm.species AS species,
                   anm.name AS nme,
                   enc.name AS enclosure
            FROM Animals anm
            INNER JOIN Enclosures enc
                ON anm.enclosure_id = enc.id
        )sql",
            Poco::Data::Keywords::into(species),
            Poco::Data::Keywords::into(names),
            Poco::Data::Keywords::into(enclosures),
            Poco::Data::Keywords::now;

        std::vector<AnimalInfo> animals;
        animals.reserve(species.size());

        for (std::size_t index = 0; index < species.size(); ++index)
        {
            animals.push_back({species[index], names[index], enclosures[index]});
        }

        return animals;
    }

    void MoveAnimalToEnclosure(
        const std::string& species,
        const std::string& enclosureName)
    {
        std::string boundEnclosureName = enclosureName,
                    boundSpecies = species;

        Poco::Data::Session session("SQLite", DatabaseFileName);
        session << R"sql(
                UPDATE Animals
                SET enclosure_id = (
                    SELECT id
                    FROM Enclosures
                    WHERE name = ?
                )
                WHERE species = ?
            )sql",
                    Poco::Data::Keywords::use(boundEnclosureName),
                    Poco::Data::Keywords::use(boundSpecies),
                    Poco::Data::Keywords::now;
    }

    void AddEncryptedAnimalNotes(
        const std::vector<AnimalNoteToStore>& notes)
    {
        Poco::Data::Session session("SQLite", DatabaseFileName);
        Poco::Data::Transaction transaction(session);

        for (const AnimalNoteToStore& note : notes)
        {
            std::string species = note.species;
            const std::vector<unsigned char> encryptedBytes(
                note.encryptedNote.begin(), note.encryptedNote.end());
            Poco::Data::BLOB encryptedNote(encryptedBytes);

            session << R"sql(
                    INSERT INTO AnimalNotes (animal_id, encrypted_note)
                    SELECT id, ?
                    FROM Animals
                    WHERE species = ?
                )sql",
                        Poco::Data::Keywords::use(encryptedNote),
                        Poco::Data::Keywords::use(species),
                        Poco::Data::Keywords::now;
        }

        transaction.commit();
    }

    std::vector<EncryptedAnimalNote> ReadEncryptedAnimalNotes()
    {
        std::vector<std::string> species, animalNames;
        std::vector<Poco::Data::BLOB> encryptedNotes;

        Poco::Data::Session session("SQLite", DatabaseFileName);
        session << R"sql(
                SELECT animal.species,
                       animal.name,
                       note.encrypted_note
                FROM AnimalNotes AS note
                INNER JOIN Animals AS animal
                    ON note.animal_id = animal.id
                ORDER BY animal.id
            )sql",
                    Poco::Data::Keywords::into(species),
                    Poco::Data::Keywords::into(animalNames),
                    Poco::Data::Keywords::into(encryptedNotes),
                    Poco::Data::Keywords::now;

        std::vector<EncryptedAnimalNote> notes;
        notes.reserve(encryptedNotes.size());

        for (std::size_t index = 0; index < encryptedNotes.size(); ++index)
        {
            const auto& content = encryptedNotes[index].content();
            notes.push_back({species[index],
                             animalNames[index],
                             {content.begin(), content.end()}});
        }

        return notes;
    }

} // namespace Database
