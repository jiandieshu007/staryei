#pragma once

#include "foundation/platform.hpp"

#include <vector>
#include <array>
#include <vulkan/vulkan.h>
#include "external/vk_mem_alloc.h"
#include <string>

namespace syi {
	namespace ResourceUsageType {
		enum Enum {
			Immutable, Dynamic, Stream, Staging, Count
		};

		enum Mask {
			Immutable_mask = 1 << 0, Dynamic_mask = 1 << 1, Stream_mask = 1 << 2, Staging_mask = 1 << 3, Count_mask = 1 << 4
		};

		static const char* s_value_names[] = {
			"Immutable", "Dynamic", "Stream", "Staging", "Count"
		};

		static const char* ToString(Enum e) {
			return ((u32)e < Enum::Count ? s_value_names[(int)e] : "unsupported");
		}
	} // namespace ResourceUsageType

	namespace ResourceUpdateType {

		enum Enum {
			Buffer, Texture, Pipeline, Sampler, DescriptorSetLayout, DescriptorSet, RenderPass, Framebuffer, ShaderState, Count
		};
	} // namespace ResourceUpdateType

	namespace QueueType {
		enum Enum {
			Graphics, Compute, CopyTransfer, Count
		};

		enum Mask {
			Graphics_mask = 1 << 0, Compute_mask = 1 << 1, CopyTransfer_mask = 1 << 2, Count_mask = 1 << 3
		};

		static const char* s_value_names[] = {
			"Graphics", "Compute", "CopyTransfer", "Count"
		};

		static const char* ToString(Enum e) {
			return ((u32)e < Enum::Count ? s_value_names[(int)e] : "unsupported");
		}
	} // namespace QueueType

	// TODO: taken from the Forge
	typedef enum ResourceState {
		RESOURCE_STATE_UNDEFINED = 0,
		RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER = 0x1,
		RESOURCE_STATE_INDEX_BUFFER = 0x2,
		RESOURCE_STATE_RENDER_TARGET = 0x4,
		RESOURCE_STATE_UNORDERED_ACCESS = 0x8,
		RESOURCE_STATE_DEPTH_WRITE = 0x10,
		RESOURCE_STATE_DEPTH_READ = 0x20,
		RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE = 0x40,
		RESOURCE_STATE_PIXEL_SHADER_RESOURCE = 0x80,
		RESOURCE_STATE_SHADER_RESOURCE = 0x40 | 0x80,
		RESOURCE_STATE_STREAM_OUT = 0x100,
		RESOURCE_STATE_INDIRECT_ARGUMENT = 0x200,
		RESOURCE_STATE_COPY_DEST = 0x400,
		RESOURCE_STATE_COPY_SOURCE = 0x800,
		RESOURCE_STATE_GENERIC_READ = (((((0x1 | 0x2) | 0x40) | 0x80) | 0x200) | 0x800),
		RESOURCE_STATE_PRESENT = 0x1000,
		RESOURCE_STATE_COMMON = 0x2000,
		RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE = 0x4000,
		RESOURCE_STATE_SHADING_RATE_SOURCE = 0x8000,
	} ResourceState;

	namespace spirv {
		struct ParseResult;
	}


	static const uint32_t                    k_invalid_index = 0xffffffff;

	static const uint32_t k_buffers_pool_size = 16384;
	static const uint32_t k_textures_pool_size = 512;
	static const uint32_t k_render_passes_pool_size = 256;
	static const uint32_t k_descriptor_set_layouts_pool_size = 128;
	static const uint32_t k_pipelines_pool_size = 128;
	static const uint32_t k_shaders_pool_size = 128;
	static const uint32_t k_descriptor_sets_pool_size = 4096;
	static const uint32_t k_samplers_pool_size = 32;

	struct ResourceHandle {
		uint32_t index;
	};
	struct BufferHandle : ResourceHandle {};
	struct TextureHandle : ResourceHandle {};
	struct ShaderstateHandle : ResourceHandle {};
	struct SamplerHandle : ResourceHandle {};
	struct DescriptorSetLayoutHandle : ResourceHandle {};
	struct DescriptorSetHandle : ResourceHandle {};
	struct PipelineHandle : ResourceHandle {};
	struct RenderPassHandle : ResourceHandle {};
	struct FramebufferHandle : ResourceHandle {};

	// Invalid handles
	static BufferHandle                 k_invalid_buffer{ k_invalid_index };
	static TextureHandle                k_invalid_texture{ k_invalid_index };
	static ShaderstateHandle            k_invalid_shader{ k_invalid_index };
	static SamplerHandle                k_invalid_sampler{ k_invalid_index };
	static DescriptorSetLayoutHandle    k_invalid_layout{ k_invalid_index };
	static DescriptorSetHandle          k_invalid_set{ k_invalid_index };
	static PipelineHandle               k_invalid_pipeline{ k_invalid_index };
	static RenderPassHandle             k_invalid_pass{ k_invalid_index };
	static FramebufferHandle            k_invalid_framebuffer{ k_invalid_index };

