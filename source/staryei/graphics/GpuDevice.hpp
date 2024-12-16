#pragma once

#if defined(_MSC_VER)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#define VK_USE_PLATFORM_WIN32_KHR
#else
#define VK_USE_PLATFORM_XLIB_KHR
#endif
#include <vulkan/vulkan.h>

#include "external/vk_mem_alloc.h"

#include "graphics/GpuResource.hpp"

#include "foundation/data_structures.hpp"
#include "foundation/service.hpp"
#include "foundation/string.hpp"
#include "foundation/array.hpp"

namespace syi
{


	struct Allocator;
	struct StackAllocator;

	// Forward-declarations
	struct CommandBuffer;
	struct CommandBufferManager;
	struct DeivceRenderFrame;
	struct GpuDevice;

	struct DeviceCreationInfo
	{
		Allocator*			allocator = nullptr;
		StackAllocator*		temp_allocator = nullptr;
		void*				window = nullptr;

		uint16_t			width = 1,
							height = 1,
							gpu_time_queies_pre_frame = 32;
		uint16_t			num_thread = 1;
		bool				enable_gpu_time_queries = false;
		bool				debug = true;

		DeviceCreationInfo&		set_window(uint32_t width, uint32_t height, void* handle)
		{
			this->width = width;
			this->height = height;
			this->window = handle;
			return *this;
		};
		DeviceCreationInfo&		set_allocator(Allocator* allocator)
		{
			this->allocator = allocator;

			return *this;
		};
		DeviceCreationInfo&		set_linear_allocator(StackAllocator* allocator)
		{
			temp_allocator = allocator;

			return *this;
		};
		DeviceCreationInfo&		set_num_threads(uint32_t value)
		{
			num_thread = value;

			return *this;
		};
	};

	struct GpuDevice : public Service
	{
		static GpuDevice* instance();

		// Helper methods
		static void                     fill_write_descriptor_sets(GpuDevice& gpu, const DescriptorSetLayout* descriptor_set_layout, VkDescriptorSet vk_descriptor_set,
			VkWriteDescriptorSet* descriptor_write, VkDescriptorBufferInfo* buffer_info, VkDescriptorImageInfo* image_info,
			VkSampler vk_default_sampler, uint32_t& num_resources, const ResourceHandle* resources, const SamplerHandle* samplers, const uint16_t* bindings);

		// Init/Terminate methods
		void                            init(const DeviceCreationInfo& CreationInfo);
		void                            shutdown();

		// CreationInfo/Destruction of resources /////////////////////////////////
		BufferHandle                    create_buffer(const BufferCreationInfo& CreationInfo);
		TextureHandle                   create_texture(const TextureCreationInfo& CreationInfo);
		PipelineHandle                  create_pipeline(const PipelineCreationInfo& CreationInfo, const char* cache_path = nullptr);
		SamplerHandle                   create_sampler(const SamplerCreationInfo& CreationInfo);
		DescriptorSetLayoutHandle       create_descriptor_set_layout(const DescriptorSetLayoutCreationInfo& CreationInfo);
		DescriptorSetHandle             create_descriptor_set(const DescriptorSetCreationInfo& CreationInfo);
		RenderPassHandle                create_render_pass(const RenderPassCreationInfo& CreationInfo);
		FramebufferHandle               create_framebuffer(const FramebufferCreationInfo& CreationInfo);
		ShaderstateHandle               create_shader_state(const ShaderStateCreationInfo& CreationInfo);

		void                            destroy_buffer(BufferHandle buffer);
		void                            destroy_texture(TextureHandle texture);
		void                            destroy_pipeline(PipelineHandle pipeline);
		void                            destroy_sampler(SamplerHandle sampler);
		void                            destroy_descriptor_set_layout(DescriptorSetLayoutHandle layout);
		void                            destroy_descriptor_set(DescriptorSetHandle set);
		void                            destroy_render_pass(RenderPassHandle render_pass);
		void                            destroy_framebuffer(FramebufferHandle framebuffer);
		void                            destroy_shader_state(ShaderstateHandle shader);

		// Query Description /////////////////////////////////////////////////
		void                            query_buffer(BufferHandle buffer, BufferDescription& out_description);
		void                            query_texture(TextureHandle texture, TextureDescription& out_description);
		void                            query_pipeline(PipelineHandle pipeline, PipelineDescription& out_description);
		void                            query_sampler(SamplerHandle sampler, SamplerDescription& out_description);
		void                            query_descriptor_set_layout(DescriptorSetLayoutHandle layout, DescriptorSetLayoutDescription& out_description);
		void                            query_descriptor_set(DescriptorSetHandle set, DesciptorSetDescription& out_description);
		void                            query_shader_state(ShaderstateHandle shader, ShaderStateDescription& out_description);

		const RenderPassOutput&			get_render_pass_output(RenderPassHandle render_pass) const;

