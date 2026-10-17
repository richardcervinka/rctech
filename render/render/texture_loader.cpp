#include "texture_loader.h"
#include "core/ktx.h"
#include <fstream>

namespace Rc::Render
{
   // TextureBatch

    TextureBatch::TextureBatch(std::unique_ptr<Buffer> buffer)
        : allocator{std::move(buffer)}
    {}

    BufferRegion TextureBatch::Load(std::filesystem::path const& path)
    {
        // TODO: Check file ext

        auto const file_size = std::filesystem::file_size(path);

        if (file_size < sizeof(KtxHeader))
        {
            throw std::runtime_error("Bad file data");
        }

        std::ifstream file(path, std::ios::binary);

        if (!file)
        {
            throw std::runtime_error("Bad file path");
        }

        if (allocator.Available() < file_size)
        {
            throw std::runtime_error("Out of memory");
            //return std::unexpected{TextureBatch::Error::OutOfMemory};
        }

        auto region = allocator.Allocate(file_size);

        // Destination raw memory.
        auto raw = allocator.Map<std::byte>(region);

        // Read entire file to the staging buffer
        if (!file.read(reinterpret_cast<char*>(raw.data()), static_cast<std::streamsize>(file_size)))
        {
            throw std::runtime_error("Read file error");
        }

        return region;
    }

    TextureInfo TextureBatch::GetTextureInfo(BufferRegion const& region) const
    {
        return KtxReader(allocator.Map<std::byte const>(region)).Info();
    }

    // TextureTransferQueue

    TextureTransfer::TextureTransfer(Device const& device) : device{device}
    {
        queue = device.CreateTransferQueue();
        commands = queue->CreateCommandBuffer();
        semaphore = device.CreateTimelineSemaphore();
    }

    void TextureTransfer::Reset()
    {
        semaphore->Wait();
        commands->Reset();
        commands->Begin();
    }
    
    void TextureTransfer::Transfer(BufferRegion const& src, std::span<TextureLayout const> layout, Texture2D& dst)
    {
        commands->Texture2DBarrier(dst, ImageUsage::Undefined, ImageUsage::TransferWrite);
        
        commands->TransferTexture(src, layout, dst);

        commands->BarrierTexture2DRelease(
            dst,
            ImageUsage::TransferWrite,
            ImageUsage::SampledImage,
            device.TransferQueueFamilyIndex(),
            device.GraphicsQueueFamilyIndex()
        );
    }

    void TextureTransfer::Submit()
    {
        commands->End();
        semaphore->Increment();
        queue->Submit(*commands, *semaphore);
    }
    
    // TextureLoader

    TextureLoader::TextureLoader(Device const& device, uint64_t capacity) :
        device{device},
        batch{device.AllocateStagingBuffer(capacity)},
        transfer{device}
    {}

    void TextureLoader::Add(Request const& request)
    {
        assert(reserved + request.file_size <= batch.Available());

        requests.push_back(request);
        reserved += request.file_size;
    }

    void TextureLoader::Load(TextureManager& manager)
    {
        abort = false;
        state = State::Loading;

        transfer.Reset();

        for (auto const& request : requests)
        {
            if (abort)
            {
                return;
            }
            try
            {
                Process(request, manager);
            }
            catch (...)
            {
                // --------------------------------- out of memory, bad file size...
            }
        }

        state = State::Complete;

        // TODO: Po dokonceni Transfer:
        // for (auto const& request : requests)
        // {
        //     manager->ActivateTexture2D(request.slot);
        // }
    }

    void TextureLoader::Process(Request const& request, TextureManager& manager)
    {
        // Load entire texture to the staging buffer.
        auto const region = batch.Load(request.path);
        
        // Parse texture info.
        auto const info = batch.GetTextureInfo(region);

        // Create GPU texture object.
        auto texture = device.AllocateTexture2D(info.format, info.width, info.height, info.layout.size());

        // Create transfer command.
        transfer.Transfer(region, info.layout, *texture);

        manager.InsertTexture2D(std::move(texture), request.slot);
    }

} // Rc::Render