#include "vulkan_app.h"

#include <iostream>
#include <stdexcept>
#include <algorithm>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <optional>
#include <set>

constexpr int WIDTH = 800;
constexpr int HEIGHT = 600;

const std::vector<const char*> required_validation_layers =
{
	"VK_LAYER_LUNARG_standard_validation"
};

const std::vector<const char*> device_extensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(
	VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageType,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData) {

	std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

	return VK_FALSE;
}

VulkanApp::VulkanApp() :
	is_released(false),
	validation_layers_enabled(false),
	physical_device(VK_NULL_HANDLE)
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
	if (!set_up_debug_messenger())
		return false;
	if (!create_surface())
		return false;
	if (!pick_physical_device())
		return false;
	if (!create_logical_device())
		return false;
	if (!create_swap_chain())
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

#pragma region validation layers

#ifdef _DEBUG
	uint32_t available_layer_count;
	vkEnumerateInstanceLayerProperties(&available_layer_count, nullptr);
	std::vector<VkLayerProperties> available_layers(available_layer_count);
	vkEnumerateInstanceLayerProperties(&available_layer_count, available_layers.data());

	// check if extentions required by glfw is available
	if (required_validation_layers.size() > available_layer_count)
	{
		Log("Validation Layers Not Supported");
		return false;
	}

	for (auto i = 0; i < required_validation_layers.size(); ++i)
	{
		bool found_layer = false;

		for (const auto& extension : available_layers)
		{
			if (strcmp(extension.layerName, required_validation_layers[i]) == 0)
				found_layer = true;
		}

		if (!found_layer)
		{
			Log("Validation Layer << " << required_validation_layers[i] << " >> is not available");
			return false;
		}
	}

	this->validation_layers_enabled = true;
#else
	this->validation_layers_enabled = false;
#endif


#pragma endregion

#pragma region extention
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

	std::vector<const char*> required_extentions(glfw_extensions, glfw_extensions + glfw_extensions_count);

	if (validation_layers_enabled)
		required_extentions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

	// check if extentions required is available
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
			if (strcmp(extension.extensionName, glfw_extensions[i]) == 0)
				found_extention = true;
		}

		if (!found_extention)
		{
			Log("Extention << " << glfw_extensions[i] << " >> for glfw is not available");
			return false;
		}
	}
#pragma endregion

	VkInstanceCreateInfo create_info = {};
	create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	create_info.pApplicationInfo = &app_info;
	create_info.enabledExtensionCount = static_cast<uint32_t>(required_extentions.size());
	create_info.ppEnabledExtensionNames = required_extentions.data();
	// Validation layer 
	if (validation_layers_enabled)
	{
		create_info.enabledLayerCount = static_cast<uint32_t>(required_validation_layers.size());
		create_info.ppEnabledLayerNames = required_validation_layers.data();
	}
	else
	{
		create_info.enabledLayerCount = 0;
	}

	VkResult result = vkCreateInstance(&create_info, nullptr, &(this->instance));

	return result == VK_SUCCESS;
}

bool VulkanApp::set_up_debug_messenger()
{
	if (!validation_layers_enabled)
		return false;

	VkDebugUtilsMessengerCreateInfoEXT create_info = {};
	create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;

	create_info.messageSeverity =
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;

	create_info.messageType =
		VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;

	create_info.pfnUserCallback = debug_callback;
	create_info.pUserData = nullptr;

	auto CreateDebugUtilsMessengerEXT = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");

	if (CreateDebugUtilsMessengerEXT != nullptr)
	{
		const auto result = CreateDebugUtilsMessengerEXT(this->instance, &create_info, nullptr, &this->debug_messenger);
		return result == VK_SUCCESS;
	}
	else
	{
		return false;
	}
}

bool VulkanApp::pick_physical_device()
{
	uint32_t available_physical_devices_count = 0;
	vkEnumeratePhysicalDevices(this->instance, &available_physical_devices_count, nullptr);

	if (available_physical_devices_count == 0)
	{
		Log("No Supported Vulkan GPU found.");
		return false;
	}

	std::vector<VkPhysicalDevice> available_physical_devices(available_physical_devices_count);
	vkEnumeratePhysicalDevices(this->instance, &available_physical_devices_count, available_physical_devices.data());

	auto selected_device = -1;

	for (auto i = 0; i < available_physical_devices_count; ++i)
	{
		const auto device = available_physical_devices[i];
		VkPhysicalDeviceProperties device_properties;
		VkPhysicalDeviceFeatures device_features;
		vkGetPhysicalDeviceProperties(device, &device_properties);
		vkGetPhysicalDeviceFeatures(device, &device_features);

		//I just skip here since i only have Intel's GPU on Surface Pro 6 
		if (find_queue_family_indices(device).is_complete())
		{
			selected_device = i;
			break;
		}
	}

	if (selected_device < 0)
	{
		Log("No Suitable Physical Device Found.");
		return false;
	}

	this->physical_device = available_physical_devices[selected_device];

	return true;
}