		// Update/Reload resources ///////////////////////////////////////////
		void                            resize_output_textures(FramebufferHandle render_pass, uint32_t width, uint32_t height);
		void                            resize_texture(TextureHandle texture, uint32_t width, uint32_t height);

		void                            update_descriptor_set(DescriptorSetHandle set);

		// Misc //////////////////////////////////////////////////////////////
		void                            link_texture_sampler(TextureHandle texture, SamplerHandle sampler);   // TODO: for now specify a sampler for a texture or use the default one.

		void                            set_present_mode(VkPresentModeKHR  mode);

		void                            frame_counters_advance();

		bool                            get_family_queue(VkPhysicalDevice physical_device);

		VkShaderModuleCreateInfo        compile_shader(cstring code, uint32_t code_size, VkShaderStageFlagBits stage, const std::string& name);

		// Swapchain //////////////////////////////////////////////////////////
		void                            create_swapchain();
		void                            destroy_swapchain();
		void                            resize_swapchain();

		// Map/Unmap /////////////////////////////////////////////////////////
		void*							map_buffer(const MapBufferParameters& parameters);
		void                            unmap_buffer(const MapBufferParameters& parameters);

		void*							dynamic_allocate(uint32_t size);

		void                            set_buffer_global_offset(BufferHandle buffer, uint32_t offset);

		// Command Buffers ///////////////////////////////////////////////////
		CommandBuffer*					get_command_buffer(uint32_t thread_index, bool begin);
		CommandBuffer*					get_secondary_command_buffer(uint32_t thread_index);

		void                            queue_command_buffer(CommandBuffer* command_buffer);          // Queue command buffer that will not be executed until present is called.

		// Rendering /////////////////////////////////////////////////////////
		void                            new_frame();
		void                            present();
		void                            resize(uint16_t width, uint16_t height);

		void                            fill_barrier(FramebufferHandle render_pass, ExecutionBarrier& out_barrier);

		BufferHandle                    get_fullscreen_vertex_buffer() const;           // Returns a vertex buffer usable for fullscreen shaders that uses no vertices.
		RenderPassHandle                get_swapchain_pass() const;                     // Returns what is considered the final pass that writes to the swapchain.
		FramebufferHandle               get_current_framebuffer() const;                // Returns the framebuffer for the active swapchain image

		TextureHandle                   get_dummy_texture() const;
		BufferHandle                    get_dummy_constant_buffer() const;
		const RenderPassOutput&			get_swapchain_output() const { return swapchain_output; }

		VkRenderPass                    get_vulkan_render_pass(const RenderPassOutput& output, const std::string& name);

		// Names and markers /////////////////////////////////////////////////
		void                            set_resource_name(VkObjectType object_type, uint64_t handle, const std::string& name);
		void                            push_marker(VkCommandBuffer command_buffer, const std::string& name);
		void                            pop_marker(VkCommandBuffer command_buffer);

		// Instant methods ///////////////////////////////////////////////////
		void                            destroy_buffer_instant(ResourceHandle buffer);
		void                            destroy_texture_instant(ResourceHandle texture);
		void                            destroy_pipeline_instant(ResourceHandle pipeline);
		void                            destroy_sampler_instant(ResourceHandle sampler);
		void                            destroy_descriptor_set_layout_instant(ResourceHandle layout);
		void                            destroy_descriptor_set_instant(ResourceHandle set);
		void                            destroy_render_pass_instant(ResourceHandle render_pass);
		void                            destroy_framebuffer_instant(ResourceHandle framebuffer);
		void                            destroy_shader_state_instant(ResourceHandle shader);

		void                            update_descriptor_set_instant(const DescriptorSetUpdate& update);

		// Memory Statistics //////////////////////////////////////////////////
		u32                             get_memory_heap_count();

		ResourcePool                    buffers;
		ResourcePool                    textures;
		ResourcePool                    pipelines;
		ResourcePool                    samplers;
		ResourcePool                    descriptor_set_layouts;
		ResourcePool                    descriptor_sets;
		ResourcePool                    render_passes;
		ResourcePool                    framebuffers;
		ResourcePool                    command_buffers;
		ResourcePool                    shaders;

		// Primitive resources
		BufferHandle                    fullscreen_vertex_buffer;
		RenderPassHandle                swapchain_render_pass{ k_invalid_index };
		SamplerHandle                   default_sampler;
		// Dummy resources
		TextureHandle                   dummy_texture;
		BufferHandle                    dummy_constant_buffer;

		RenderPassOutput                swapchain_output;

		StringBuffer                    string_buffer;

		Allocator*						allocator;
		StackAllocator*					temporary_allocator;

		uint32_t                             dynamic_max_per_frame_size;
		BufferHandle						 dynamic_buffer;
		uint8_t*							 dynamic_mapped_memory;
		uint32_t                             dynamic_allocated_size;
		uint32_t                             dynamic_per_frame_size;

		CommandBuffer**						 queued_command_buffers = nullptr;
		uint32_t                             num_allocated_command_buffers = 0;
		uint32_t                             num_queued_command_buffers = 0;

