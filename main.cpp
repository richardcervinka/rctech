#include "generic/application.h"
#include "base/float.h"
#include "base/image.h"
#include "render/development.h"

using namespace Rc;

class TestApplication : public Generic::Application
{
public:
    using Generic::Application::Application;

    void Initialize() override;

    void BeginFrame() override
    {
        Generic::Application::BeginFrame();
        
        if (!Rc::Dev::initialized)
        {
            Rc::Dev::initialized = GetRenderer().QueryUpload(Rc::Dev::resource_timeline);
        }

        static double step = 0;
        step += 0.020;

        // Test animation
        if (Rc::Dev::initialized)
        {
            int const size = 420;
            double const scale = 10.0;

            for (int r = 0; r < size; r++)
            {
                for (int c = 0; c < size; c++)
                {
                    auto sin = Math::Sin(step + ((Math::pi * r) / scale) + ((Math::pi * c) / scale));
                    Rc::Dev::instances[r * size + c].y = sin * 0.3;
                    Rc::Dev::instances[r * size + c].scale = 0.35 + sin * 0.03;
                    //tm->instances[r * size + c].yaw = step;
                }
            }
        }
    }
};

void TestApplication::Initialize()
{
    Generic::Application::Initialize();

    auto& renderer = GetRenderer();

    //auto& renderer = GetRenderer();
    //auto& rm = GetResourceManager();

    Rc::Dev::initialized = false;


    renderer.BeginUpload();

    Rc::Dev::vb_handle = renderer.AllocateVertexBuffer(Render::ResourceFamily{0}, sizeof(Gfx::VertexBasic) * 24);

    Rc::Dev::resource_timeline = renderer.Upload(Rc::Dev::vb_handle, [](Render::BufferWriter& writer) {
        // Front
        writer << Float3{-1, -1,  1} << Float2{0, 1};
        writer << Float3{ 1,  1,  1} << Float2{1, 0};
        writer << Float3{-1,  1,  1} << Float2{0, 0};
        writer << Float3{ 1, -1,  1} << Float2{1, 1};
        // Baack
        writer << Float3{-1, -1, -1} << Float2{0, 1};
        writer << Float3{ 1,  1, -1} << Float2{1, 0};
        writer << Float3{-1,  1, -1} << Float2{0, 0};
        writer << Float3{ 1, -1, -1} << Float2{1, 1};
        // Left
        writer << Float3{-1, -1, -1} << Float2{0, 1};
        writer << Float3{-1,  1,  1} << Float2{1, 0};
        writer << Float3{-1,  1, -1} << Float2{0, 0};
        writer << Float3{-1, -1,  1} << Float2{1, 1};
        // Right
        writer << Float3{ 1, -1,  1} << Float2{0, 1};
        writer << Float3{ 1,  1, -1} << Float2{1, 0};
        writer << Float3{ 1,  1,  1} << Float2{0, 0};
        writer << Float3{ 1, -1, -1} << Float2{1, 1};
        // Top
        writer << Float3{-1,  1,  1} << Float2{0, 1};
        writer << Float3{ 1,  1, -1} << Float2{1, 0};
        writer << Float3{-1,  1, -1} << Float2{0, 0};
        writer << Float3{ 1,  1,  1} << Float2{1, 1};
        // Bottom
        writer << Float3{-1, -1, -1} << Float2{0, 1};
        writer << Float3{ 1, -1,  1} << Float2{1, 0};
        writer << Float3{-1, -1,  1} << Float2{0, 0};
        writer << Float3{ 1, -1, -1} << Float2{1, 1};
    });
    
    Rc::Dev::ib_handle = renderer.AllocateIndexBuffer(Render::ResourceFamily{0}, sizeof(uint16_t) * 36);

    Rc::Dev::resource_timeline = renderer.Upload(Rc::Dev::ib_handle, [](Render::BufferWriter& writer) {
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

    //Rc::Render::TextureBuffer tb(

    renderer.EndUpload();

    for (int r = 0; r < 420; r++)
    {
        for (int c = 0; c < 420; c++)
        {
            Gfx::Transformations tm;
            tm.scale = 0.35;
            tm.x = -210 + r;
            tm.z = -210 + c;
            Rc::Dev::instances.push_back(tm);
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