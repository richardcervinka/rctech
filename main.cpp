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
        if (test_model != nullptr)
        {
            if (GetResourceManager().Complete(resource_timeline))
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
            for (int r = 0; r < 10; r++)
            {
                for (int c = 0; c < 10; c++)
                {
                    tm->instances[r * 10 + c].y = std::sin(step + ((Math::pi * r) / 10.0) + ((Math::pi * c) / 10.0)) * 0.2;
                    tm->instances[r * 10 + c].scale = 0.35 + std::sin(step + ((Math::pi * r) / 10.0) + ((Math::pi * c) / 10.0)) * 0.05;
                }
            }
        }


        Generic::Application::BeginFrame();
    }

    std::unique_ptr<TestModel> test_model;
    uint64_t resource_timeline {0};
};

void TestApplication::Initialize()
{
    Generic::Application::Initialize();

    //auto& renderer = GetRenderer();
    auto& rm = GetResourceManager();

    test_model = std::make_unique<TestModel>();

    rm.BeginUpload();

    test_model->vb_handle = rm.AllocateVertexBuffer(Render::ResourceFamily{0}, sizeof(Gfx::VertexBasic) * 8);

    resource_timeline = rm.Upload(test_model->vb_handle, [](Render::BufferWriter& writer) {
        // front-left-bottom
        writer << Float3{-1, -1,  1};
        writer << Float3{1, 0, 0};
        // front-right-top
        writer << Float3{1,  1,  1};
        writer << Float3{1, 0, 0};
        // front-left-top
        writer << Float3{-1,  1,  1};
        writer << Float3{1, 0, 0};
        // front-right-bottom
        writer << Float3{1, -1,  1};
        writer << Float3{1, 0, 0};
        // back-left-bottom
        writer << Float3{-1, -1, -1};
        writer << Float3{0, 1, 0};
        // back-right-top
        writer << Float3{1,  1, -1};
        writer << Float3{0, 1, 0};
        // back-left-top
        writer << Float3{-1,  1, -1};
        writer << Float3{0, 0, 1};
        // back-right-bottom
        writer << Float3{1, -1, -1};
        writer << Float3{0, 0, 1};
    });
    
    test_model->ib_handle = rm.AllocateIndexBuffer(Render::ResourceFamily{0}, sizeof(uint16_t) * 36);

    resource_timeline = rm.Upload(test_model->ib_handle, [](Render::BufferWriter& writer) {
        writer << std::array<uint16_t, 36>{
            // front
            0, 1, 2, 0, 3, 1,
            // back
            4, 6, 5, 4, 5, 7,
            // left
            4, 2, 6, 4, 0, 2,
            // right
            3, 5, 1, 3, 7, 5,
            // top
            2, 1, 5, 2, 5, 6,
            // bottom
            4, 3, 0, 4, 7, 3
        };
    });

    rm.EndUpload();
    //GetRenderer().transfer_timeline = rm.Transfer();

    for (int r = 0; r < 10; r++)
        {
            for (int c = 0; c < 10; c++)
            {
                Gfx::Transformations tm;
                tm.scale = 0.35;
                tm.x = -4.5 + r;
                tm.z = -4.5 + c;
                tm.y = std::sin(((Math::pi * r) / 10.0) + ((Math::pi * c) / 10.0)) * 0.2;

                test_model->instances.push_back(tm);
            }
        }
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