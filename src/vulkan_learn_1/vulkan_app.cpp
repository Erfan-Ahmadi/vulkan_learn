#include "vulkan_app.h"

constexpr int WIDTH = 800;
constexpr int HEIGHT = 600;

VulkanApp::VulkanApp() : 
	is_released(false)
{
}

VulkanApp::~VulkanApp()
{
	release();
}

bool VulkanApp::run()
{
	if (!setup_window())
		return false;

	if (!setup_vulkan())
		return false;

	if (!main_loop())
		return false;

	if (!release())
		return false;

	return true;
}

bool VulkanApp::setup_window()
{
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	this->window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan-Learn-1", nullptr, nullptr);

	return true;
}

bool VulkanApp::setup_vulkan()
{
	if (!create_instance())
		return false;

	return true;
}

bool VulkanApp::create_instance()
{
	VkApplicationInfo app_info = {};
	app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	app_info.pApplicationName = "vk-1";
	app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	app_info.pEngineName = "Mir Engine®";
	app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	app_info.apiVersion = VK_API_VERSION_1_0;


	// Ok Let's Do It :(
	uint32_t glfw_extentions_count = 0;
	const char** glfw_extentions;

	// Probably : VK_KHR_Surface(?) and VkSwapchainKHR(vsync)
	glfw_extentions = glfwGetRequiredInstanceExtensions(&glfw_extentions_count);

	VkInstanceCreateInfo create_info = {};
	create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	create_info.pApplicationInfo = &app_info;
	create_info.enabledExtensionCount = glfw_extentions_count;
	create_info.ppEnabledLayerNames = glfw_extentions;
	// Validation layer 
	create_info.enabledLayerCount = 0;

	VkResult result = vkCreateInstance(&create_info, nullptr, &this->instance);

	return result == VK_SUCCESS;
}

bool VulkanApp::main_loop()
{
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
	}

	return true;
}

bool VulkanApp::release()
{
	if (is_released)
		return true;

	glfwDestroyWindow(window);
	glfwTerminate();

	is_released = true;
	return true;
}
