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
    
    test_model->in_handle = rm.AllocateInstanceBuffer(Render::ResourceFamily{0}, 2 * sizeof(Gfx::VertexInstance));
    
    resource_timeline = rm.Upload(test_model->in_handle, [](Render::BufferWriter& writer) {
        Gfx::Transformations tm;
        tm.yaw = Math::pi + (Math::pi / 4.0);
        tm.scale = 1.0;

        writer << tm.Local().To<float>();
        writer << tm.World().To<float>();

        tm.yaw = Math::pi;
        tm.scale = 0.9;
        tm.z = -2;
        tm.y = 1;
        
        writer << tm.Local().To<float>();
        writer << tm.World().To<float>();
    });

    rm.EndUpload();
    //GetRenderer().transfer_timeline = rm.Transfer();
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