	static const uint8_t                     k_max_image_outputs = 8;                // Maximum number of images/render_targets/fbo attachments usable.
	static const uint8_t                     k_max_descriptor_set_layouts = 8;       // Maximum number of layouts in the pipeline.
	static const uint8_t                     k_max_shader_stages = 5;                // Maximum simultaneous shader stages. Applicable to all different type of pipelines.
	static const uint8_t                     k_max_descriptors_per_set = 16;         // Maximum list elements for both descriptor set layout and descriptor sets.
	static const uint8_t                     k_max_vertex_streams = 16;
	static const uint8_t                     k_max_vertex_attributes = 16;

	static const uint32_t                    k_submit_header_sentinel = 0xfefeb7ba;
	static const uint32_t                    k_max_resource_deletions = 64;

	// Resource CreationInfo info structs
	struct Offset2D {
		int32_t x = 0, y = 0;
	};

	struct Extent2D {
		uint32_t width = 0, height = 0;
	};

	struct Rect2D {
		Offset2D offest{};
		Extent2D extent{};
	};

	struct Viewport {
		float x = 0, y = 0;
		float width = 0, height = 0;
		float minDepth = 0, maxDepth = 0;
	};

	struct ViewportState {
		std::vector<Viewport> viewports;
		std::vector<Rect2D> scissors;
	};

	//
	//
	struct StencilOperationState {

		VkStencilOp                     fail = VK_STENCIL_OP_KEEP;
		VkStencilOp                     pass = VK_STENCIL_OP_KEEP;
		VkStencilOp                     depth_fail = VK_STENCIL_OP_KEEP;
		VkCompareOp                     compare = VK_COMPARE_OP_ALWAYS;
		uint32_t                             compare_mask = 0xff;
		uint32_t                             write_mask = 0xff;
		uint32_t                             reference = 0xff;

	}; // struct StencilOperationState

	//
	//
	struct DepthStencilCreationInfo {

		StencilOperationState           front;
		StencilOperationState           back;
		VkCompareOp                     depth_comparison = VK_COMPARE_OP_ALWAYS;
		// 
		uint8_t                              depth_enable : 1;
		uint8_t                              depth_write_enable : 1;
		uint8_t                              stencil_enable : 1;
		uint8_t                              pad : 5;

		// Default constructor
		DepthStencilCreationInfo() : depth_enable(0), depth_write_enable(0), stencil_enable(0) {
		}

		DepthStencilCreationInfo& set_depth(bool write, VkCompareOp comparison_test);

	}; // struct DepthStencilCreationInfo

	struct BlendState {

		VkBlendFactor                   source_color = VK_BLEND_FACTOR_ONE;
		VkBlendFactor                   destination_color = VK_BLEND_FACTOR_ONE;
		VkBlendOp                       color_operation = VK_BLEND_OP_ADD;

		VkBlendFactor                   source_alpha = VK_BLEND_FACTOR_ONE;
		VkBlendFactor                   destination_alpha = VK_BLEND_FACTOR_ONE;
		VkBlendOp                       alpha_operation = VK_BLEND_OP_ADD;

		VkColorComponentFlags         color_write_mask = 0xf; // write all 

		uint8_t                              blend_enabled : 1;
		uint8_t                              separate_blend : 1;
		uint8_t                              pad : 6;


		BlendState() : blend_enabled(0), separate_blend(0) {
		}

		BlendState& set_color(VkBlendFactor source, VkBlendFactor destination, VkBlendOp operation);
		BlendState& set_alpha(VkBlendFactor source, VkBlendFactor destination, VkBlendOp operation);
		BlendState& set_color_write_mask(VkColorComponentFlags value);

	}; // struct BlendState

	struct BlendStateCreationInfo {

		std::array<BlendState,k_max_image_outputs> blend_states;
		uint32_t                             active_states = 0;

		BlendStateCreationInfo& reset();
		BlendState& add_blend_state();

	}; // BlendStateCreationInfo

	struct RasterizationCreationInfo {

		VkCullModeFlagBits              cull_mode = VK_CULL_MODE_NONE;
		VkFrontFace                     front = VK_FRONT_FACE_COUNTER_CLOCKWISE;
		VkPolygonMode                  fill = VK_POLYGON_MODE_FILL;
	}; // struct RasterizationCreationInfo

	struct BufferCreationInfo {

