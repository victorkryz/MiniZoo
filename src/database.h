#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace Database
{
    struct AnimalInfo
    {
        std::string species;
        std::string name;
        std::string enclosure;
    };

    struct AnimalNoteToStore
    {
        std::string species;
        std::vector<std::uint8_t> encryptedNote;
    };

    struct EncryptedAnimalNote
    {
        std::string species;
        std::string animalName;
        std::vector<std::uint8_t> encryptedNote;
    };

    void Cleanup();

    void Initialize();

    std::vector<AnimalInfo> ReadAnimalsWithEnclosures();

    void MoveAnimalToEnclosure(
        const std::string& species,
        const std::string& enclosureName);

    void AddEncryptedAnimalNotes(
        const std::vector<AnimalNoteToStore>& notes);

    std::vector<EncryptedAnimalNote> ReadEncryptedAnimalNotes();

} // namespace Database
