#include "graphics/CommandBuffer.hpp"
#include "graphics/GpuDevice.hpp"

#include "foundation/memory.hpp"

#include "external/tracy/tracy/Tracy.hpp"


namespace syi
{
	static constexpr u32 k_global_pool_elements = 128;

	void CommandBuffer::init(GpuDevice* gpu)
	{
		device = gpu;

		//create descriptor pools
		VkDescriptorPoolSize pool_sizes[] =
		{
			{ VK_DESCRIPTOR_TYPE_SAMPLER, k_global_pool_elements },
			{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, k_global_pool_elements },
			{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, k_global_pool_elements },
			{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, k_global_pool_elements },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, k_global_pool_elements },
			{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, k_global_pool_elements },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, k_global_pool_elements },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, k_global_pool_elements },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, k_global_pool_elements },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, k_global_pool_elements },
			{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, k_global_pool_elements}
		};
		VkDescriptorPoolCreateInfo poolCI{};
		poolCI.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolCI.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
		poolCI.maxSets = k_descriptor_sets_pool_size;
		poolCI.poolSizeCount = static_cast<uint32_t>(ARRAYSIZE(pool_sizes));
		poolCI.pPoolSizes = pool_sizes;
		RASSERT(vkCreateDescriptorPool(device->vulkan_device, &poolCI, device->vulkan_allocation_callbacks, &vk_descriptor_pool) == VK_SUCCESS);

		descriptor_sets.init(device->allocator, k_descriptor_sets_pool_size, sizeof(DescriptorSet));

		reset();
	}

	void CommandBuffer::shutdown()
	{
		is_recording = false;

		reset();
		descriptor_sets.shutdown();

		vkDestroyDescriptorPool(device->vulkan_device, vk_descriptor_pool, device->vulkan_allocation_callbacks);
	}

	DescriptorSetHandle CommandBuffer::create_descriptor_set(const DescriptorSetCreationInfo& creation)
	{
		DescriptorSetHandle handle = { descriptor_sets.obtain_resource() };
		if (handle.index == k_invalid_index) {
			return handle;
		}

		DescriptorSet* descriptor_set = (DescriptorSet*)descriptor_sets.access_resource(handle.index);
		const DescriptorSetLayout* descriptor_set_layout = device->access_descriptor_set_layout(creation.layout);
		VkDescriptorSetAllocateInfo alloc_info{ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO };
		alloc_info.descriptorPool = vk_descriptor_pool;
		alloc_info.descriptorSetCount = 1;
		alloc_info.pSetLayouts = &descriptor_set_layout->vk_descriptor_set_layout;
		RASSERT(vkAllocateDescriptorSets(device->vulkan_device, &alloc_info, &descriptor_set->vk_descriptor_set) == VK_SUCCESS);

		// Cache data
		u8* memory = rallocam((sizeof(ResourceHandle) + sizeof(SamplerHandle) + sizeof(u16)) * creation.num_resources, device->allocator);
		descriptor_set->resources = (ResourceHandle*)memory;
		descriptor_set->samplers = (SamplerHandle*)(memory + sizeof(ResourceHandle) * creation.num_resources);
		descriptor_set->bindings = (u16*)(memory + (sizeof(ResourceHandle) + sizeof(SamplerHandle)) * creation.num_resources);
		descriptor_set->num_resources = creation.num_resources;
		descriptor_set->layout = descriptor_set_layout;

		// when eight is not enough, how to fix it ?
		std::array<VkWriteDescriptorSet, 8> descriptor;
		std::array<VkDescriptorBufferInfo, 8> buffer_info;
		std::array<VkDescriptorImageInfo, 8> image_info;

		Sampler* vk_default_sampler = device->access_sampler(device->default_sampler);

		auto num_resources = creation.num_resources;
		GpuDevice::fill_write_descriptor_sets(*device, descriptor_set_layout, descriptor_set->vk_descriptor_set,
			descriptor.data(), buffer_info.data(), image_info.data(), vk_default_sampler->vk_sampler,
			num_resources, creation.resources.data(), creation.samplers.data(), creation.bindings.data());

		// Cache resources
		for (u32 r = 0; r < creation.num_resources; r++) {
			descriptor_set->resources[r] = creation.resources[r];
			descriptor_set->samplers[r] = creation.samplers[r];
			descriptor_set->bindings[r] = creation.bindings[r];
		}

		vkUpdateDescriptorSets(device->vulkan_device, num_resources, descriptor.data(), 0, nullptr);

		return handle;

	}

	void CommandBuffer::begin()
	{
		if( !is_recording )
		{
			VkCommandBufferBeginInfo begin = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
			begin.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
			vkBeginCommandBuffer(vk_command_buffer, &begin);

			is_recording = true;
		}
	}

	// here still unknow secondary command buffer usage 
	void CommandBuffer::begin_secondary(RenderPass* current_render_pass, Framebuffer* current_framebuffer)
	{
		if (!is_recording) {
			VkCommandBufferInheritanceInfo inheritance{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO };
			inheritance.renderPass = current_render_pass->vk_render_pass;
			inheritance.subpass = 0;
			inheritance.framebuffer = current_framebuffer->vk_framebuffer;

			VkCommandBufferBeginInfo beginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
			beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT | VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;
			beginInfo.pInheritanceInfo = &inheritance;

			vkBeginCommandBuffer(vk_command_buffer, &beginInfo);

			is_recording = true;

			current_render_pass = current_render_pass;
		}
	}

	void CommandBuffer::end()
	{
		if (is_recording) {
			vkEndCommandBuffer(vk_command_buffer);

			is_recording = false;
		}
	}

	void CommandBuffer::end_current_render_pass()
	{
		if (is_recording && current_render_pass != nullptr) {
			if (device->dynamic_rendering_extension_present) {
				device->cmd_end_rendering(vk_command_buffer);
			}
			else {
				vkCmdEndRenderPass(vk_command_buffer);
			}

			current_render_pass = nullptr;
		}
	}

	void CommandBuffer::bind_pass(RenderPassHandle handle, FramebufferHandle framebuffer, bool use_secondary)
	{
		is_recording = true;
		auto renderPass = device->access_render_pass(handle);
		if( current_render_pass && (renderPass != current_render_pass))
		{
			end_current_render_pass();
		}

		auto frameBuffer = device->access_framebuffer(framebuffer);

		if( renderPass != current_render_pass)
		{
			if( device->dynamic_rendering_extension_present)
			{
				Array<VkRenderingAttachmentInfoKHR> colorAttachmentInfo;
				colorAttachmentInfo.init(device->allocator, frameBuffer->num_color_attachments, frameBuffer->num_color_attachments);
				memset(colorAttachmentInfo.data, 0, sizeof(VkRenderingAttachmentInfoKHR) * frameBuffer->num_color_attachments);

				for(u32 a = 0; a < frameBuffer->num_color_attachments; ++a)
				{
					auto texture = device->access_texture(frameBuffer->color_attachments[a]);

					auto& colorAttahInfo = colorAttachmentInfo[a];
					colorAttahInfo.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
					colorAttahInfo.imageView = texture->vk_image_view;
					colorAttahInfo.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
					colorAttahInfo.resolveMode = VK_RESOLVE_MODE_NONE;
					colorAttahInfo.loadOp = renderPass->output.color_operations[a];
					colorAttahInfo.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
					colorAttahInfo.clearValue = renderPass->output.color_operations[a] == VK_ATTACHMENT_LOAD_OP_CLEAR ? clears[0] : VkClearValue{};
				}

				VkRenderingAttachmentInfoKHR depthAttachmentInfo{ VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR };

				auto hasDepth = frameBuffer->depth_stencil_attachment.index != k_invalid_index;
				if( hasDepth )
				{
					auto texture = device->access_texture(frameBuffer->depth_stencil_attachment);

					depthAttachmentInfo.imageView = texture->vk_image_view;
					depthAttachmentInfo.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
					depthAttachmentInfo.resolveMode = VK_RESOLVE_MODE_NONE;
					depthAttachmentInfo.loadOp = renderPass->output.depth_operation;
					depthAttachmentInfo.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
					depthAttachmentInfo.clearValue = renderPass->output.depth_operation == VK_ATTACHMENT_LOAD_OP_CLEAR  ? clears[1] : VkClearValue{ };

				}

				VkRenderingInfoKHR renderingInfo{ VK_STRUCTURE_TYPE_RENDERING_INFO_KHR };
				renderingInfo.flags = use_secondary ? VK_RENDERING_CONTENTS_SECONDARY_COMMAND_BUFFERS_BIT_KHR : 0;
				renderingInfo.renderArea = { 0,0,frameBuffer->width,frameBuffer->height };
				renderingInfo.layerCount = 1;
				renderingInfo.viewMask = 0;
				renderingInfo.colorAttachmentCount = frameBuffer->num_color_attachments;
				renderingInfo.pColorAttachments = frameBuffer->num_color_attachments > 0 ? colorAttachmentInfo.data : nullptr;
				renderingInfo.pDepthAttachment = hasDepth ? &depthAttachmentInfo : nullptr;
				renderingInfo.pStencilAttachment = nullptr;

				device->cmd_begin_rendering(vk_command_buffer, &renderingInfo);

				colorAttachmentInfo.shutdown();
			}else
			{
				VkRenderPassBeginInfo renderPassBegin{ VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO };
				renderPassBegin.framebuffer = frameBuffer->vk_framebuffer;
				renderPassBegin.renderPass = renderPass->vk_render_pass;

				renderPassBegin.renderArea = { {0,0},{frameBuffer->width,frameBuffer->height} };
				std::vector<VkClearValue> clearvalue;
				clearvalue.reserve(k_max_image_outputs + 1);

				for(u32 o = 0; o<renderPass->output.num_color_formats; ++o)
				{
					if(renderPass->output.color_operations[o] == VK_ATTACHMENT_LOAD_OP_CLEAR )
					{
						clearvalue.push_back(clears[0]);
					}
				}
				if( renderPass->output.depth_stencil_format != VK_FORMAT_UNDEFINED)
				{
					if( renderPass->output.depth_operation == VK_ATTACHMENT_LOAD_OP_CLEAR)
					{
						clearvalue.push_back(clears[1]);
					}
				}
				renderPassBegin.clearValueCount = clearvalue.size();
				renderPassBegin.pClearValues = clearvalue.data();

				vkCmdBeginRenderPass(vk_command_buffer, &renderPassBegin, use_secondary ? VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS : VK_SUBPASS_CONTENTS_INLINE);
			}
		}

		// Cache render pass
		current_render_pass = renderPass;
		current_framebuffer = frameBuffer;
	}

	void CommandBuffer::bind_pipeline(PipelineHandle handle)
	{
		auto pipeline = device->access_pipeline(handle);

		vkCmdBindPipeline(vk_command_buffer, pipeline->vk_bind_point, pipeline->vk_pipeline);

		current_pipeline = pipeline;
	}

	void CommandBuffer::bind_vertex_buffer(BufferHandle handle, uint32_t binding, uint32_t offset)
	{
		auto buffer = device->access_buffer(handle);
		VkDeviceSize offsets[] = { offset };

		auto vkBuffer = buffer->vk_buffer;

		if( buffer->parent_buffer.index != k_invalid_index)
		{
			buffer = device->access_buffer(buffer->parent_buffer);
			vkBuffer = buffer->vk_buffer;
			offsets[0] = buffer->global_offset;
		}
		vkCmdBindVertexBuffers(vk_command_buffer, binding, 1, &vkBuffer, offsets);
	}

	void CommandBuffer::bind_index_buffer(BufferHandle handle, uint32_t offset, VkIndexType index_type)
	{
		auto buffer = device->access_buffer(handle);


		auto vkBuffer = buffer->vk_buffer;
		if( buffer->parent_buffer.index != k_invalid_index)
		{
			buffer = device->access_buffer(buffer->parent_buffer);
			vkBuffer = buffer->vk_buffer;
			offset = buffer->global_offset;
		}
		vkCmdBindIndexBuffer(vk_command_buffer, vkBuffer, offset, index_type);
	}

	void CommandBuffer::bind_descriptor_set(DescriptorSetHandle* handles, uint32_t num_lists, uint32_t* offsets,
		uint32_t num_offsets)
	{
		std::vector<uint32_t> offsets;

		for(u32 l=0; l<num_lists; ++l)
		{
			auto descriptorSet = device->access_descriptor_set(handles[l]);
			vk_descriptor_sets[l] = descriptorSet->vk_descriptor_set;

			const auto descriptorSetLayout = descriptorSet->layout;
			for( u32 i=0; i<descriptorSetLayout->num_bindings; ++i)
			{
				const auto& rb = descriptorSetLayout->bindings[i];
				if(rb.type == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER)
				{
					const u32 resourceIndex = descriptorSet->bindings[i];
					auto bufferHandle = descriptorSet->resources[resourceIndex];
					auto buffer = device->access_buffer({ bufferHandle });

					offsets.push_back(buffer->global_offset);
				}
			}
		}
		
		vkCmdBindDescriptorSets(vk_command_buffer, current_pipeline->vk_bind_point, current_pipeline->vk_pipeline_layout, 1, num_lists,
			vk_descriptor_sets.data(), offsets.size(), offsets.data());
		if( device->bindless_supported)
		{
			vkCmdBindDescriptorSets(vk_command_buffer, current_pipeline->vk_bind_point, current_pipeline->vk_pipeline_layout, 0, 1,
				&device->vulkan_bindless_descriptor_set_cached, 0, nullptr);
		}
	}

	void CommandBuffer::bind_local_descriptor_set(DescriptorSetHandle* handles, uint32_t num_lists, uint32_t* offsets,
		uint32_t num_offsets)
	{
		// TODO:
		std::vector<uint32_t> offsets;

		for (u32 l = 0; l < num_lists; ++l) {
			DescriptorSet* descriptor_set = (DescriptorSet*)descriptor_sets.access_resource(handles[l].index);
			vk_descriptor_sets[l] = descriptor_set->vk_descriptor_set;

			// Search for dynamic buffers
			const DescriptorSetLayout* descriptor_set_layout = descriptor_set->layout;
			for (u32 i = 0; i < descriptor_set_layout->num_bindings; ++i) {
				const DescriptorBinding& rb = descriptor_set_layout->bindings[i];

				if (rb.type == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER) {
					// Search for the actual buffer offset
					const u32 resource_index = descriptor_set->bindings[i];
					ResourceHandle buffer_handle = descriptor_set->resources[resource_index];
					Buffer* buffer = device->access_buffer({ buffer_handle });

					offsets.push_back(buffer->global_offset);
				}
			}
		}


		vkCmdBindDescriptorSets(vk_command_buffer, current_pipeline->vk_bind_point, current_pipeline->vk_pipeline_layout, 1,
			num_lists, vk_descriptor_sets.data(), offsets.size(), offsets.data());

		if (device->bindless_supported) {
			vkCmdBindDescriptorSets(vk_command_buffer, current_pipeline->vk_bind_point, current_pipeline->vk_pipeline_layout, 0,
				1, &device->vulkan_bindless_descriptor_set_cached, 0, nullptr);
		}
	}

	void CommandBuffer::set_viewport(const Viewport* viewport)
	{
		VkViewport vkViewport;
		if( viewport )
		{
			vkViewport.x = viewport->x;
			// Invert Y with negative height and proper offset - Vulkan has unique Clipping Y.
			vkViewport.y = viewport->height - viewport->y;
			vkViewport.width = viewport->width;
			vkViewport.height = -viewport->height;
			vkViewport.minDepth = viewport->minDepth;
			vkViewport.maxDepth = viewport->maxDepth;
		}else
		{
			vkViewport.x = 0.f;

			if( current_render_pass )
			{
				vkViewport.width = current_framebuffer->width * 1.f;
				// Invert Y with negative height and proper offset - Vulkan has unique Clipping Y.
				vkViewport.y = current_framebuffer->height * 1.f;
				vkViewport.height = -current_framebuffer->height * 1.f;
			}else{
				vkViewport.width = device->swapchain_width * 1.f;
				// Invert Y with negative height and proper offset - Vulkan has unique Clipping Y.
				vkViewport.y = device->swapchain_height * 1.f;
				vkViewport.height = -device->swapchain_height * 1.f;
			}
			vkViewport.minDepth = 0.f;
			vkViewport.maxDepth = 1.f;
		}
		vkCmdSetViewport(vk_command_buffer, 0, 1, &vkViewport);
	}

	void CommandBuffer::set_scissor(const Rect2D* rect)
	{

		if( rect )
		{
			VkRect2D scissor = { {rect->offest.x,rect->offest.y},{rect->extent.width,rect->extent.height} };
			vkCmdSetScissor(vk_command_buffer, 0, 1, &scissor);
		}else
		{
			VkRect2D scissor = { {0,0},{device->swapchain_width,device->swapchain_height} };
			vkCmdSetScissor(vk_command_buffer, 0, 1, &scissor);
		}
	}

	void CommandBuffer::clear(float red, float green, float blue, float alpha)
	{
		clears[0] = { red,green,blue,alpha };
	}

	void CommandBuffer::clear_depth_stencil(float depth, uint8_t stencil)
	{
		clears[1].depthStencil.depth = depth;
		clears[1].depthStencil.stencil = stencil;
	}

	void CommandBuffer::draw(VkPolygonMode topology, uint32_t first_vertex, uint32_t vertex_count,
		uint32_t first_instance, uint32_t instance_count)
	{
	}

	void CommandBuffer::draw_indexed(VkPolygonMode topology, uint32_t index_count, uint32_t instance_count,
		uint32_t first_index, i32 vertex_offset, uint32_t first_instance)
	{
	}

	void CommandBuffer::draw_indirect(BufferHandle handle, uint32_t offset, uint32_t stride)
	{
	}

	void CommandBuffer::draw_indexed_indirect(BufferHandle handle, uint32_t offset, uint32_t stride)
	{
	}

	void CommandBuffer::dispatch(uint32_t group_x, uint32_t group_y, uint32_t group_z)
	{
	}

	void CommandBuffer::dispatch_indirect(BufferHandle handle, uint32_t offset)
	{
	}

	void CommandBuffer::barrier(const ExecutionBarrier& barrier)
	{
	}

	void CommandBuffer::fill_buffer(BufferHandle buffer, uint32_t offset, uint32_t size, uint32_t data)
	{
	}

	void CommandBuffer::push_marker(const std::string& name)
	{
	}

	void CommandBuffer::pop_marker()
	{
	}

	void CommandBuffer::upload_texture_data(TextureHandle texture, void* texture_data, BufferHandle staging_buffer,
		sizet staging_buffer_offset)
	{
	}

	void CommandBuffer::copy_texture(TextureHandle src_, ResourceState src_state, TextureHandle dst_,
		ResourceState dst_state)
	{
	}

	void CommandBuffer::upload_buffer_data(BufferHandle buffer, void* buffer_data, BufferHandle staging_buffer,
		sizet staging_buffer_offset)
	{
	}

	void CommandBuffer::upload_buffer_data(BufferHandle src, BufferHandle dst)
	{
	}

	void CommandBuffer::reset()
	{
		is_recording = false;
		current_command = 0;
		current_render_pass = nullptr;
		current_framebuffer = nullptr;
		current_pipeline = nullptr;

		vkResetDescriptorPool(device->vulkan_device, vk_descriptor_pool, 0);

		auto resourceCount = descriptor_sets.free_indices_head;
		for(u32 i=0; i<resourceCount; ++i)
		{
			DescriptorSet* set = static_cast<DescriptorSet*>(descriptor_sets.access_resource(i));
			if(set )
			{
				rfree(set->resources, device->allocator);
			}
			descriptor_sets.release_resource(i);
		}
	}
}
