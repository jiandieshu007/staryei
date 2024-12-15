
#include "GpuResource.hpp"
#include "GpuResource.hpp"

syi::DepthStencilCreationInfo& syi::DepthStencilCreationInfo::set_depth(bool write, VkCompareOp comparison_test)
{
	depth_write_enable = write;
	depth_comparison = comparison_test;
	depth_enable = 1;
	return *this;
}

syi::BlendState& syi::BlendState::set_color(VkBlendFactor source, VkBlendFactor destination,
	VkBlendOp operation)
{
	source_color = source;
	destination_color = destination;
	color_operation = operation;
	blend_enabled = 1;

	return *this;
}

syi::BlendState& syi::BlendState::set_alpha(VkBlendFactor source, VkBlendFactor destination,
	VkBlendOp operation)
{
	source_alpha = source;
	destination_alpha = destination;
	alpha_operation = operation;
	separate_blend = 1;

	return *this;
}

syi::BlendState& syi::BlendState::set_color_write_mask(VkColorComponentFlags value)
{
	color_write_mask = value;

	return *this;
}

syi::BlendStateCreationInfo& syi::BlendStateCreationInfo::reset()
{
	active_states = 0;
	return *this;
}

syi::BlendState& syi::BlendStateCreationInfo::add_blend_state()
{
	return blend_states[active_states++];
}

syi::BufferCreationInfo& syi::BufferCreationInfo::reset()
{
	type_flags = 0;
 // usage = ?
	size = 0;
	initial_data = nullptr;
	persistent = 0;
	device_only = 0;
	name.clear();

	return *this;
}

syi::BufferCreationInfo& syi::BufferCreationInfo::set(VkBufferUsageFlags flags, VkBufferUsageFlagBits usage,
	uint32_t size)
{
	type_flags = flags;
	this->usage = usage;
	this->size = size;

	return *this;
}

syi::BufferCreationInfo& syi::BufferCreationInfo::set_data(void* data)
{
	initial_data = data;

	return *this;
}

syi::BufferCreationInfo& syi::BufferCreationInfo::set_name(const std::string& name)
{
	this->name = name;

	return *this;
}

syi::BufferCreationInfo& syi::BufferCreationInfo::set_persistent(bool value)
{
	persistent = value ? 1 : 0;

	return *this;
}

syi::BufferCreationInfo& syi::BufferCreationInfo::set_device_only(bool value)
{
	device_only = value ? 1 : 0;

	return *this;
}

syi::TextureCreationInfo& syi::TextureCreationInfo::set_size(uint16_t width, uint16_t height, uint16_t depth)
{
	this->width = width;
	this->height = height;
	this->depth = depth;

	return *this;
}

syi::TextureCreationInfo& syi::TextureCreationInfo::set_flags(uint8_t mipmaps, uint8_t flags)
{
	this->mipmaps = mipmaps;
	this->flags = flags;

	return *this;
}

syi::TextureCreationInfo& syi::TextureCreationInfo::set_format_type(VkFormat format, VkImageViewType type)
{
	this->format = format;
	this->type = type;

	return *this;
}

syi::TextureCreationInfo& syi::TextureCreationInfo::set_name(const std::string& name)
{
	this->name = name;

	return *this;
}

syi::TextureCreationInfo& syi::TextureCreationInfo::set_data(void* data)
{
	initial_data = data;

	return *this;
}

syi::TextureCreationInfo& syi::TextureCreationInfo::set_alias(TextureHandle alias)
{
	this->alias = alias;

	return *this;
}

syi::SamplerCreationInfo& syi::SamplerCreationInfo::set_min_mag_mip(VkFilter min, VkFilter mag, VkSamplerMipmapMode mip)
{
	min_filter = min;
	mag_filter = mag;
	mip_filter = mip;

	return *this;
}

syi::SamplerCreationInfo& syi::SamplerCreationInfo::set_address_mode_u(VkSamplerAddressMode u)
{
	address_mode_u = u;

	return *this;
}

syi::SamplerCreationInfo& syi::SamplerCreationInfo::set_address_mode_uv(VkSamplerAddressMode u, VkSamplerAddressMode v)
{
	address_mode_v = v;

	return *this;
}

syi::SamplerCreationInfo& syi::SamplerCreationInfo::set_address_mode_uvw(VkSamplerAddressMode u, VkSamplerAddressMode v,
	VkSamplerAddressMode w)
{
	address_mode_w = w;

	return *this;
}

