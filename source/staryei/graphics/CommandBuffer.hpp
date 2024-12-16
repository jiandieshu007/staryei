#pragma once

#include "graphics/GpuDevice.hpp"

namespace syi
{
	static constexpr uint32_t k_secondary_command_buffer_count = 2;


	struct CommandBuffer
	{
		void init(GpuDevice* gpu);
		void shutdown();

        //
// Commands interface
//

        DescriptorSetHandle             create_descriptor_set(const DescriptorSetCreationInfo& creation);

        void                            begin();
        void                            begin_secondary(RenderPass* current_render_pass, Framebuffer* current_framebuffer);
        void                            end();
        void                            end_current_render_pass();

        void                            bind_pass(RenderPassHandle handle, FramebufferHandle framebuffer, bool use_secondary);
        void                            bind_pipeline(PipelineHandle handle);
        void                            bind_vertex_buffer(BufferHandle handle, u32 binding, u32 offset);
        void                            bind_index_buffer(BufferHandle handle, u32 offset, VkIndexType index_type);
        void                            bind_descriptor_set(DescriptorSetHandle* handles, u32 num_lists, u32* offsets, u32 num_offsets);
        void                            bind_local_descriptor_set(DescriptorSetHandle* handles, u32 num_lists, u32* offsets, u32 num_offsets);

        void                            set_viewport(const Viewport* viewport);
        void                            set_scissor(const Rect2D* rect);

        void                            clear(f32 red, f32 green, f32 blue, f32 alpha);
        void                            clear_depth_stencil(f32 depth, u8 stencil);

        void                            draw(VkPolygonMode topology, u32 first_vertex, u32 vertex_count, u32 first_instance, u32 instance_count);
        void                            draw_indexed(VkPolygonMode topology, u32 index_count, u32 instance_count, u32 first_index, i32 vertex_offset, u32 first_instance);
        void                            draw_indirect(BufferHandle handle, u32 offset, u32 stride);
        void                            draw_indexed_indirect(BufferHandle handle, u32 offset, u32 stride);

        void                            dispatch(u32 group_x, u32 group_y, u32 group_z);
        void                            dispatch_indirect(BufferHandle handle, u32 offset);

        void                            barrier(const ExecutionBarrier& barrier);

        void                            fill_buffer(BufferHandle buffer, u32 offset, u32 size, u32 data);

        void                            push_marker(const std::string& name);
        void                            pop_marker();

        // Non-drawing methods
        void                            upload_texture_data(TextureHandle texture, void* texture_data, BufferHandle staging_buffer, sizet staging_buffer_offset);
        void                            copy_texture(TextureHandle src_, ResourceState src_state, TextureHandle dst_, ResourceState dst_state);

        void                            upload_buffer_data(BufferHandle buffer, void* buffer_data, BufferHandle staging_buffer, sizet staging_buffer_offset);
        void                            upload_buffer_data(BufferHandle src, BufferHandle dst);

        void                            reset();

        VkCommandBuffer                 vk_command_buffer;

        VkDescriptorPool                vk_descriptor_pool;
        ResourcePool                    descriptor_sets;

        GpuDevice* device;

        std::array<VkDescriptorSet,16>                 vk_descriptor_sets;

        RenderPass* current_render_pass;
        Framebuffer* current_framebuffer;
        Pipeline* current_pipeline;
        std::array<VkClearValue,2>                    clears;          // 0 = color, 1 = depth stencil
        bool                            is_recording;

        u32                             handle;

        u32                             current_command;
        ResourceHandle                  resource_handle;
	};
}
