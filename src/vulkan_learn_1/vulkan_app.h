#include <iostream>
#include <stdexcept>
#include <functional>
#include <cstdlib>

#include <vulkan/vulkan.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

class VulkanApp
{
public:
	VulkanApp();
	~VulkanApp();

	bool run();

	bool setup_window();
	bool setup_vulkan();
	bool main_loop();
	bool release();

private:
	GLFWwindow* window;
	bool is_released;
};