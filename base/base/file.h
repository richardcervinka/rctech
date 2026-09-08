#pragma once

#include <vector>
#include <cstddef>
#include <filesystem>

namespace Rc
{
    std::vector<std::byte> ReadFile(std::filesystem::path path);
    //std::vector<std::byte> ReadFile(std::filesystem::path path, std::span<std::byte> buffer);

} // Rc