QueueFamilyIndices VulkanApp::find_queue_family_indices(VkPhysicalDevice device)
{
	QueueFamilyIndices indices;

	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queue_families(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queue_families.data());


	for (auto i = 0; i < queue_families.size(); ++i)
	{
		const auto& queue_familiy = queue_families[i];

		if (queue_familiy.queueCount > 0 && (queue_familiy.queueFlags & VK_QUEUE_GRAPHICS_BIT))
		{
			indices.graphics_family = i;
		}

		VkBool32 present_support = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &present_support);

		if (queue_familiy.queueCount > 0 && present_support)
			indices.present_family = i;

		if (indices.is_complete())
			break;
	}

	return indices;
}

bool VulkanApp::create_logical_device()
{
	if (!check_device_extentions_support())
		return false;

	this->indices = find_queue_family_indices(this->physical_device);
	std::set<uint32_t> unique_queue_families = { indices.graphics_family.value(), indices.present_family.value() };

	std::vector<VkDeviceQueueCreateInfo> queue_create_infos;

	float queue_priorities[1] = { 1.0f };

	for (auto queue_family : unique_queue_families)
	{
		VkDeviceQueueCreateInfo queue_create_info = {};
		queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queue_create_info.queueCount = 1;
		queue_create_info.queueFamilyIndex = queue_family;
		queue_create_info.pQueuePriorities = queue_priorities;
		queue_create_infos.push_back(queue_create_info);
	}

	// nothing for now
	VkPhysicalDeviceFeatures device_features = {};

	VkDeviceCreateInfo  create_info = {};
	create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	create_info.queueCreateInfoCount = static_cast<uint32_t>(queue_create_infos.size());
	create_info.pQueueCreateInfos = queue_create_infos.data();
	create_info.pEnabledFeatures = &device_features;
	create_info.enabledExtensionCount = static_cast<uint32_t>(device_extensions.size());
	create_info.ppEnabledExtensionNames = device_extensions.data();

	if (validation_layers_enabled)
	{
		create_info.enabledLayerCount = static_cast<uint32_t>(required_validation_layers.size());
		create_info.ppEnabledLayerNames = required_validation_layers.data();
	}
	else
	{
		create_info.enabledLayerCount = 0;
	}

	auto result = vkCreateDevice(this->physical_device, &create_info, nullptr, &this->device);

	vkGetDeviceQueue(device, indices.graphics_family.value(), 0, &graphics_queue);
	vkGetDeviceQueue(device, indices.present_family.value(), 0, &present_queue);

	return result == VK_SUCCESS;
}

bool VulkanApp::create_surface()
{
	return glfwCreateWindowSurface(this->instance, this->window, nullptr, &this->surface) == VK_SUCCESS;
}

