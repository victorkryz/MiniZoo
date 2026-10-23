#include "crypto.h"
#include "database.h"

#include <cstdint>
#include <exception>
#include <iostream>
#include <string>
#include <vector>

void AnimalActions();
void AesActions();
void DisplayAnimals(const std::vector<Database::AnimalInfo>& animals);

int main()
{
    try
    {
        std::cout << std::endl
                  << "Welcome to MiniZoo!" << std::endl
                  << std::endl;

        // Cleanup and initialize the database to ensure a clean state for this run.
        Database::Cleanup();
        Database::Initialize();

        // Perform animal actions:
        AnimalActions();

        // Perform AES actions:
        AesActions();

        std::cout << std::endl;

        return 0;
    }
    catch (const std::exception& exception)
    {
        std::cerr << "MiniZoo actions failed: " << exception.what() << '\n';
        return 1;
    }
}

void AnimalActions()
{
    std::cout << "Animals in initial state:" << std::endl;
    std::cout << "-------------------------" << std::endl;
    DisplayAnimals(Database::ReadAnimalsWithEnclosures());

    Database::MoveAnimalToEnclosure("Parrot", "Savanna");

    std::cout << std::endl;
    std::cout << "Animals after moving parrot to Savanna:" << std::endl;
    std::cout << "---------------------------------------" << std::endl;
    DisplayAnimals(Database::ReadAnimalsWithEnclosures());
}

void AesActions()
{
    const std::string tigerNote =
        "The tiger (Panthera tigris) is the largest wild member of the cat "
        "family and a powerful apex predator known for its distinctive orange "
        "coat and dark vertical stripes.";
    const std::string sharkNote =
        "Sharks are a group of ancient, cartilaginous fish that have lived in "
        "the Earth's oceans for more than 400 million years, predating the "
        "dinosaurs.";

    // clang-format off
    const std::vector<std::uint8_t> key = {
        0x30, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6,
        0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c,
        0x76, 0x2e, 0x71, 0x60, 0xf3, 0x8b, 0x4d, 0xa5,
        0x6a, 0x78, 0x4d, 0x90, 0x45, 0x19, 0x0c, 0xfe,
    };
    // clang-format on

    const std::vector<Database::AnimalNoteToStore> notes = {
        {"Tiger", Encrypt(tigerNote, key)},
        {"Shark", Encrypt(sharkNote, key)},
    };
    Database::AddEncryptedAnimalNotes(notes);

    std::cout << std::endl;
    std::cout << "Decrypted animal notes:" << std::endl;
    std::cout << "-----------------------" << std::endl;

    for (const Database::EncryptedAnimalNote& note :
         Database::ReadEncryptedAnimalNotes())
    {
        std::cout << note.species << " (" << note.animalName << "): "
                  << Decrypt(note.encryptedNote, key) << std::endl;
    }
}

void DisplayAnimals(const std::vector<Database::AnimalInfo>& animals)
{
    for (const Database::AnimalInfo& animal : animals)
    {
        std::cout << animal.species << " (" << animal.name << ") -> "
                  << animal.enclosure << std::endl;
    }
}