		VkPresentModeKHR					 present_mode = VK_PRESENT_MODE_FIFO_KHR;
		uint32_t                             current_frame;
		uint32_t                             previous_frame;

		uint32_t                             absolute_frame;

		uint16_t							swapchain_width = 1;
		uint16_t							swapchain_height = 1;

		bool                            bindless_supported = true;
		bool                            resized = false;
		bool                            vertical_sync = false;

		static constexpr cstring        k_name = "syi_gpu_service";


		VkAllocationCallbacks*			vulkan_allocation_callbacks;
		VkInstance                      vulkan_instance;
		VkPhysicalDevice                vulkan_physical_device;
		VkPhysicalDeviceProperties      vulkan_physical_properties;
		VkDevice                        vulkan_device;
		VkQueue                         vulkan_main_queue;
		VkQueue                         vulkan_transfer_queue;
		uint32_t                        vulkan_main_queue_family;
		uint32_t                        vulkan_transfer_queue_family;
		VkDescriptorPool                vulkan_descriptor_pool;

		// [TAG: BINDLESS]
		VkDescriptorPool                vulkan_bindless_descriptor_pool;
		VkDescriptorSet                 vulkan_bindless_descriptor_set_cached;  // Cached but will be removed with its associated DescriptorSet.
		DescriptorSetLayoutHandle       bindless_descriptor_set_layout;
		DescriptorSetHandle             bindless_descriptor_set;

		// Swapchain
		std::array<FramebufferHandle,k_max_swapchain_images> vulkan_swapchain_framebuffers{ k_invalid_index, k_invalid_index, k_invalid_index };

		VkQueryPool                     vulkan_timestamp_query_pool;
		// Per frame synchronization
		std::array<VkSemaphore, k_max_swapchain_images> vulkan_render_complete_semaphore;
		std::array<VkSemaphore, k_max_swapchain_images> vulkan_image_acquired_semaphore;
		std::array<VkFence, k_max_swapchain_images>     vulkan_command_buffer_executed_fence;

		static const uint32_t           k_max_frames = 3;

		// Windows specific
		VkSurfaceKHR                    vulkan_window_surface;
		VkSurfaceFormatKHR              vulkan_surface_format;
		VkPresentModeKHR                vulkan_present_mode;
		VkSwapchainKHR                  vulkan_swapchain;
		uint32_t                        vulkan_swapchain_image_count;

		VkDebugReportCallbackEXT        vulkan_debug_callback;
		VkDebugUtilsMessengerEXT        vulkan_debug_utils_messenger;

		uint32_t                        vulkan_image_index;

		VmaAllocator                    vma_allocator;

		// Extension functions
		PFN_vkCmdBeginRenderingKHR      cmd_begin_rendering;
		PFN_vkCmdEndRenderingKHR        cmd_end_rendering;

		// These are dynamic - so that workload can be handled correctly.
		Array<ResourceUpdate>           resource_deletion_queue;
		Array<DescriptorSetUpdate>      descriptor_set_updates;
		// [TAG: BINDLESS]
		Array<ResourceUpdate>           texture_to_update_bindless;

		bool                            debug_utils_extension_present = false;
		bool                            dynamic_rendering_extension_present = false;

		size_t                           ubo_alignment = 256;
		size_t                           ssbo_alignemnt = 256;

		char                            vulkan_binaries_path[512];

		ShaderState* access_shader_state(ShaderstateHandle shader);
		const ShaderState* access_shader_state(ShaderstateHandle shader) const;

		Texture* access_texture(TextureHandle texture);
		const Texture* access_texture(TextureHandle texture) const;

		Buffer* access_buffer(BufferHandle buffer);
		const Buffer* access_buffer(BufferHandle buffer) const;

		Pipeline* access_pipeline(PipelineHandle pipeline);
		const Pipeline* access_pipeline(PipelineHandle pipeline) const;

		Sampler* access_sampler(SamplerHandle sampler);
		const Sampler* access_sampler(SamplerHandle sampler) const;

		DescriptorSetLayout* access_descriptor_set_layout(DescriptorSetLayoutHandle layout);
		const DescriptorSetLayout* access_descriptor_set_layout(DescriptorSetLayoutHandle layout) const;

		DescriptorSetLayoutHandle get_descriptor_set_layout(PipelineHandle pipeline_handle, int layout_index);
		DescriptorSetLayoutHandle get_descriptor_set_layout(PipelineHandle pipeline_handle, int layout_index) const;

		DescriptorSet* access_descriptor_set(DescriptorSetHandle set);
		const DescriptorSet* access_descriptor_set(DescriptorSetHandle set) const;

		RenderPass* access_render_pass(RenderPassHandle render_pass);
		const RenderPass* access_render_pass(RenderPassHandle render_pass) const;

		Framebuffer* access_framebuffer(FramebufferHandle framebuffer);
		const Framebuffer* access_framebuffer(FramebufferHandle framebuffer) const;
	};
}