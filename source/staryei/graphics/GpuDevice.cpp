#include "graphics/GpuDevice.hpp"
#include "graphics/CommandBuffer.hpp"
#include "graphics/SpirvParser.hpp"

#include "foundation/memory.hpp"
#include "foundation/hash_map.hpp"
#include "foundation/process.hpp"
#include "foundation/file.hpp"

template<class T>
constexpr const T& syi_min(const T& a, const T& b) {
    return (a < b) ? a : b;
}

template<class T>
constexpr const T& syi_max(const T& a, const T& b) {
    return (a < b) ? b : a;
}

#define VMA_MAX syi_max
#define VMA_MIN syi_min
#define VMA_USE_STL_CONTAINERS 0
#define VMA_USE_STL_VECTOR 0
#define VMA_USE_STL_UNORDERED_MAP 0
#define VMA_USE_STL_LIST 0

#if defined (_MSC_VER)
#pragma warning (disable: 4127)
#pragma warning (disable: 4189)
#pragma warning (disable: 4191)
#pragma warning (disable: 4296)
#pragma warning (disable: 4324)
#pragma warning (disable: 4355)
#pragma warning (disable: 4365)
#pragma warning (disable: 4625)
#pragma warning (disable: 4626)
#pragma warning (disable: 4668)
#pragma warning (disable: 5026)
#pragma warning (disable: 5027)
#endif // _MSC_VER

//#define VMA_DEBUG_LOG rprintret

#define VMA_IMPLEMENTATION
#include "external/vk_mem_alloc.h"

// SDL and Vulkan headers
#include <SDL.h>
#include <SDL_vulkan.h>
#include <unordered_map>

namespace syi
{
	static void checkResult(VkResult result);
	#define check(result) RASSERTM( result == VK_SUCCESS, "vk assert code %u", result);
	// Device implementation //////////////////////////////////////////////////

	// Methods //////////////////////////////////////////////////////////////////////

// Enable this to add debugging capabilities.
// https://www.khronos.org/registry/vulkan/specs/1.2-extensions/man/html/VK_EXT_debug_utils.html
#define VULKAN_DEBUG_REPORT

//#define VULKAN_SYNCHRONIZATION_VALIDATION

static const char* s_requested_extensions[] = {
    VK_KHR_SURFACE_EXTENSION_NAME,
    // Platform specific extension
#ifdef VK_USE_PLATFORM_WIN32_KHR
        VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
#elif defined(VK_USE_PLATFORM_MACOS_MVK)
        VK_MVK_MACOS_SURFACE_EXTENSION_NAME,
#elif defined(VK_USE_PLATFORM_XCB_KHR)
        VK_KHR_XCB_SURFACE_EXTENSION_NAME,
#elif defined(VK_USE_PLATFORM_ANDROID_KHR)
        VK_KHR_ANDROID_SURFACE_EXTENSION_NAME,
#elif defined(VK_USE_PLATFORM_XLIB_KHR)
        VK_KHR_XLIB_SURFACE_EXTENSION_NAME,
#elif defined(VK_USE_PLATFORM_XCB_KHR)
        VK_KHR_XCB_SURFACE_EXTENSION_NAME,
#elif defined(VK_USE_PLATFORM_WAYLAND_KHR)
        VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME,
#elif defined(VK_USE_PLATFORM_MIR_KHR || VK_USE_PLATFORM_DISPLAY_KHR)
        VK_KHR_DISPLAY_EXTENSION_NAME,
#elif defined(VK_USE_PLATFORM_ANDROID_KHR)
        VK_KHR_ANDROID_SURFACE_EXTENSION_NAME,
#elif defined(VK_USE_PLATFORM_IOS_MVK)
        VK_MVK_IOS_SURFACE_EXTENSION_NAME,
#endif // VK_USE_PLATFORM_WIN32_KHR

#if defined (VULKAN_DEBUG_REPORT)
    VK_EXT_DEBUG_REPORT_EXTENSION_NAME,
    VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
#endif // VULKAN_DEBUG_REPORT
};

static const char* s_requested_layers[] = {
	#if defined (VULKAN_DEBUG_REPORT)
	    "VK_LAYER_KHRONOS_validation",
	    //"VK_LAYER_LUNARG_core_validation",
	    //"VK_LAYER_LUNARG_image",
	    //"VK_LAYER_LUNARG_parameter_validation",
	    //"VK_LAYER_LUNARG_object_tracker"
	#else
	    "",
	#endif // VULKAN_DEBUG_REPORT
};

#ifdef VULKAN_DEBUG_REPORT
    static VkBool32 debug_utils_callback(VkDebugUtilsMessageSeverityFlagBitsEXT severity,
        VkDebugUtilsMessageTypeFlagsEXT types,
        const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
        void* user_data) {
        bool triggerBreak = severity & (VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT);

        if (triggerBreak) {
            // __debugbreak();
        }

        rprint(" MessageID: %s %i\nMessage: %s\n\n", callback_data->pMessageIdName, callback_data->messageIdNumber, callback_data->pMessage);
        return VK_FALSE;
    }
#endif // VULKAN_DEBUG_REPORT

    static SDL_Window* sdl_window;

    PFN_vkSetDebugUtilsObjectNameEXT    pfnSetDebugUtilsObjectNameEXT;
    PFN_vkCmdBeginDebugUtilsLabelEXT    pfnCmdBeginDebugUtilsLabelEXT;
    PFN_vkCmdEndDebugUtilsLabelEXT      pfnCmdEndDebugUtilsLabelEXT;

    static std::unordered_map<u64, VkRenderPass> render_pass_cache;
    static CommandBufferManager command_buffer_ring;

    static const u32        k_bindless_texture_binding = 10;
    static const u32        k_max_bindless_resources = 1024;

}
