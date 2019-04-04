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
	return true;
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
