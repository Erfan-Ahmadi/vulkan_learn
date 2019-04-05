#include <iostream>
#include <stdexcept>
#include <functional>
#include <cstdlib>

#include <vulkan/vulkan.h>

#define Log(str) std::cout << str << std::endl
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

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

	bool main_loop();

	//	Vulkan
	VkInstance instance;
	VkPhysicalDevice physical_device;
	bool validation_layers_enabled;

#ifdef _DEBUG
	VkDebugUtilsMessengerEXT debug_messenger;
#endif

	//	Window
	GLFWwindow* window;

	bool is_released;
};