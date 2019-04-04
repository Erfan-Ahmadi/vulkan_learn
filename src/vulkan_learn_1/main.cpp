#include "vulkan_app.h"

int main()
{
	VulkanApp app;

	if (!app.run())
		return EXIT_FAILURE;

	return EXIT_SUCCESS;
}