#include <iostream>
#include <stdexcept>
#include <functional>
#include <cstdlib>
#include <optional>
#include <vulkan/vulkan.h>

#define Log(str) std::cout << str << std::endl
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

struct QueueFamilyIndices {
	std::optional<uint32_t> graphics_family;

	bool is_complete() {
		return graphics_family.has_value();
	}
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

	bool create_logical_device();

	bool main_loop();

	//	Vulkan
	VkInstance instance;
	VkPhysicalDevice physical_device;
	VkDevice  device;
	bool validation_layers_enabled;

#ifdef _DEBUG
	VkDebugUtilsMessengerEXT debug_messenger;
#endif

	//	Window
	GLFWwindow* window;

	bool is_released;
};