#include <functional>
#include <optional>
#include <stdio.h>
#include <array>
#include <glm/glm.hpp>

#ifdef _WIN32
#include <direct.h>
#define GetCurrentDir _getcwd
#endif

#ifdef _WIN32
#define VK_USE_PLATFORM_WIN32_KHR
#define NOMINMAX
#endif

#include <vulkan/vulkan.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define Log(str) std::cout << str << std::endl

struct Vertex
{
	glm::vec2 pos;
	glm::vec3 color;

	static VkVertexInputBindingDescription getBindingDesc()
	{
		VkVertexInputBindingDescription bindingDesc = {};

		bindingDesc.binding = 0;
		bindingDesc.stride = sizeof(Vertex);
		bindingDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		return bindingDesc;
	}

	static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions() 
	{
		std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions = {};

		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(Vertex, pos);

		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(Vertex, color);

		return attributeDescriptions;
	}
};

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

	void window_resize();

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
	bool create_renderpass();
	bool create_graphics_pipeline();
	bool create_vertex_buffer();
	bool create_index_buffer();
	bool create_frame_buffers();
	bool create_command_pool();
	bool create_command_buffers();
	bool create_sync_objects();

	bool cleanup_swap_chain();
	bool recreate_swap_chain();
	bool set_viewport_scissor();

	bool create_buffer(
		VkDeviceSize buffer_size,
		VkBufferUsageFlags usage,
		VkMemoryPropertyFlags memory_properties,
		VkBuffer& buffer,
		VkDeviceMemory& buffer_memory);

	bool copy_buffer(
		VkBuffer& src_buffer,
		VkBuffer& dst_buffer,
		VkDeviceSize buffer_size);

	bool draw_frame();

	bool main_loop();

	VkShaderModule create_shader_module(const std::vector<char>& code);
	uint32_t find_memory_type(uint32_t type_filter, VkMemoryPropertyFlags properties);

	bool should_recreate_swapchain;

	//	Vulkan
	VkInstance instance;
	VkPhysicalDevice physical_device;
	VkDevice  device;
	VkSurfaceKHR surface;
	QueueFamilyIndices family_indices;

	VkQueue graphics_queue;
	VkQueue present_queue;

	VkSwapchainKHR swap_chain;
	std::vector<VkImage> swap_chain_images;
	std::vector<VkImageView> swap_chain_image_views;
	std::vector<VkFramebuffer> swap_chain_frame_buffers;
	VkFormat swap_chain_image_format;
	VkExtent2D swap_chain_extent;

	VkRenderPass render_pass;
	VkPipelineLayout pipeline_layout;
	VkPipeline graphics_pipeline;

	VkViewport viewport;
	VkRect2D scissor;

	VkBuffer vertex_buffer;
	VkDeviceMemory vertex_buffer_memory;
	VkBuffer index_buffer;
	VkDeviceMemory index_buffer_memory;

	VkCommandPool command_pool;
	std::vector<VkCommandBuffer> command_buffers;

	std::vector<VkSemaphore> image_available_semaphore;
	std::vector<VkSemaphore> render_finished_semaphore;
	std::vector<VkFence> draw_fences;

	size_t num_frames;
	size_t current_frame;

	bool validation_layers_enabled;

#ifdef _DEBUG
	VkDebugUtilsMessengerEXT debug_messenger;
#endif

	std::string app_path;

	//	Window
	GLFWwindow* window;

	bool is_released;
};