		VkBufferUsageFlags              type_flags = 0;
		VkBufferUsageFlagBits          usage ;
		uint32_t                             size = 0;
		uint32_t                             persistent = 0;
		uint32_t                             device_only = 0;
		void* initial_data = nullptr;

		std::string name{} ;

		BufferCreationInfo& reset();
		BufferCreationInfo& set(VkBufferUsageFlags flags, VkBufferUsageFlagBits usage, uint32_t size);
		BufferCreationInfo& set_data(void* data);
		BufferCreationInfo& set_name(const std::string & name);
		BufferCreationInfo& set_persistent(bool value);
		BufferCreationInfo& set_device_only(bool value);

	}; // struct BufferCreationInfo

	struct TextureCreationInfo {

		void* initial_data = nullptr;
		uint16_t                             width = 1;
		uint16_t                             height = 1;
		uint16_t                             depth = 1;
		uint8_t                              mipmaps = 1;
		uint8_t                              flags = 0;    // TextureFlags bitmasks

		VkFormat                        format = VK_FORMAT_UNDEFINED;
		VkImageViewType               type = VK_IMAGE_VIEW_TYPE_2D;

		TextureHandle                   alias = k_invalid_texture;

		std::string name {};

		TextureCreationInfo& set_size(uint16_t width, uint16_t height, uint16_t depth);
		TextureCreationInfo& set_flags(uint8_t mipmaps, uint8_t flags);
		TextureCreationInfo& set_format_type(VkFormat format, VkImageViewType type);
		TextureCreationInfo& set_name(const std::string& name);
		TextureCreationInfo& set_data(void* data);
		TextureCreationInfo& set_alias(TextureHandle alias);

	}; // struct TextureCreationInfo

	struct SamplerCreationInfo {

		VkFilter                        min_filter = VK_FILTER_NEAREST;
		VkFilter                        mag_filter = VK_FILTER_NEAREST;
		VkSamplerMipmapMode             mip_filter = VK_SAMPLER_MIPMAP_MODE_NEAREST;

		VkSamplerAddressMode            address_mode_u = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		VkSamplerAddressMode            address_mode_v = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		VkSamplerAddressMode            address_mode_w = VK_SAMPLER_ADDRESS_MODE_REPEAT;

		std::string name {};

		SamplerCreationInfo& set_min_mag_mip(VkFilter min, VkFilter mag, VkSamplerMipmapMode mip);
		SamplerCreationInfo& set_address_mode_u(VkSamplerAddressMode u);
		SamplerCreationInfo& set_address_mode_uv(VkSamplerAddressMode u, VkSamplerAddressMode v);
		SamplerCreationInfo& set_address_mode_uvw(VkSamplerAddressMode u, VkSamplerAddressMode v, VkSamplerAddressMode w);
		SamplerCreationInfo& set_name(const std::string& name);

	}; // struct SamplerCreationInfo

	struct ShaderStage {

		const char* code = nullptr;
		uint32_t                             code_size = 0;
		VkShaderStageFlagBits           type = VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;

	}; // struct ShaderStage

	struct ShaderStateCreationInfo {

		std::array<ShaderStage, k_max_shader_stages>  stages;

		std::string name{};

		uint32_t                             stages_count = 0;
		uint32_t                             spv_input = 0;

		// Building helpers
		ShaderStateCreationInfo& reset();
		ShaderStateCreationInfo& set_name(const std::string& name);
		ShaderStateCreationInfo& add_stage(const char* code, sizet code_size, VkShaderStageFlagBits type);
		ShaderStateCreationInfo& set_spv_input(bool value);

	}; // struct ShaderStateCreationInfo

	struct DescriptorSetLayoutCreationInfo {

		//
		// A single descriptor binding. It can be relative to one or more resources of the same type.
		//
		struct Binding {

			VkDescriptorType            type = VK_DESCRIPTOR_TYPE_MAX_ENUM;
			uint16_t                         index = 0;
			uint16_t                         count = 0;
			std::string                     name = nullptr;  // Comes from external memory.
		}; // struct Binding

		std::array<Binding, k_max_descriptors_per_set> bindings;
		uint32_t                             num_bindings = 0;
		uint32_t                             set_index = 0;
		bool                            bindless = false;
		bool                            dynamic = false;

		std::string                         name = nullptr;

		// Building helpers
		DescriptorSetLayoutCreationInfo& reset();
		DescriptorSetLayoutCreationInfo& add_binding(const Binding& binding);
		DescriptorSetLayoutCreationInfo& add_binding(VkDescriptorType type, uint32_t index, uint32_t count, std::string name);
		DescriptorSetLayoutCreationInfo& add_binding_at_index(const Binding& binding, int index);
		DescriptorSetLayoutCreationInfo& set_name(const std::string& name);
		DescriptorSetLayoutCreationInfo& set_set_index(uint32_t index);

	}; // struct DescriptorSetLayoutCreationInfo

