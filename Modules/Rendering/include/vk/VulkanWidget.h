#pragma once

// TODO :: Handle Which Platform to Use Upon Build System Creation
#ifdef _WIN32
#define VK_USE_PLATFORM_WIN32_KHR
#endif

#include "qwidget.h"
#include "vulkan/vulkan.h" // TODO :: abstract away public vulkan requirements here ... 

// This class Requires the user to Manually Call the Initialize and Cleanup Vulkan Resources
class VulkanWidget : public QWidget
{
	Q_OBJECT

public:

	VulkanWidget();
	virtual ~VulkanWidget();

	void InitializeVulkan();
	void CleanupVulkan();

private:

	void createVKInstance();
	void pickPhysicalDevice();
	void createLogicalDevice();
	void createSurface();
	void createSwapChain();
	void createImageViews();
	void createGraphicsPipeline();

	VkInstance m_VkInstance = VK_NULL_HANDLE;
	VkDebugUtilsMessengerEXT m_VkDebugMessenger = VK_NULL_HANDLE;
	VkPhysicalDevice m_VkPhysicalDevice = VK_NULL_HANDLE;
	VkDevice m_VkDevice = VK_NULL_HANDLE;
	VkQueue m_VkGraphicsQueue = VK_NULL_HANDLE;
	VkQueue m_VkPresentQueue = VK_NULL_HANDLE;
	VkSurfaceKHR m_VkSurface = VK_NULL_HANDLE;

	VkSwapchainKHR m_VkSwapChain = VK_NULL_HANDLE;
	std::vector<VkImage> m_VkSwapChainImages;
	VkFormat m_VkSwapChainFormat;
	VkExtent2D m_VkSwapChainExtent;
	std::vector<VkImageView> m_VkSwapChainImageViews;

};