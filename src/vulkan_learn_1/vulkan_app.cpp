#include "vulkan_app.h"
#include <vector>

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
	app_info.pEngineName = "Mir Engine�";
	app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	app_info.apiVersion = VK_API_VERSION_1_0;

	// check available extentions
	uint32_t available_extention_count = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &available_extention_count, nullptr);
	std::vector<VkExtensionProperties> available_extensions(available_extention_count);
	vkEnumerateInstanceExtensionProperties(nullptr, &available_extention_count, available_extensions.data());

	Log("available extensions" << "(" << available_extention_count << ") : ");

	for (const auto& extension : available_extensions) {
		Log("\t" << extension.extensionName);
	}

	// Ok Let's Do It :(
	uint32_t glfw_extensions_count = 0;
	const char** glfw_extensions;

	glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extensions_count);

	// check if extentions required by glfw is available
	if (glfw_extensions_count > available_extention_count)
	{
		Log("Extentions for glfw are not available");
		return false;
	}

	for (auto i = 0; i < glfw_extensions_count; ++i)
	{
		bool found_extention = false;

		for (const auto& extension : available_extensions) 
		{
			if (extension.extensionName == std::string(glfw_extensions[i]))
				found_extention = true;
		}

		if (!found_extention)
		{
			Log("Extention << " << glfw_extensions[i] << " >> for glfw is not available");
			return false;
		}
	}

	VkInstanceCreateInfo create_info = {};
	create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	create_info.pApplicationInfo = &app_info;
	create_info.enabledExtensionCount = glfw_extensions_count;
	create_info.ppEnabledExtensionNames = glfw_extensions;
	// Validation layer 
	create_info.enabledLayerCount = 0;

	VkResult result = vkCreateInstance(&create_info, nullptr, &(this->instance));

	return result == VK_SUCCESS;
}

bool VulkanApp::main_loop()
{
	while (!glfwWindowShouldClose(this->window))
	{
		glfwPollEvents();
	}

	return true;
}

bool VulkanApp::release()
{
	if (is_released)
		return true;

	vkDestroyInstance(this->instance, nullptr);

	glfwDestroyWindow(this->window);
	glfwTerminate();

	is_released = true;
	return true;
}