	struct DescriptorSetCreationInfo {

		std::array<ResourceHandle, k_max_descriptors_per_set>   resources;
		std::array<SamplerHandle, k_max_descriptors_per_set>  samplers;
		std::array<uint16_t, k_max_descriptors_per_set>  bindings;

		DescriptorSetLayoutHandle       layout;
		uint32_t                             num_resources = 0;

		std::string                         name{};

		// Building helpers
		DescriptorSetCreationInfo& reset();
		DescriptorSetCreationInfo& set_layout(DescriptorSetLayoutHandle layout);
		DescriptorSetCreationInfo& texture(TextureHandle texture, uint16_t binding);
		DescriptorSetCreationInfo& buffer(BufferHandle buffer, uint16_t binding);
		DescriptorSetCreationInfo& texture_sampler(TextureHandle texture, SamplerHandle sampler, uint16_t binding);   // TODO: separate samplers from textures
		DescriptorSetCreationInfo& set_name(const std::string& name);

	}; // struct DescriptorSetCreationInfo

	struct DescriptorSetUpdate {
		DescriptorSetHandle             descriptor_set;

		uint32_t                             frame_issued = 0;
	}; // DescriptorSetUpdate

	struct VertexAttribute {

		uint16_t                             location = 0;
		uint16_t                             binding = 0;
		uint32_t                             offset = 0;
		VkFormat     format;

	}; // struct VertexAttribute

	struct VertexStream {

		uint16_t                             binding = 0;
		uint16_t                             stride = 0;
		std::array<VkVertexInputRate, VK_VERTEX_INPUT_RATE_VERTEX>  input_rate;

	}; // struct VertexStream

	struct VertexInputCreationInfo {

		uint32_t                             num_vertex_streams = 0;
		uint32_t                             num_vertex_attributes = 0;

		std::array<VertexStream, k_max_vertex_streams> vertex_streams;
		std::array<VertexAttribute, k_max_vertex_attributes> vertex_attributes;

		VertexInputCreationInfo& reset();
		VertexInputCreationInfo& add_vertex_stream(const VertexStream& stream);
		VertexInputCreationInfo& add_vertex_attribute(const VertexAttribute& attribute);
	}; // struct VertexInputCreationInfo

	struct RenderPassOutput {

		std::array<VkFormat, k_max_image_outputs> color_formats;
		std::array<VkImageLayout, k_max_image_outputs> color_final_layouts;
		std::array<VkAttachmentLoadOp, k_max_image_outputs> color_operations;

		VkFormat                        depth_stencil_format;
		VkImageLayout                   depth_stencil_final_layout;

		uint32_t                             num_color_formats;

		VkAttachmentLoadOp       depth_operation = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		VkAttachmentLoadOp       stencil_operation = VK_ATTACHMENT_LOAD_OP_DONT_CARE;

		RenderPassOutput& reset();
		RenderPassOutput& color(VkFormat format, VkImageLayout layout, VkAttachmentLoadOp load_op);
		RenderPassOutput& depth(VkFormat format, VkImageLayout layout);
		RenderPassOutput& set_depth_stencil_operations(VkAttachmentLoadOp depth, VkAttachmentLoadOp stencil);

	}; // struct RenderPassOutput

	struct RenderPassCreationInfo {

		uint16_t                             num_render_targets = 0;

		std::array<VkFormat, k_max_image_outputs> color_formats;
		std::array<VkImageLayout, k_max_image_outputs> color_final_layouts;
		std::array<VkAttachmentLoadOp, k_max_image_outputs>  color_operations;

		VkFormat                        depth_stencil_format = VK_FORMAT_UNDEFINED;
		VkImageLayout                   depth_stencil_final_layout;

		VkAttachmentLoadOp       depth_operation = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		VkAttachmentLoadOp      stencil_operation = VK_ATTACHMENT_LOAD_OP_DONT_CARE;

		std::string name;

		RenderPassCreationInfo& reset();
		RenderPassCreationInfo& add_attachment(VkFormat format, VkImageLayout layout, VkAttachmentLoadOp load_op);
		RenderPassCreationInfo& set_depth_stencil_texture(VkFormat format, VkImageLayout layout);
		RenderPassCreationInfo& set_name(const std::string& name);
		RenderPassCreationInfo& set_depth_stencil_operations(VkAttachmentLoadOp depth, VkAttachmentLoadOp stencil);

	}; // struct RenderPassCreationInfo

	struct FramebufferCreationInfo {

		RenderPassHandle                render_pass;

		uint16_t                             num_render_targets = 0;

		std::array<TextureHandle, k_max_image_outputs>  output_textures;
		TextureHandle                   depth_stencil_texture = { k_invalid_index };

