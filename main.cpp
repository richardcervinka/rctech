#include "generic/application.h"
#include "base/float.h"

using namespace Rc;

class TestApplication : public Generic::Application
{
public:
    using Generic::Application::Application;

    void Initialize() override;

    void BeginFrame() override
    {
        Generic::Application::BeginFrame();
        
        if (test_model != nullptr)
        {
            if (GetRenderer().QueryUpload(resource_timeline))
            {
                GetRenderer().test_model = std::move(test_model);
                test_model = nullptr;
            }
        }

        static double step = 0;
        step += 0.005;

        // Test animation
        if (auto& tm = GetRenderer().test_model)
        {
            int size = 420;

            for (int r = 0; r < size; r++)
            {
                for (int c = 0; c < size; c++)
                {
                    tm->instances[r * size + c].y = Math::Sin(step + ((Math::pi * r) / 10.0) + ((Math::pi * c) / 10.0)) * 0.3;
                    tm->instances[r * size + c].scale = 0.35 + Math::Sin(step + ((Math::pi * r) / size) + ((Math::pi * c) / size)) * 0.02;
                    //tm->instances[r * size + c].yaw = step;
                }
            }
        }
    }

    std::unique_ptr<TestModel> test_model;
    uint64_t resource_timeline {0};
};

void TestApplication::Initialize()
{
    Generic::Application::Initialize();

    auto& renderer = GetRenderer();

    //auto& renderer = GetRenderer();
    //auto& rm = GetResourceManager();

    test_model = std::make_unique<TestModel>();

    renderer.BeginUpload();

    test_model->vb_handle = renderer.AllocateVertexBuffer(Render::ResourceFamily{0}, sizeof(Gfx::VertexBasic) * 24);

    resource_timeline = renderer.Upload(test_model->vb_handle, [](Render::BufferWriter& writer) {
        // Front
        writer << Float3{-1, -1,  1}; writer << Float2{0.01, 0.99};
        writer << Float3{ 1,  1,  1}; writer << Float2{0.99, 0.01};
        writer << Float3{-1,  1,  1}; writer << Float2{0.01, 0.01};
        writer << Float3{ 1, -1,  1}; writer << Float2{0.99, 0.99};
        // Baack
        writer << Float3{-1, -1, -1}; writer << Float2{0.01, 0.99};
        writer << Float3{ 1,  1, -1}; writer << Float2{0.99, 0.01};
        writer << Float3{-1,  1, -1}; writer << Float2{0.01, 0.01};
        writer << Float3{ 1, -1, -1}; writer << Float2{0.99, 0.99};
        // Left
        writer << Float3{-1, -1, -1}; writer << Float2{0.01, 0.99};
        writer << Float3{-1,  1,  1}; writer << Float2{0.99, 0.01};
        writer << Float3{-1,  1, -1}; writer << Float2{0.01, 0.01};
        writer << Float3{-1, -1,  1}; writer << Float2{0.99, 0.99};
        // Right
        writer << Float3{ 1, -1,  1}; writer << Float2{0.01, 0.99};
        writer << Float3{ 1,  1, -1}; writer << Float2{0.99, 0.01};
        writer << Float3{ 1,  1,  1}; writer << Float2{0.01, 0.01};
        writer << Float3{ 1, -1, -1}; writer << Float2{0.99, 0.99};
        // Top
        writer << Float3{-1,  1,  1}; writer << Float2{0.01, 0.99};
        writer << Float3{ 1,  1, -1}; writer << Float2{0.99, 0.01};
        writer << Float3{-1,  1, -1}; writer << Float2{0.01, 0.01};
        writer << Float3{ 1,  1,  1}; writer << Float2{0.99, 0.99};
        // Bottom
        writer << Float3{-1, -1, -1}; writer << Float2{0.01, 0.99};
        writer << Float3{ 1, -1,  1}; writer << Float2{0.99, 0.01};
        writer << Float3{-1, -1,  1}; writer << Float2{0.01, 0.01};
        writer << Float3{ 1, -1, -1}; writer << Float2{0.99, 0.99};
    });
    
    test_model->ib_handle = renderer.AllocateIndexBuffer(Render::ResourceFamily{0}, sizeof(uint16_t) * 36);

    resource_timeline = renderer.Upload(test_model->ib_handle, [](Render::BufferWriter& writer) {
        writer << std::array<uint16_t, 36>{
            // front
            0, 1, 2,
            0, 3, 1,
            // back
            6, 5, 4,
            5, 7, 4,
            // left
            8, 9, 10,
            8, 11, 9,
            // right
            12, 13, 14,
            12, 15, 13,
            // top
            16, 17, 18,
            16, 19, 17,
            // bottom
            20, 21, 22,
            20, 23, 21
        };
    });

    resource_timeline = renderer.Upload(Rc::Render::Texture2dHandle{}, [](Render::BufferWriter& writer) {

        static const uint32_t data[256]
        {
            0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0xFF30D0FF, 0xFF80E8FF, 0xFF80E8FF, 0xFF80E8FF, 0xFF80E8FF, 0xFF30D0FF, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
            0x00000000, 0x00000000, 0x00000000, 0xFF30D0FF, 0xFF80E8FF, 0xFFC0F0FF, 0xFFC0F0FF, 0xFFC0F0FF, 0xFFC0F0FF, 0xFFC0F0FF, 0xFFC0F0FF, 0xFF80E8FF, 0xFF30D0FF, 0x00000000, 0x00000000, 0x00000000,
            0x00000000, 0x00000000, 0xFF30D0FF, 0xFF80E8FF, 0xFFC0F0FF, 0xFFC0F0FF, 0xFFC0F0FF, 0xFFC0F0FF, 0xFFC0F0FF, 0xFFC0F0FF, 0xFFC0F0FF, 0xFFC0F0FF, 0xFF80E8FF, 0xFF30D0FF, 0x00000000, 0x00000000,
            0x00000000, 0xFF30D0FF, 0xFF80E8FF, 0xFFC0F0FF, 0xFFC0F0FF, 0xFFC0F0FF, 0xFFC0F0FF, 0xFFC0F0FF, 0xFFC0F0FF, 0xFFC0F0FF, 0xFFC0F0FF, 0xFFC0F0FF, 0xFFC0F0FF, 0xFF80E8FF, 0xFF30D0FF, 0x00000000,
            0x00000000, 0xFF30D0FF, 0xFF80E8FF, 0xFFC0F0FF, 0xFF202020, 0xFF202020, 0xFFC0F0FF, 0xFFC0F0FF, 0xFFC0F0FF, 0xFFC0F0FF, 0xFF202020, 0xFF202020, 0xFFC0F0FF, 0xFF80E8FF, 0xFF30D0FF, 0x00000000,
            0xFF30D0FF, 0xFF80E8FF, 0xFFC0F0FF, 0x80303030, 0xFF202020, 0xFF202020, 0x80303030, 0xFFC0F0FF, 0xFFC0F0FF, 0x80303030, 0xFF202020, 0xFF202020, 0x80303030, 0xFFC0F0FF, 0xFF80E8FF, 0xFF30D0FF,
            0xFF30D0FF, 0xFF80E8FF, 0xFF80E8FF, 0x80303030, 0xFF202020, 0xFF202020, 0x80303030, 0xFF80E8FF, 0xFF80E8FF, 0x80303030, 0xFF202020, 0xFF202020, 0x80303030, 0xFF80E8FF, 0xFF80E8FF, 0xFF30D0FF,
            0xFF30D0FF, 0xFF80E8FF, 0xFF80E8FF, 0xFF80E8FF, 0xFF80E8FF, 0xFF80E8FF, 0xFF80E8FF, 0xFF80E8FF, 0xFF80E8FF, 0xFF80E8FF, 0xFF80E8FF, 0xFF80E8FF, 0xFF80E8FF, 0xFF80E8FF, 0xFF80E8FF, 0xFF30D0FF,
            0xFF30D0FF, 0xFF30D0FF, 0xFF80E8FF, 0xFF80E8FF, 0xFF80E8FF, 0xFF80E8FF, 0xFF80E8FF, 0xFF80E8FF, 0xFF80E8FF, 0xFF80E8FF, 0xFF80E8FF, 0xFF80E8FF, 0xFF80E8FF, 0xFF30D0FF, 0xFF30D0FF, 0xFF30D0FF,
            0xFF30D0FF, 0xFF10A0FF, 0xFF30D0FF, 0x80303030, 0xFF80E8FF, 0xFF80E8FF, 0xFF80E8FF, 0xFF80E8FF, 0xFF80E8FF, 0xFF80E8FF, 0xFF80E8FF, 0x80303030, 0xFF30D0FF, 0xFF10A0FF, 0xFF30D0FF, 0xFF30D0FF,
            0xFF10A0FF, 0xFF10A0FF, 0xFF10A0FF, 0xFF30D0FF, 0xFF202020, 0xFF80E8FF, 0xFF80E8FF, 0xFF80E8FF, 0xFF80E8FF, 0xFF80E8FF, 0xFF202020, 0xFF30D0FF, 0xFF10A0FF, 0xFF10A0FF, 0xFF30D0FF, 0xFF30D0FF,
            0x00000000, 0xFF10A0FF, 0xFF10A0FF, 0xFF10A0FF, 0xFF30D0FF, 0xFF202020, 0xFF202020, 0xFF202020, 0xFF202020, 0xFF202020, 0xFF30D0FF, 0xFF10A0FF, 0xFF10A0FF, 0xFF10A0FF, 0xFF30D0FF, 0x00000000,
            0x00000000, 0xFF30D0FF, 0xFF10A0FF, 0xFF10A0FF, 0xFF10A0FF, 0xFF10A0FF, 0xFF10A0FF, 0xFF10A0FF, 0xFF10A0FF, 0xFF10A0FF, 0xFF10A0FF, 0xFF10A0FF, 0xFF10A0FF, 0xFF30D0FF, 0x00000000, 0x00000000,
            0x00000000, 0x00000000, 0xFF30D0FF, 0xFF10A0FF, 0xFF10A0FF, 0xFF10A0FF, 0xFF10A0FF, 0xFF10A0FF, 0xFF10A0FF, 0xFF10A0FF, 0xFF10A0FF, 0xFF10A0FF, 0xFF30D0FF, 0x00000000, 0x00000000, 0x00000000,
            0x00000000, 0x00000000, 0x00000000, 0xFF0070DD, 0xFF30D0FF, 0xFF10A0FF, 0xFF10A0FF, 0xFF10A0FF, 0xFF10A0FF, 0xFF10A0FF, 0xFF30D0FF, 0xFF0070DD, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
            0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0xFF0070DD, 0xFF0070DD, 0xFF0070DD, 0xFF0070DD, 0xFF0070DD, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000
        };
        
        for (auto pixel : data)
        {
            writer << pixel;
        }
    });

    renderer.EndUpload();

    for (int r = 0; r < 420; r++)
    {
        for (int c = 0; c < 420; c++)
        {
            Gfx::Transformations tm;
            tm.scale = 0.35;
            tm.x = -210 + r;
            tm.z = -210 + c;
            //tm.y = Math::Sin(((Math::pi * r) / 10.0) + ((Math::pi * c) / 10.0)) * 0.2;

            test_model->instances.push_back(tm);
        }
    }

    //renderer.SetupTestScene();
}

int APIENTRY wWinMain(
    [[maybe_unused]] HINSTANCE hInst,
    [[maybe_unused]] HINSTANCE hPrevInstance,
    [[maybe_unused]] PWSTR cmdline,
    [[maybe_unused]] int nShowCmd)
{
    TestApplication app({
        .name = "RcTech"
    });

    app.Initialize();
    app.Run();

    return 0;
}