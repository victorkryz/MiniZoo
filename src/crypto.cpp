#include "crypto.h"

#include <Poco/Crypto/Cipher.h>
#include <Poco/Crypto/CipherFactory.h>
#include <Poco/Crypto/CipherKey.h>

#include <cstddef>
#include <stdexcept>

namespace
{
    constexpr auto CipherName = "aes-256-cbc";
    constexpr std::size_t Aes256KeySize = 32;
    constexpr std::size_t AesBlockSize = 16;

    Poco::Crypto::CipherKey::ByteVec ConvertToCipherKeyBytes(
        const std::vector<std::uint8_t>& bytes)
    {
        return {bytes.begin(), bytes.end()};
    }

    void ValidateKey(const std::vector<std::uint8_t>& key)
    {
        if (key.size() != Aes256KeySize)
        {
            throw std::invalid_argument("AES-256 requires a 32-byte key");
        }
    }
} // namespace

std::vector<std::uint8_t> Encrypt(const std::string& text,
                                  const std::vector<std::uint8_t>& key)
{
    ValidateKey(key);

    // Generate a new random IV (Initialization Vector) for every encryption.
    // It is not secret, so it is  stored as the first AES block of
    // the returned byte vector.
    const Poco::Crypto::CipherKey randomKey(CipherName);
    const auto& iv = randomKey.getIV();

    const Poco::Crypto::CipherKey cipherKey(
        CipherName, ConvertToCipherKeyBytes(key), iv);

    Poco::Crypto::Cipher::Ptr aesCipher =
        Poco::Crypto::CipherFactory::defaultFactory().createCipher(cipherKey);

    const std::string encrypted =
        aesCipher->encryptString(text, Poco::Crypto::Cipher::ENC_NONE);

    std::vector<std::uint8_t> result;
    result.reserve(iv.size() + encrypted.size());
    result.insert(result.end(), iv.begin(), iv.end());
    result.insert(result.end(), encrypted.begin(), encrypted.end());
    return result;
}

std::string Decrypt(const std::vector<std::uint8_t>& cipher,
                    const std::vector<std::uint8_t>& key)
{
    ValidateKey(key);

    if (cipher.size() <= AesBlockSize ||
        (cipher.size() - AesBlockSize) % AesBlockSize != 0)
    {
        throw std::invalid_argument("Invalid AES cipher data");
    }

    const Poco::Crypto::CipherKey::ByteVec iv(
        cipher.begin(), cipher.begin() + AesBlockSize);

    const std::string encrypted(
        cipher.begin() + AesBlockSize, cipher.end());

    const Poco::Crypto::CipherKey cipherKey(
        CipherName, ConvertToCipherKeyBytes(key), iv);

    Poco::Crypto::Cipher::Ptr aesCipher =
        Poco::Crypto::CipherFactory::defaultFactory().createCipher(cipherKey);

    return aesCipher->decryptString(encrypted, Poco::Crypto::Cipher::ENC_NONE);
}