		uint16_t                             width = 0;
		uint16_t                             height = 0;

		float                             scale_x = 1.f;
		float                             scale_y = 1.f;
		uint8_t                              resize = 1;

		std::string name;

		FramebufferCreationInfo& reset();
		FramebufferCreationInfo& add_render_texture(TextureHandle texture);
		FramebufferCreationInfo& set_depth_stencil_texture(TextureHandle texture);
		FramebufferCreationInfo& set_scaling(float scale_x, float scale_y, uint8_t resize);
		FramebufferCreationInfo& set_name(const std::string& name);

	}; // struct RenderPassCreationInfo

	struct PipelineCreationInfo {

		RasterizationCreationInfo           rasterization;
		DepthStencilCreationInfo            depth_stencil;
		BlendStateCreationInfo              blend_state;
		VertexInputCreationInfo             vertex_input;
		ShaderStateCreationInfo             shaders;

		RenderPassOutput                render_pass;
		std::array<DescriptorSetLayoutHandle, k_max_descriptor_set_layouts> descriptor_set_layout;
		const ViewportState* viewport = nullptr;

		uint32_t                             num_active_layouts = 0;

		std::string name ;

		PipelineCreationInfo& add_descriptor_set_layout(DescriptorSetLayoutHandle handle);
		RenderPassOutput& render_pass_output();

	}; // struct PipelineCreationInfo


	//
	// Helper methods for texture formats
	//
	namespace TextureFormat {

		inline bool                     is_depth_stencil(VkFormat value) {
			return value == VK_FORMAT_D16_UNORM_S8_UINT || value == VK_FORMAT_D24_UNORM_S8_UINT || value == VK_FORMAT_D32_SFLOAT_S8_UINT;
		}
		inline bool                     is_depth_only(VkFormat value) {
			return value >= VK_FORMAT_D16_UNORM && value < VK_FORMAT_D32_SFLOAT;
		}
		inline bool                     is_stencil_only(VkFormat value) {
			return value == VK_FORMAT_S8_UINT;
		}

		inline bool                     has_depth(VkFormat value) {
			return (value >= VK_FORMAT_D16_UNORM && value < VK_FORMAT_S8_UINT) || (value >= VK_FORMAT_D16_UNORM_S8_UINT && value <= VK_FORMAT_D32_SFLOAT_S8_UINT);
		}
		inline bool                     has_stencil(VkFormat value) {
			return value >= VK_FORMAT_S8_UINT && value <= VK_FORMAT_D32_SFLOAT_S8_UINT;
		}
		inline bool                     has_depth_or_stencil(VkFormat value) {
			return value >= VK_FORMAT_D16_UNORM && value <= VK_FORMAT_D32_SFLOAT_S8_UINT;
		}

	} // namespace TextureFormat



	struct DescriptorData {
		void* data = nullptr;
	};

	struct DescriptorBinding {
		VkDescriptorType type;
		uint32_t index = 0, count = 0, set = 0;
		const std::string name{};
	};

	struct ShaderStateDescription {

		void* native_handle = nullptr;
		std::string                         name{};

	}; // struct ShaderStateDescription

	struct BufferDescription {

		void* native_handle = nullptr;
		cstring                         name = nullptr;

		VkBufferUsageFlags              type_flags = 0;
		VkBufferUsageFlagBits         usage;
		uint32_t                             size = 0;
		BufferHandle                    parent_handle;

	}; // struct BufferDescription

	struct TextureDescription {

		void* native_handle = nullptr;
		std::string                         name{};

		uint16_t                             width = 1;
		uint16_t                             height = 1;
		uint16_t                             depth = 1;
		uint8_t                              mipmaps = 1;
		uint8_t                              render_target = 0;
		uint8_t                              compute_access = 0;

		VkFormat                        format = VK_FORMAT_UNDEFINED;
		VkImageViewType               type = VK_IMAGE_VIEW_TYPE_2D;

	}; // struct Texture

	struct SamplerDescription {

		std::string                         name{ };

		VkFilter                        min_filter = VK_FILTER_NEAREST;
		VkFilter                        mag_filter = VK_FILTER_NEAREST;
		VkSamplerMipmapMode             mip_filter = VK_SAMPLER_MIPMAP_MODE_NEAREST;

		VkSamplerAddressMode            address_mode_u = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		VkSamplerAddressMode            address_mode_v = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		VkSamplerAddressMode            address_mode_w = VK_SAMPLER_ADDRESS_MODE_REPEAT;

	}; // struct SamplerDescription


	struct DescriptorSetLayoutDescription {
		std::vector<DescriptorBinding*> bindings;
	};

	struct DesciptorSetDescription {
		std::vector<DescriptorData*> resources;
	}; // struct DesciptorSetDescription