bool VulkanApp::create_swap_chain()
{
	// Get Properties

	SwapChainSupportDetails properties;

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(this->physical_device, this->surface, &properties.capabilities);

	uint32_t format_count = 0;
	vkGetPhysicalDeviceSurfaceFormatsKHR(this->physical_device, this->surface, &format_count, nullptr);

	if (format_count > 0)
	{
		properties.formats.resize(format_count);
		vkGetPhysicalDeviceSurfaceFormatsKHR(this->physical_device, this->surface, &format_count, properties.formats.data());
	}

	uint32_t present_modes_count = 0;
	vkGetPhysicalDeviceSurfacePresentModesKHR(this->physical_device, this->surface, &present_modes_count, nullptr);

	if (present_modes_count > 0)
	{
		properties.present_modes.resize(present_modes_count);
		vkGetPhysicalDeviceSurfacePresentModesKHR(this->physical_device, this->surface, &format_count, properties.present_modes.data());
	}

	// Choose Best Config

	if (properties.capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
	{
		this->swap_chain_extent = properties.capabilities.currentExtent;
	}
	else
	{
		this->swap_chain_extent = { WIDTH, HEIGHT };
		this->swap_chain_extent.width = std::max(properties.capabilities.maxImageExtent.width, std::min(this->swap_chain_extent.width, properties.capabilities.minImageExtent.width));
		this->swap_chain_extent.height = std::max(properties.capabilities.maxImageExtent.height, std::min(this->swap_chain_extent.height, properties.capabilities.minImageExtent.height));
	}

	if (properties.formats.empty())
	{
		Log("No Format Exists for this Physical Device");
		return false;
	}


	VkSurfaceFormatKHR selected_format = properties.formats[0];

	if (selected_format.format == VK_FORMAT_UNDEFINED)
	{
		// Best Case
		selected_format = { VK_FORMAT_B8G8R8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
	}
	else
	{
		for (const auto& format : properties.formats)
		{
			if (format.format == VK_FORMAT_B8G8R8_UNORM && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			{
				selected_format = format;
				break;
			}
		}
	}

	this->swap_chain_image_format = selected_format.format;



	VkPresentModeKHR selected_present_mode = VK_PRESENT_MODE_FIFO_KHR;

	for (const auto& available_present_mode : properties.present_modes)
	{
		if (available_present_mode == VK_PRESENT_MODE_MAILBOX_KHR)
		{
			selected_present_mode = available_present_mode;
			break;
		}
		else if (available_present_mode == VK_PRESENT_MODE_IMMEDIATE_KHR)
		{
			selected_present_mode = available_present_mode;
		}
	}

	uint32_t min_image_count = properties.capabilities.minImageCount + 1;
	if (min_image_count > properties.capabilities.maxImageCount)
		min_image_count = properties.capabilities.maxImageCount;

	// Create Swap Chain
	VkSwapchainCreateInfoKHR create_info = {};
	create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	create_info.imageExtent = this->swap_chain_extent;
	create_info.imageArrayLayers = 1;
	create_info.imageFormat = selected_format.format;
	create_info.imageColorSpace = selected_format.colorSpace;
	create_info.presentMode = selected_present_mode;
	create_info.minImageCount = min_image_count;
	create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	uint32_t queue_family_indices[] = { this->indices.graphics_family.value(), indices.present_family.value() };

	if (this->indices.graphics_family != indices.present_family)
	{
		create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		create_info.queueFamilyIndexCount = 2;
		create_info.pQueueFamilyIndices = queue_family_indices;
	}
	else
	{
		create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		create_info.queueFamilyIndexCount = 0;
		create_info.pQueueFamilyIndices = 0;
	}

	create_info.preTransform = properties.capabilities.currentTransform;
	create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	create_info.clipped = VK_TRUE;
	create_info.oldSwapchain = VK_NULL_HANDLE;

	if (vkCreateSwapchainKHR(this->device, &create_info, nullptr, &this->swap_chain) != VK_SUCCESS)
		return false;

	uint32_t images_count;
	vkGetSwapchainImagesKHR(device, this->swap_chain, &images_count, nullptr);
	this->swap_chain_images.resize(images_count);
	vkGetSwapchainImagesKHR(device, this->swap_chain, &images_count, this->swap_chain_images.data());

	return true;
}

bool VulkanApp::check_device_extentions_support()
{
	uint32_t available_extensions_count;
	vkEnumerateDeviceExtensionProperties(this->physical_device, nullptr, &available_extensions_count, nullptr);
	std::vector<VkExtensionProperties> available_extensions(available_extensions_count);
	vkEnumerateDeviceExtensionProperties(this->physical_device, nullptr, &available_extensions_count, available_extensions.data());

	std::set<std::string> required_extensions(device_extensions.begin(), device_extensions.end());

	for (auto& extension : available_extensions)
		required_extensions.erase(extension.extensionName);

	return required_extensions.empty();
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

	auto DestroyDebugUtilsMessengerEXT = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");

	if (DestroyDebugUtilsMessengerEXT != nullptr)
	{
		DestroyDebugUtilsMessengerEXT(this->instance, this->debug_messenger, nullptr);
	}

	vkDestroySwapchainKHR(this->device, this->swap_chain, nullptr);
	vkDestroySurfaceKHR(this->instance, this->surface, nullptr);
	vkDestroyInstance(this->instance, nullptr);
	vkDestroyDevice(this->device, nullptr);

	glfwDestroyWindow(this->window);
	glfwTerminate();

	is_released = true;
	return true;
}
