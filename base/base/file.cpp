#include "file.h"
#include <fstream>

namespace Rc
{
    std::vector<std::byte> ReadFile(std::filesystem::path path)
    {
        std::ifstream file(path, std::ios::binary | std::ios::ate);

        if (!file)
        {
            throw std::runtime_error("File error");
        }

        auto const size = file.tellg();
        file.seekg(0, std::ios::beg);

        std::vector<std::byte> result(size);
        file.read(reinterpret_cast<char*>(result.data()), size); //----------- return value?
        return result;
    }

} // Rc