syi::SamplerCreationInfo& syi::SamplerCreationInfo::set_name(const std::string& name)
{
	this->name = name;

	return *this;
}

syi::ShaderStateCreationInfo& syi::ShaderStateCreationInfo::reset()
{
	stages_count = 0;

	return *this;
}

syi::ShaderStateCreationInfo& syi::ShaderStateCreationInfo::set_name(const std::string& name)
{
	this->name = name;

	return *this;
}

syi::ShaderStateCreationInfo& syi::ShaderStateCreationInfo::add_stage(const char* code, sizet code_size,
	VkShaderStageFlagBits type)
{
	stages[stages_count].code = code;
	stages[stages_count].code_size = static_cast<uint32_t>(code_size);
	stages[stages_count].type = type;
	++stages_count;

	return *this;
}

syi::ShaderStateCreationInfo& syi::ShaderStateCreationInfo::set_spv_input(bool value)
{
	spv_input = value;

	return *this;
}

syi::DescriptorSetLayoutCreationInfo& syi::DescriptorSetLayoutCreationInfo::reset()
{
	num_bindings = 0;
	set_index = 0;

	return *this;
}

syi::DescriptorSetLayoutCreationInfo& syi::DescriptorSetLayoutCreationInfo::add_binding(const Binding& binding)
{
	bindings[num_bindings++] = binding;

	return *this;
}

syi::DescriptorSetLayoutCreationInfo& syi::DescriptorSetLayoutCreationInfo::add_binding(VkDescriptorType type,
	uint32_t index, uint32_t count, std::string name)
{
	bindings[num_bindings++] = { type,static_cast<uint16_t>(index),static_cast<uint16_t>(count), name };

	return *this;
}

syi::DescriptorSetLayoutCreationInfo& syi::DescriptorSetLayoutCreationInfo::add_binding_at_index(const Binding& binding,
	int index)
{
	bindings[index] = binding;
	num_bindings = (index + 1) > num_bindings ? index + 1 : num_bindings;

	return *this;
}

syi::DescriptorSetLayoutCreationInfo& syi::DescriptorSetLayoutCreationInfo::set_name(const std::string& name)
{
	this->name = name;

	return *this;
}

syi::DescriptorSetLayoutCreationInfo& syi::DescriptorSetLayoutCreationInfo::set_set_index(uint32_t index)
{
	set_index = index;

	return *this;
}

syi::DescriptorSetCreationInfo& syi::DescriptorSetCreationInfo::reset()
{
	num_resources = 0;

	return *this;
}

syi::DescriptorSetCreationInfo& syi::DescriptorSetCreationInfo::set_layout(DescriptorSetLayoutHandle layout)
{
	this->layout = layout;

	return *this;
}

syi::DescriptorSetCreationInfo& syi::DescriptorSetCreationInfo::texture(TextureHandle texture, uint16_t binding)
{
	samplers[num_resources] = k_invalid_sampler;
	bindings[num_resources] = binding;
	resources[num_resources++] = static_cast<ResourceHandle>(texture.index);

	return *this;
}

syi::DescriptorSetCreationInfo& syi::DescriptorSetCreationInfo::buffer(BufferHandle buffer, uint16_t binding)
{
	samplers[num_resources] = k_invalid_sampler;
	bindings[num_resources] = binding;
	resources[num_resources++] = { static_cast<ResourceHandle>(buffer.index) };

	return *this;
}

syi::DescriptorSetCreationInfo& syi::DescriptorSetCreationInfo::texture_sampler(TextureHandle texture,
	SamplerHandle sampler, uint16_t binding)
{
	bindings[num_resources] = binding;
	resources[num_resources] = static_cast<ResourceHandle>(texture.index);;
	samplers[num_resources++] = sampler;

	return *this;
}

syi::DescriptorSetCreationInfo& syi::DescriptorSetCreationInfo::set_name(const std::string& name)
{
	this->name = name;

	return *this;
}

syi::VertexInputCreationInfo& syi::VertexInputCreationInfo::reset()
{
	num_vertex_streams = num_vertex_attributes = 0;

	return *this;
}

syi::VertexInputCreationInfo& syi::VertexInputCreationInfo::add_vertex_stream(const VertexStream& stream)
{
	vertex_streams[num_vertex_streams++] = stream;

	return *this;
}