	struct PipelineDescription {
		ShaderstateHandle               shader;
	}; // struct PipelineDescription

	struct MapBufferParameters {
		BufferHandle                    buffer;
		uint32_t                             offset = 0;
		uint32_t                             size = 0;
	}; // struct MapBufferParameters

	struct ImageBarrier {

		TextureHandle                   texture;

	}; // struct ImageBarrier


	struct MemoryBarrier {
		BufferHandle                    buffer;
	}; // struct MemoryBarrier

	struct ExecutionBarrier {

		VkPipelineStageFlags             source_pipeline_stage;
		VkPipelineStageFlags            destination_pipeline_stage;

		uint32_t                             new_barrier_experimental = std::numeric_limits<uint32_t>::max();
		uint32_t                             load_operation = 0;

		uint32_t                             num_image_barriers;
		uint32_t                             num_memory_barriers;

		std::array<ImageBarrier,8>                    image_barriers;
		std::array<MemoryBarrier,8>                   memory_barriers;

		ExecutionBarrier& reset();
		ExecutionBarrier& set(VkPipelineStageFlags source, VkPipelineStageFlags destination);
		ExecutionBarrier& add_image_barrier(const ImageBarrier& image_barrier);
		ExecutionBarrier& add_memory_barrier(const MemoryBarrier& memory_barrier);

	}; // struct Barrier

	struct ResourceUpdate {

		ResourceUpdateType::Enum        type;
		ResourceHandle                  handle;
		uint32_t                             current_frame;
		uint32_t                             deleting;
	}; // struct ResourceUpdate

	// reources 

	static const uint32_t            k_max_swapchain_images = 3;

	struct DeviceStateVulkan;

	struct Buffer {

		VkBuffer                        vk_buffer;
		VmaAllocation                   vma_allocation;
		VkDeviceMemory                  vk_device_memory;
		VkDeviceSize                    vk_device_size;

		VkBufferUsageFlags              type_flags = 0;
		ResourceUsageType::Enum         usage = ResourceUsageType::Immutable;
		uint32_t                             size = 0;
		uint32_t                             global_offset = 0;    // Offset into global constant, if dynamic

		BufferHandle                    handle;
		BufferHandle                    parent_buffer;

		uint8_t* mapped_data = nullptr;
		std::string name{};

	}; // struct BufferVulkan

	struct Sampler {

		VkSampler                       vk_sampler;

		VkFilter                        min_filter = VK_FILTER_NEAREST;
		VkFilter                        mag_filter = VK_FILTER_NEAREST;
		VkSamplerMipmapMode             mip_filter = VK_SAMPLER_MIPMAP_MODE_NEAREST;

		VkSamplerAddressMode            address_mode_u = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		VkSamplerAddressMode            address_mode_v = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		VkSamplerAddressMode            address_mode_w = VK_SAMPLER_ADDRESS_MODE_REPEAT;

		std::string name{};

	}; // struct SamplerVulkan

	struct Texture {

		VkImage                         vk_image;
		VkImageView                     vk_image_view;
		VkFormat                        vk_format;
		VkImageLayout                   vk_image_layout;
		VmaAllocation                   vma_allocation;

		uint16_t                             width = 1;
		uint16_t                             height = 1;
		uint16_t                             depth = 1;
		uint8_t                              mipmaps = 1;
		uint8_t                              flags = 0;

		TextureHandle                   handle;
		VkImageViewType              type = VK_IMAGE_VIEW_TYPE_2D;

		Sampler* sampler = nullptr;

		std::string name{};
	}; // struct TextureVulkan

	struct ShaderState {

		std::array<VkPipelineShaderStageCreateInfo,k_max_shader_stages> shader_stage_info;

		std::string name{};

		uint32_t                             active_shaders = 0;
		bool                            graphics_pipeline = false;

		spirv::ParseResult* parse_result;
	}; // struct ShaderStateVulkan

	struct DescriptorSetLayout {

		VkDescriptorSetLayout           vk_descriptor_set_layout;

		std::vector<VkDescriptorSetLayoutBinding> vk_binding;
		std::vector<DescriptorBinding> bindings;
		uint8_t* index_to_binding = nullptr; // Mapping between binding point and binding data.
		uint16_t                             num_bindings = 0;
		uint16_t                             set_index = 0;
		uint8_t                              bindless = 0;
		uint8_t                              dynamic = 0;

		DescriptorSetLayoutHandle       handle;

	}; // struct DesciptorSetLayoutVulkan

	struct DescriptorSet {

		VkDescriptorSet                 vk_descriptor_set;

		std::vector<ResourceHandle> resources ;
		std::vector<SamplerHandle> samplers ;
		uint16_t* bindings = nullptr;

