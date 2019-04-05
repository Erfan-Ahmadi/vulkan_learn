#include <iostream>
#include <stdexcept>
#include <functional>
#include <cstdlib>
#include <optional>

#ifdef _WIN32
#define VK_USE_PLATFORM_WIN32_KHR
#define NOMINMAX
#endif

#include <vulkan/vulkan.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define Log(str) std::cout << str << std::endl

struct QueueFamilyIndices {
	std::optional<uint32_t> graphics_family;
	std::optional<uint32_t> present_family;

	bool is_complete() {
		return graphics_family.has_value() && present_family.has_value();
	}
};

struct SwapChainSupportDetails
{
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> present_modes;
};

class VulkanApp
{
public:
	VulkanApp();
	~VulkanApp();

	bool run();

	bool release();

private:

	bool setup_window();

	bool setup_vulkan();

	bool create_instance();
	bool set_up_debug_messenger();
	bool pick_physical_device();
	QueueFamilyIndices find_queue_family_indices(VkPhysicalDevice device);

	bool check_device_extensions_support();
	bool create_logical_device();
	bool create_surface();
	bool create_swap_chain();
	bool create_image_views();
	bool create_graphics_pipeline();
	VkShaderModule create_shader_module(const std::vector<char>& code);

	bool main_loop();

	//	Vulkan
	VkInstance instance;
	VkPhysicalDevice physical_device;
	VkDevice  device;
	VkSurfaceKHR surface;
	QueueFamilyIndices indices;

	VkQueue graphics_queue;
	VkQueue present_queue;

	VkSwapchainKHR swap_chain;
	std::vector<VkImage> swap_chain_images;
	std::vector<VkImageView> swap_chain_image_views;
	VkFormat swap_chain_image_format;
	VkExtent2D swap_chain_extent;

	bool validation_layers_enabled;

#ifdef _DEBUG
	VkDebugUtilsMessengerEXT debug_messenger;
#endif

	//	Window
	GLFWwindow* window;

	bool is_released;
};