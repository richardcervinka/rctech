#pragma once

#include <memory>
#include <atomic>
#include <vector>
#include <filesystem>
#include "buffer_linear_allocator.h"
#include "render/device.h"
#include "texture_manager.h"

namespace Rc::Render
{
    //
    // Reading texture to the internal staging buffer.
    //
    class TextureBatch
    {
    public:
        explicit TextureBatch(std::unique_ptr<Buffer> buffer);

        BufferRegion Load(std::filesystem::path const& path);

        TextureInfo GetTextureInfo(BufferRegion const& region) const;

        void Reset()
        {
            allocator.Reset();
        }

        uint64_t Available() const
        {
            return allocator.Available();
        }

    private:
        BufferLinearAllocator allocator;
    };

    //
    // Texture transfer queue
    //
    class TextureTransfer
    {
    public:
        TextureTransfer(Device const& device);

        void Reset();

        // Create a load request.
        // Returns unuque identifier.
        void Transfer(BufferRegion const& src, std::span<TextureLayout const> layout, Texture2D& dst);

        void Submit();

        bool Complete() const
        {
            return semaphore->QueryCounter() >= semaphore->Value();
        }

        void Wait()
        {
            semaphore->Wait();
        }

    private:
        Device const& device;
        
        std::unique_ptr<TransferCommandQueue> queue;
        std::unique_ptr<TransferCommandBuffer> commands;
        std::unique_ptr<TimelineSemaphore> semaphore;
    };

    //
    // 1) Reset()
    // 2) Add()
    // 3) Laod()
    // 4) LoadComplete()
    // 5) Transfer()
    // 6) TransferComplete()
    //
    class TextureLoader
    {
    public:
        struct Request
        {
            TextureSlot slot;

            std::filesystem::path path;

            uint64_t file_size;
        };

        TextureLoader(Device const& device, uint64_t capacity);

        ~TextureLoader() = default;

        TextureLoader(TextureLoader const&) = delete;
        TextureLoader(TextureLoader&&) = delete;
        TextureLoader& operator=(TextureLoader const&) = delete;
        TextureLoader& operator=(TextureLoader&&) = delete;

        // TODO: uint64_t Capacity() const

        void Add(Request const& request);

        void Abort()
        {
            abort = true;
        }

        bool Complete() const
        {
            return state == State::Complete;
        }

        void Transfer()
        {
            state = State::Transfer;

            transfer.Submit();
        }

        bool Transfered() const
        {
            return transfer.Complete();
        }

        void Reset()
        {
            state = State::Ready;
            requests.clear();
        }

        // Load added requests to the target texture manager
        void Load(TextureManager& manager);

    private:
        enum class State
        {
            // Ready to call Add() and Load().
            Ready,

            // Loading in progress.
            Loading,

            // All requests loaded, ready to call Transfer()
            Complete,

            // Transfer in progerss.
            // Check transfering status using Transfered() method.
            Transfer
        };

        void Process(Request const& request, TextureManager& manager);

        Device const& device;
        TextureBatch batch;
        TextureTransfer transfer;

        std::vector<Request> requests;

        uint64_t reserved {0};

        // Signal to abort running loading.
        std::atomic<bool> abort {false};

        std::atomic<State> state {State::Ready};
    };
    
} // Rc::Render