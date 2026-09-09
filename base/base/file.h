#pragma once

#include <vector>
#include <cstddef>
#include <filesystem>
#include <span>

namespace Rc
{
    std::vector<std::byte> ReadFile(std::filesystem::path path);
    std::span<std::byte> ReadFileTo(std::filesystem::path path, std::span<std::byte> buffer);
    //std::vector<std::byte> ReadFile(std::filesystem::path path, std::span<std::byte> buffer);

} // Rc