syi::VertexInputCreationInfo& syi::VertexInputCreationInfo::add_vertex_attribute(const VertexAttribute& attribute)
{
	vertex_attributes[num_vertex_attributes++] = attribute;

	return *this;
}

syi::RenderPassOutput& syi::RenderPassOutput::reset()
{
	num_color_formats = 0;
	for (u32 i = 0; i < k_max_image_outputs; ++i) {
		color_formats[i] = VK_FORMAT_UNDEFINED;
		color_final_layouts[i] = VK_IMAGE_LAYOUT_UNDEFINED;
		color_operations[i] = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	}
	depth_stencil_format = VK_FORMAT_UNDEFINED;
	depth_operation = stencil_operation = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	return *this;
}

syi::RenderPassOutput& syi::RenderPassOutput::color(VkFormat format, VkImageLayout layout, VkAttachmentLoadOp load_op)
{
	color_formats[num_color_formats] = format;
	color_operations[num_color_formats] = load_op;
	color_final_layouts[num_color_formats++] = layout;

	return *this;
}

syi::RenderPassOutput& syi::RenderPassOutput::depth(VkFormat format, VkImageLayout layout)
{
	depth_stencil_format = format;
	depth_stencil_final_layout = layout;

	return *this;
}

syi::RenderPassOutput& syi::RenderPassOutput::set_depth_stencil_operations(VkAttachmentLoadOp depth,
	VkAttachmentLoadOp stencil)
{
	depth_operation = depth;
	stencil_operation = stencil;

	return *this;
}

syi::RenderPassCreationInfo& syi::RenderPassCreationInfo::reset()
{
	num_render_targets = 0;
	depth_stencil_format = VK_FORMAT_UNDEFINED;
	for (u32 i = 0; i < k_max_image_outputs; ++i) {
		color_operations[i] = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	}
	depth_operation = stencil_operation = VK_ATTACHMENT_LOAD_OP_DONT_CARE;

	return *this;
}

syi::RenderPassCreationInfo& syi::RenderPassCreationInfo::add_attachment(VkFormat format, VkImageLayout layout,
                                                                         VkAttachmentLoadOp load_op)
{
	color_formats[num_render_targets] = format;
	color_operations[num_render_targets] = load_op;
	color_final_layouts[num_render_targets++] = layout;

	return *this;
}

syi::RenderPassCreationInfo& syi::RenderPassCreationInfo::set_depth_stencil_texture(VkFormat format,
                                                                                    VkImageLayout layout)
{
	depth_stencil_format = format;
	depth_stencil_final_layout = layout;

	return *this;
}

syi::RenderPassCreationInfo& syi::RenderPassCreationInfo::set_name(const std::string& name)
{
	this->name = name;

	return *this;
}

syi::RenderPassCreationInfo& syi::RenderPassCreationInfo::set_depth_stencil_operations(VkAttachmentLoadOp depth,
	VkAttachmentLoadOp stencil)
{
	depth_operation = depth;
	stencil_operation = stencil;

	return *this;
}

syi::FramebufferCreationInfo& syi::FramebufferCreationInfo::reset()
{
	num_render_targets = 0;
	name = nullptr;
	depth_stencil_texture.index = k_invalid_index;

	resize = 0;
	scale_x = 1.f;
	scale_y = 1.f;

	return *this;
}

syi::FramebufferCreationInfo& syi::FramebufferCreationInfo::add_render_texture(TextureHandle texture)
{
	output_textures[num_render_targets++] = texture;

	return *this;
}

syi::FramebufferCreationInfo& syi::FramebufferCreationInfo::set_depth_stencil_texture(TextureHandle texture)
{
	depth_stencil_texture = texture;

	return *this;
}

syi::FramebufferCreationInfo& syi::FramebufferCreationInfo::set_scaling(float scale_x, float scale_y, uint8_t resize)
{
	this->scale_x = scale_x;
	this->scale_y = scale_y;
	this->resize = resize;


	return *this;
}

syi::FramebufferCreationInfo& syi::FramebufferCreationInfo::set_name(const std::string& name)
{
	this->name = name;

	return *this;
}

syi::PipelineCreationInfo& syi::PipelineCreationInfo::add_descriptor_set_layout(DescriptorSetLayoutHandle handle)
{
	descriptor_set_layout[num_active_layouts++] = handle;

	return *this;
}

syi::RenderPassOutput& syi::PipelineCreationInfo::render_pass_output()
{
	return  render_pass;
}
