#pragma once

#include <array>
#include <cassert>
#include <memory>
#include "texture.h"

namespace Rc::Render
{
    using TextureSlot = uint32_t;

    //
    // Map public texture slot to the Vulkan objects
    //
    class TextureManager
    {
    public:
        static inline const uint32_t slot_count = UINT16_MAX;

        void InsertTexture2D(std::unique_ptr<Texture2D> texture, TextureSlot slot/*, uint32_t index*/)
        {
            assert(slot < slot_count);
            assert(slots[slot].texture == nullptr);

            slots[slot].texture = std::move(texture);
            slots[slot].state = TextureState::Created;
            //slots[slot].index = index;
        }

        void ReleaseTexture2D(TextureSlot slot)
        {
            assert(slot < slot_count);

            slots[slot] = {};
        }

        void ActivateTexture2D(TextureSlot slot)
        {
            assert(slot < slot_count);

            slots[slot].state = TextureState::Ready;
        }

        Texture2D const& GetTexture2D(TextureSlot slot) const
        {
            return *slots[slot].texture;
        }

        // uint32_t GetIndex(TextureSlot slot) const
        // {
        //     return slots[slot].index;
        // }

    private:
        enum class TextureState
        {
            Null,
            Created,
            Ready
        };

        struct Slot
        {
            std::unique_ptr<Texture2D> texture;

            TextureState state {TextureState::Null};

            // Descriptor heap index
            //uint32_t index {0};

            // Optional
            // std::string name;
        };

        std::array<Slot, slot_count> slots;
    };

} // Rc::Render