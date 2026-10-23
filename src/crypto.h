#pragma once

#include <cstdint>
#include <string>
#include <vector>

std::vector<std::uint8_t> Encrypt(
    const std::string& text,
    const std::vector<std::uint8_t>& key);

std::string Decrypt(
    const std::vector<std::uint8_t>& cipher,
    const std::vector<std::uint8_t>& key);
