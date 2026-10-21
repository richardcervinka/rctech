#pragma once

#include <array>
#include <cassert>
#include <memory>
#include "texture.h"

namespace Rc::Render
{
    using TextureSlot = uint32_t;

    // enum class TextureState
    // {
    //     Null,
    //     Created,
    //     Ready
    // };

    //
    // Map public logical texture slot to the Vulkan objects
    //
    class TextureManager
    {
    public:
        static inline const uint32_t slot_count = UINT16_MAX;

        void Insert(std::unique_ptr<Texture2D> texture, TextureSlot slot, uint32_t index)
        {
            assert(slots[slot].texture == nullptr);

            slots[slot].texture = std::move(texture);
            //slots[slot].state = TextureState::Created;
            slots[slot].index = index;
        }

        void Erase(TextureSlot slot)
        {
            slots[slot] = {};
        }

        // void ActivateTexture2D(TextureSlot slot) // ----------------- + descriptor heap index ?
        // {
        //     slots[slot].state = TextureState::Ready;
        // }

        Texture2D const& GetTexture2D(TextureSlot slot) const
        {
            return *slots[slot].texture;
        }

        // Get descriptor heap index
        uint32_t GetIndex(TextureSlot slot) const
        {
            return slots[slot].index;
        }

        // Set descriptor heap index
        void SetIndex(TextureSlot slot, uint32_t index)
        {
            slots[slot].index = index;
        }

        uint32_t Placeholder() const
        {
            return placeholder;
        }

        void SetPlaceholder(uint32_t value)
        {
            placeholder = value;
        }

    private:
        struct Slot
        {
            std::unique_ptr<Texture2D> texture;

            // TextureState state {TextureState::Null};

            // Descriptor heap index.
            uint32_t index {0};

            // Optional
            // std::string name;
        };

        std::array<Slot, slot_count> slots {};

        uint32_t placeholder {0};
    };

} // Rc::Render