		const DescriptorSetLayout* layout = nullptr;
		uint32_t                             num_resources = 0;
	}; // struct DesciptorSet

	struct Pipeline {

		VkPipeline                      vk_pipeline;
		VkPipelineLayout                vk_pipeline_layout;

		VkPipelineBindPoint             vk_bind_point;

		ShaderstateHandle               shader_state;

		std::array<const DescriptorSetLayout*, k_max_descriptor_set_layouts> descriptor_set_layout;
		std::array<DescriptorSetLayoutHandle, k_max_descriptor_set_layouts> descriptor_set_layout_handles;
		uint32_t                             num_active_layouts = 0;

		DepthStencilCreationInfo            depth_stencil;
		BlendStateCreationInfo              blend_state;
		RasterizationCreationInfo           rasterization;

		PipelineHandle                  handle;
		bool                            graphics_pipeline = true;

	}; // struct Pipeline

	struct RenderPass {

		// NOTE(marco): this will be a null handle if dynamic rendering is available
		VkRenderPass                    vk_render_pass;

		RenderPassOutput                output;

		uint16_t                             dispatch_x = 0;
		uint16_t                             dispatch_y = 0;
		uint16_t                             dispatch_z = 0;

		uint8_t                              num_render_targets = 0;

		std::string name{};
	}; // struct RenderPassVulkan

	struct Framebuffer {

		// NOTE(marco): this will be a null handle if dynamic rendering is available
		VkFramebuffer                   vk_framebuffer;

		// NOTE(marco): cache render pass handle
		RenderPassHandle                render_pass;

		uint16_t                             width = 0;
		uint16_t                             height = 0;

		float                             scale_x = 1.f;
		float                             scale_y = 1.f;

		TextureHandle                   color_attachments[k_max_image_outputs];
		TextureHandle                   depth_stencil_attachment;
		uint32_t                             num_color_attachments;

		uint8_t                              resize = 0;

		std::string name{};
	}; // struct Framebuffer

	struct ComputeLocalSize {

		uint32_t                             x : 10;
		uint32_t                             y : 10;
		uint32_t                             z : 10;
		uint32_t                             pad : 2;
	}; // struct ComputeLocalSize

	// Enum translations. Use tables or switches depending on the case. ////////////
	static std::string to_compiler_extension(VkShaderStageFlagBits value) {
		switch (value) {
		case VK_SHADER_STAGE_VERTEX_BIT:
			return "vert";
		case VK_SHADER_STAGE_FRAGMENT_BIT:
			return "frag";
		case VK_SHADER_STAGE_COMPUTE_BIT:
			return "comp";
		default:
			return "";
		}
	}

	//
	static std::string to_stage_defines(VkShaderStageFlagBits value) {
		switch (value) {
		case VK_SHADER_STAGE_VERTEX_BIT:
			return "VERTEX";
		case VK_SHADER_STAGE_FRAGMENT_BIT:
			return "FRAGMENT";
		case VK_SHADER_STAGE_COMPUTE_BIT:
			return "COMPUTE";
		default:
			return "";
		}
	}

	static VkImageLayout util_to_vk_image_layout(ResourceState usage) {
		if (usage & RESOURCE_STATE_COPY_SOURCE)
			return VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;

		if (usage & RESOURCE_STATE_COPY_DEST)
			return VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;

		if (usage & RESOURCE_STATE_RENDER_TARGET)
			return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		if (usage & RESOURCE_STATE_DEPTH_WRITE)
			return VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		if (usage & RESOURCE_STATE_DEPTH_READ)
			return VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;

		if (usage & RESOURCE_STATE_UNORDERED_ACCESS)
			return VK_IMAGE_LAYOUT_GENERAL;

		if (usage & RESOURCE_STATE_SHADER_RESOURCE)
			return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

		if (usage & RESOURCE_STATE_PRESENT)
			return VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		if (usage == RESOURCE_STATE_COMMON)
			return VK_IMAGE_LAYOUT_GENERAL;

		return VK_IMAGE_LAYOUT_UNDEFINED;
	}

