#pragma once

#include "vulkan/device.h"
#include "base/color.h"
#include "texture.h"
#include <array>

namespace Rc::Render
{
    enum class RenderTargetSlot
    {
        FrameBuffer = 0
    };

    class RenderTargetAttachments
    {
    public:
        static constexpr int slots_count = 4;

        static constexpr std::array<VkFormat, slots_count> slots_format {
            // RenderTargetSlot::FrameBuffer
            VK_FORMAT_R8G8B8A8_SRGB,
            //
            VK_FORMAT_UNDEFINED,
            //
            VK_FORMAT_UNDEFINED,
            //
            VK_FORMAT_UNDEFINED
        };

        RenderTargetAttachments()
        {
            for (auto& attachment : m_color_attachments)
            {
                attachment = {
                    .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
                    .pNext = nullptr,
                    .imageView = VK_NULL_HANDLE,
                    .imageLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                    .resolveMode = VK_RESOLVE_MODE_NONE,
                    .resolveImageView = VK_NULL_HANDLE,
                    .resolveImageLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                    .loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                    .storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
                    .clearValue = {}
                };
            }
        }

        ~RenderTargetAttachments() = default;

        RenderTargetAttachments(RenderTargetAttachments const&) = delete;
        RenderTargetAttachments(RenderTargetAttachments&&) = delete;
        RenderTargetAttachments& operator=(RenderTargetAttachments const&) = delete;
        RenderTargetAttachments& operator=(RenderTargetAttachments&&) = delete;

        void EnableColorAttachment(RenderTargetSlot slot, RenderTargetView const& render_target)
        {
            m_color_attachments[std::to_underlying(slot)] = {
                .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
                .pNext = nullptr,
                .imageView = render_target.View(),
                .imageLayout = render_target.Layout(),
                .resolveMode = VK_RESOLVE_MODE_NONE,
                .resolveImageView = VK_NULL_HANDLE,
                .resolveImageLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                .loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                .storeOp = VK_ATTACHMENT_STORE_OP_STORE
            };
        }

        void ClearRenderTarget(RenderTargetSlot slot, Color const& color)
        {
            auto& attachment = m_color_attachments[std::to_underlying(slot)];

            attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
            attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
            attachment.clearValue.color.float32[0] = color.r;
            attachment.clearValue.color.float32[1] = color.g;
            attachment.clearValue.color.float32[2] = color.b;
            attachment.clearValue.color.float32[3] = color.a;
        }

        void LoadRenderTarget(RenderTargetSlot slot)
        {
            m_color_attachments[std::to_underlying(slot)].loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
        }

        void DisableColorAttachment(RenderTargetSlot slot)
        {
            m_color_attachments[std::to_underlying(slot)] = {
                .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
                .pNext = nullptr,
                .imageView = VK_NULL_HANDLE,
                .imageLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                .resolveMode = VK_RESOLVE_MODE_NONE,
                .resolveImageView = VK_NULL_HANDLE,
                .resolveImageLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                .loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                .storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
                .clearValue = {}
            };
        }

        std::span<VkRenderingAttachmentInfo const, slots_count> ColorAttachmentsInfo() const
        {
            return m_color_attachments;
        }

    private:
        // Dynamic rendering color attachments (outputs)
        std::array<VkRenderingAttachmentInfo, slots_count> m_color_attachments {};
        
        std::optional<VkRenderingAttachmentInfo> m_depth_attachment;
    };

    
} // Rc::Render