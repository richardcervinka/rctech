#include "gtest/gtest.h"
#include "core/ktx.h"
#include "base/file.h"
#include <filesystem>

using namespace Rc;

TEST(KtxReader, ReadHeader)
{
    auto data = Rc::ReadFile("C:\\Users\\richa\\cpp\\rctech\\dev\\valid_R8G8B8A8_SRGB_2D.ktx2");

    KtxReader ktx(data);

    auto w = ktx.Width();
    auto h = ktx.Height();
    auto l = ktx.LevelCount();

    SUCCEED();
    // std::ifstream file(path, std::ios::binary | std::ios::ate);
    // if (!file) return {};

    // auto size = file.tellg();
    // file.seekg(0, std::ios::beg);

    // std::vector<std::byte> buffer(size);
    // file.read(reinterpret_cast<char*>(buffer.data()), size);

    // return buffer;
    //KtxReader reader(
}