	static VkAccessFlags util_to_vk_access_flags(ResourceState state) {
		VkAccessFlags ret = 0;
		if (state & RESOURCE_STATE_COPY_SOURCE) {
			ret |= VK_ACCESS_TRANSFER_READ_BIT;
		}
		if (state & RESOURCE_STATE_COPY_DEST) {
			ret |= VK_ACCESS_TRANSFER_WRITE_BIT;
		}
		if (state & RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER) {
			ret |= VK_ACCESS_UNIFORM_READ_BIT | VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT;
		}
		if (state & RESOURCE_STATE_INDEX_BUFFER) {
			ret |= VK_ACCESS_INDEX_READ_BIT;
		}
		if (state & RESOURCE_STATE_UNORDERED_ACCESS) {
			ret |= VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT;
		}
		if (state & RESOURCE_STATE_INDIRECT_ARGUMENT) {
			ret |= VK_ACCESS_INDIRECT_COMMAND_READ_BIT;
		}
		if (state & RESOURCE_STATE_RENDER_TARGET) {
			ret |= VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		}
		if (state & RESOURCE_STATE_DEPTH_WRITE) {
			ret |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
		}
		if (state & RESOURCE_STATE_SHADER_RESOURCE) {
			ret |= VK_ACCESS_SHADER_READ_BIT;
		}
		if (state & RESOURCE_STATE_PRESENT) {
			ret |= VK_ACCESS_MEMORY_READ_BIT;
		}
#ifdef ENABLE_RAYTRACING
		if (state & RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE) {
			ret |= VK_ACCESS_ACCELERATION_STRUCTURE_READ_BIT_NV | VK_ACCESS_ACCELERATION_STRUCTURE_WRITE_BIT_NV;
		}
#endif

		return ret;
	}

	// Determines pipeline stages involved for given accesses
	static VkPipelineStageFlags util_determine_pipeline_stage_flags(VkAccessFlags accessFlags, QueueType::Enum queueType) {
		VkPipelineStageFlags flags = 0;

		switch (queueType) {
		case QueueType::Graphics:
		{
			if ((accessFlags & (VK_ACCESS_INDEX_READ_BIT | VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT)) != 0)
				flags |= VK_PIPELINE_STAGE_VERTEX_INPUT_BIT;

			if ((accessFlags & (VK_ACCESS_UNIFORM_READ_BIT | VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT)) != 0) {
				flags |= VK_PIPELINE_STAGE_VERTEX_SHADER_BIT;
				flags |= VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
				/*if ( pRenderer->pActiveGpuSettings->mGeometryShaderSupported ) {
					flags |= VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT;
				}
				if ( pRenderer->pActiveGpuSettings->mTessellationSupported ) {
					flags |= VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT;
					flags |= VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT;
				}*/
				flags |= VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
#ifdef ENABLE_RAYTRACING
				if (pRenderer->mVulkan.mRaytracingExtension) {
					flags |= VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_NV;
				}
#endif
			}

			if ((accessFlags & VK_ACCESS_INPUT_ATTACHMENT_READ_BIT) != 0)
				flags |= VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;

			if ((accessFlags & (VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT)) != 0)
				flags |= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

			if ((accessFlags & (VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT)) != 0)
				flags |= VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;

			break;
		}
		case QueueType::Compute:
		{
			if ((accessFlags & (VK_ACCESS_INDEX_READ_BIT | VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT)) != 0 ||
				(accessFlags & VK_ACCESS_INPUT_ATTACHMENT_READ_BIT) != 0 ||
				(accessFlags & (VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT)) != 0 ||
				(accessFlags & (VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT)) != 0)
				return VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;

			if ((accessFlags & (VK_ACCESS_UNIFORM_READ_BIT | VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT)) != 0)
				flags |= VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;

			break;
		}
		case QueueType::CopyTransfer: return VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
		default: break;
		}

		// Compatible with both compute and graphics queues
		if ((accessFlags & VK_ACCESS_INDIRECT_COMMAND_READ_BIT) != 0)
			flags |= VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT;

		if ((accessFlags & (VK_ACCESS_TRANSFER_READ_BIT | VK_ACCESS_TRANSFER_WRITE_BIT)) != 0)
			flags |= VK_PIPELINE_STAGE_TRANSFER_BIT;

		if ((accessFlags & (VK_ACCESS_HOST_READ_BIT | VK_ACCESS_HOST_WRITE_BIT)) != 0)
			flags |= VK_PIPELINE_STAGE_HOST_BIT;

		if (flags == 0)
			flags = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;

		return flags;
	}

	void util_add_image_barrier(VkCommandBuffer command_buffer, VkImage image, ResourceState old_state, ResourceState new_state,
		uint32_t base_mip_level, uint32_t mip_count, bool is_depth);

	void util_add_image_barrier_ext(VkCommandBuffer command_buffer, VkImage image, ResourceState old_state, ResourceState new_state,
		uint32_t base_mip_level, uint32_t mip_count, bool is_depth, uint32_t source_family, uint32_t destination_family,
		QueueType::Enum source_queue_type, QueueType::Enum destination_queue_type);

	void util_add_buffer_barrier_ext(VkCommandBuffer command_buffer, VkBuffer buffer, ResourceState old_state, ResourceState new_state,
		uint32_t buffer_size, uint32_t source_family, uint32_t destination_family,
		QueueType::Enum source_queue_type, QueueType::Enum destination_queue_type);

	VkFormat util_string_to_vk_format(cstring format);
}