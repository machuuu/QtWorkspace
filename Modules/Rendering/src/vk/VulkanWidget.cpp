#include "VulkanWidget.h"
#include <Logger.h>
#include <set>
#include <limits>

const std::vector<const char*> validationLayers = {
	"VK_LAYER_KHRONOS_validation"
};

const std::vector<const char*> deviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

#ifdef _DEBUG
const bool enableValidationLayers = true;
#else
const bool enableValidationLayers = false;
#endif // _DEBUG

namespace {
	bool checkValidationLayerSupport(VkInstanceCreateInfo* createInfo)
	{
		if (createInfo == VK_NULL_HANDLE) {
			qDebug() << "Create Info argument was null";
			return false;
		}

		qDebug() << "Querying Number of Available Validation Layers";
		VkResult result;
		uint32_t layerCount;
		result = vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
		if (result != VK_SUCCESS) {
			return false;
		}

		std::vector<VkLayerProperties> availableLayers(layerCount);
		result = vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());
		if (result != VK_SUCCESS) {
			return false;
		}

		for (const char* layerName : validationLayers) {
			bool layerFound = false;
			for (const auto& layerProperties : availableLayers) {
				if (strcmp(layerName, layerProperties.layerName) == 0) {
					qDebug() << "Found Layer:" << layerName; 
					layerFound = true;
					break;
				}
			}

			if (!layerFound) {
				return false;
			}
		}

		createInfo->enabledLayerCount = (uint32_t)validationLayers.size();
		createInfo->ppEnabledLayerNames = validationLayers.data();

		return true;
	}
	
	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData)
	{
		qDebug() << "Validation Layer:" << messageSeverity << pCallbackData->pMessage;
		return VK_FALSE;
	}

	void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT* createInfo) {
		if (createInfo == VK_NULL_HANDLE) {
			qDebug() << "Create Info argument was null";
			return;
		}
		createInfo->sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		createInfo->messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		createInfo->messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		createInfo->pfnUserCallback = debugCallback;
	}

	VkResult createDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
		if (instance == VK_NULL_HANDLE) {
			qDebug() << "Instance argument was null";
			return VK_ERROR_UNKNOWN;
		}
		if (pCreateInfo == VK_NULL_HANDLE) {
			qDebug() << "Create Info argument was null";
			return VK_ERROR_UNKNOWN;
		}
		if (pDebugMessenger == VK_NULL_HANDLE) {
			qDebug() << "Debug Messenger argument was null";
			return VK_ERROR_UNKNOWN;
		}		
		auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
		if (func != nullptr) {
			return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
		}
		else {
			return VK_ERROR_EXTENSION_NOT_PRESENT;
		}
	}

	void destroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
		if (instance == VK_NULL_HANDLE) {
			qDebug() << "Instance argument was null";
			return;
		}
		if (debugMessenger == VK_NULL_HANDLE) {
			qDebug() << "Debug Messenger argument was null";
			return;
		}		
		auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
		if (func != nullptr) {
			func(instance, debugMessenger, pAllocator);
		}
	}

	int rateDeviceSuitability(VkPhysicalDevice physicalDevice)
	{
		if (physicalDevice == VK_NULL_HANDLE) {
			qDebug() << "physicalDevice argument was null. Returning -1";
			return -1;
		}

		VkPhysicalDeviceProperties deviceProperties;
		VkPhysicalDeviceFeatures deviceFeatures;
		vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);
		vkGetPhysicalDeviceFeatures(physicalDevice, &deviceFeatures);

		int score = 0;

		// Discrete GPUs have a significant performance advantage
		if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
			score += 1000;
		}

		// Maximum possible size of textures affects graphics quality
		score += deviceProperties.limits.maxImageDimension2D;

		// Application can't function without geometry shaders
		if (!deviceFeatures.geometryShader) {
			return 0;
		}

		return score;
	}

	struct QueueFamilyIndices 
	{
		std::optional<uint32_t> graphicsFamily;
		std::optional<uint32_t> presentFamily;
		
		bool isComplete() 
		{
			return graphicsFamily.has_value() && presentFamily.has_value();
		}
	};

	struct SwapChainSupportDetails {
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	};

	SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface)
	{
		SwapChainSupportDetails details;

		// input checking ...

		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

		uint32_t formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

		if (formatCount != 0) {
			details.formats.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
		}

		uint32_t presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

		if (presentModeCount != 0) {
			details.presentModes.resize(presentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
		}

		return details;
	}

	// to do:: do i need surface here?
	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface)
	{		
		QueueFamilyIndices indices;

		if (device == VK_NULL_HANDLE) {
			qDebug() << "Physical Device argument was null";
			return indices;
		}
		if (surface == VK_NULL_HANDLE) {
			qDebug() << "Surface argument was null";
			return indices;
		}

		qDebug() << "Querying Number of Queue Families";
		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
		qDebug() << "There are:" << queueFamilyCount << "Queue Families";

		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

		int i = 0;
		for (const auto& queueFamily : queueFamilies) {
			qDebug() << "Queue Family Idx:" << i;
			qDebug() << "Queue count:" << std::to_string(queueFamily.queueCount).c_str();
			qDebug() << "Queue flags:" << std::to_string(queueFamily.queueFlags).c_str();
			if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
				qDebug() << "Found Graphics Flag";
 				indices.graphicsFamily = i;
			}
			if (queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT) {
				qDebug() << "Found Compute Flag";
			}
			if (queueFamily.queueFlags & VK_QUEUE_TRANSFER_BIT) {
				qDebug() << "Found Transfer Flag";
			}
			if (queueFamily.queueFlags & VK_QUEUE_SPARSE_BINDING_BIT) {
				qDebug() << "Found Sparse Binding Flag";
			}

			VkBool32 presentSupport = VK_FALSE;
			vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

			if (static_cast<bool>(presentSupport) == true) {
				indices.presentFamily = i;
			}

			if (indices.isComplete()) {
				qDebug() << "Device has a queue family that supports graphics." << "Picking Idx:" << i;
				break;
			}
			i++;
		}

		return indices;
	}

	bool checkDeviceExtensionSupport(VkPhysicalDevice physicalDevice) {
		
		if (physicalDevice == VK_NULL_HANDLE) {
			qDebug() << "Physical Device argument was null";
			return false;
		}

		uint32_t extensionCount;
		vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, nullptr);

		std::vector<VkExtensionProperties> availableExtensions(extensionCount);
		vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, availableExtensions.data());

		std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

		for (const auto& extension : availableExtensions) {
			requiredExtensions.erase(extension.extensionName);
		}
		 
		return requiredExtensions.empty();
	}

	bool isDeviceSuitable(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface)
	{
		QueueFamilyIndices indices = findQueueFamilies(physicalDevice, surface);
		
		bool extensionSupported = checkDeviceExtensionSupport(physicalDevice);

		bool swapChainAdequate = false;
		if (extensionSupported) {
			SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physicalDevice, surface);
			swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
		}

		return indices.isComplete() && extensionSupported && swapChainAdequate;
	}

	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
	{
		// check if greater than 0

		for (const auto& availableFormat : availableFormats) {
			if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
				return availableFormat;
			}
		}

		return availableFormats[0];
	}

	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
	{
		for (const auto& availablePresentMode : availablePresentModes)
		{
			if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
				return availablePresentMode;
			}
		}
		return VK_PRESENT_MODE_FIFO_KHR;
	}

	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, const QWidget* widget) 
	{
		if (capabilities.currentExtent.width != (std::numeric_limits<uint32_t>::max)())
		{
			return capabilities.currentExtent;
		}
		else
		{
			int width = widget->width();
			int height = widget->height();

			VkExtent2D actualExtent = {
				static_cast<uint32_t>(width),
				static_cast<uint32_t>(height)
			};

			actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
			actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

			return actualExtent;
		}
	}
}

VulkanWidget::VulkanWidget()
{

}

VulkanWidget::~VulkanWidget()
{

}

void VulkanWidget::InitializeVulkan()
{
	createVKInstance();
	createSurface();
	pickPhysicalDevice();
	createLogicalDevice();
	createSwapChain();
	createImageViews();
	createGraphicsPipeline();
}

void VulkanWidget::CleanupVulkan()
{
	for (auto imageView : m_VkSwapChainImageViews) {
		vkDestroyImageView(m_VkDevice, imageView, nullptr);
	}
	
	if (m_VkSwapChain != VK_NULL_HANDLE) {
		vkDestroySwapchainKHR(m_VkDevice, m_VkSwapChain, nullptr);
		m_VkSwapChain = VK_NULL_HANDLE;
	}
	
	if (m_VkDevice != VK_NULL_HANDLE) {
		vkDestroyDevice(m_VkDevice, nullptr);
		m_VkDevice = VK_NULL_HANDLE;
	}
	
	if (enableValidationLayers &&
		m_VkInstance != VK_NULL_HANDLE &&
		m_VkDebugMessenger != VK_NULL_HANDLE)
	{
		destroyDebugUtilsMessengerEXT(m_VkInstance, m_VkDebugMessenger, nullptr);
		m_VkDebugMessenger = VK_NULL_HANDLE;
	}

	if (m_VkSurface != VK_NULL_HANDLE) {
		vkDestroySurfaceKHR(m_VkInstance, m_VkSurface, nullptr);
		m_VkSurface = VK_NULL_HANDLE;
	}

	if (m_VkInstance != VK_NULL_HANDLE)
	{
		vkDestroyInstance(m_VkInstance, nullptr);
		m_VkInstance = VK_NULL_HANDLE;
	}
}

void VulkanWidget::createVKInstance()
{
	VkApplicationInfo appInfo{};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Hello Triangle";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "No Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;
	
	qDebug() << "Querying Instance Extension Properties";
	uint32_t numProperties = 0;
	VkResult propResult = vkEnumerateInstanceExtensionProperties(nullptr, &numProperties, nullptr);
	if (propResult != VK_SUCCESS) {
		std::string msg = "Failed to Query the Number of Extension Properties.";
		qFatal(msg.c_str());
	}

	std::vector<VkExtensionProperties> availableExtensionProperties(numProperties);
	propResult = vkEnumerateInstanceExtensionProperties(nullptr, &numProperties, availableExtensionProperties.data());
	if (propResult != VK_SUCCESS) {
		std::string msg = "Failed to Query the Extension Properties.";
		qFatal(msg.c_str());
	}

	qDebug() << "Available Instance Extension Properties";
	for (const auto& extension : availableExtensionProperties) {
		qDebug() << "Extension:" << extension.extensionName << "spec version:" << extension.specVersion;
	}

	std::vector<const char*> extensionsPropertiesRequired;
	for (const auto& it : availableExtensionProperties)
	{
		extensionsPropertiesRequired.push_back(it.extensionName);
	}

	VkInstanceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;
	createInfo.enabledExtensionCount = numProperties;
	createInfo.ppEnabledExtensionNames = extensionsPropertiesRequired.data();
	createInfo.enabledLayerCount = 0;
	createInfo.pNext = nullptr;

	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
	if (enableValidationLayers) {

		if (!checkValidationLayerSupport(&createInfo)) {
			std::string msg = "Failed to Enable Validation Layers";
			qFatal(msg.c_str());
		}
		else {
			populateDebugMessengerCreateInfo(&debugCreateInfo);
			createInfo.pNext = &debugCreateInfo;
			qDebug() << "Successfully added Validation Layers";
		}
	}

	qDebug() << "Creating Vulkan Instance";
	VkResult result = vkCreateInstance(&createInfo, nullptr, &m_VkInstance);
	if (result != VK_SUCCESS) {
		std::string msg = "Failed to Create Vulkan Instance";
		qFatal(msg.c_str());
	}
	qDebug() << "Successfully Created Vulkan Instance";

	if (enableValidationLayers)
	{
		VkDebugUtilsMessengerCreateInfoEXT debugMessengerCreateInfo;
		populateDebugMessengerCreateInfo(&debugMessengerCreateInfo);
		if (createDebugUtilsMessengerEXT(m_VkInstance, &debugMessengerCreateInfo, nullptr, &m_VkDebugMessenger) != VK_SUCCESS) {
			std::string msg = "Failed to Create Debug Messenger";
			qFatal(msg.c_str());
		}
	}


}

void VulkanWidget::pickPhysicalDevice()
{
	qDebug() << "Attempting to enumerate physical devices on system.";
	uint32_t deviceCount = 0;
	VkResult result = vkEnumeratePhysicalDevices(m_VkInstance, &deviceCount, nullptr);
	if (result != VK_SUCCESS) {
		std::string msg = "Failed to Enumerate Physical Devices.";
		qFatal(msg.c_str());
	}

	if (deviceCount == 0) {
		std::string msg = "There are 0 Physical Devices.";
		qFatal(msg.c_str());
	}
	else {
		qDebug() << "There are" << deviceCount << "physical devices";
	}

	std::vector<VkPhysicalDevice> devices(deviceCount);
	result = vkEnumeratePhysicalDevices(m_VkInstance, &deviceCount, devices.data());
	if (result != VK_SUCCESS) {
		std::string msg = "Failed to Populate the Number of Devices.";
		qFatal(msg.c_str());
	}

	qDebug() << "Attempting to pick an suitable device.";	
	std::multimap<int, VkPhysicalDevice> candidates;
	for (const auto& device : devices) {
		VkPhysicalDeviceProperties deviceProperties;
		vkGetPhysicalDeviceProperties(device, &deviceProperties);
		qDebug() << "Printing Properties for:" << deviceProperties.deviceName;
		qDebug() << "\t" << "device id" << deviceProperties.deviceID;
		qDebug() << "\t" << "device type" << deviceProperties.deviceType;
		qDebug() << "\t" << "driver version" << deviceProperties.driverVersion;
		qDebug() << "\t" << "api version" << deviceProperties.apiVersion;
		qDebug() << "\t" << "vender id" << deviceProperties.vendorID;		
		if (isDeviceSuitable(device, m_VkSurface)) {
			qDebug() << "Device is suitable.";
			int score = rateDeviceSuitability(device);
			qDebug() << "Device Score is:" << score;
			candidates.insert(std::make_pair(score, device));
		}
	}

	if (candidates.size() > 0) {
		if (candidates.rbegin()->first > 0) {
			m_VkPhysicalDevice = candidates.rbegin()->second;
			qDebug() << "Picked device with the highest score.";
		}
	}

	if (m_VkPhysicalDevice == VK_NULL_HANDLE) {
		std::string msg = "No devices were suitable";
		qFatal(msg.c_str());
	}
}

void VulkanWidget::createLogicalDevice()
{
	QueueFamilyIndices indices = findQueueFamilies(m_VkPhysicalDevice, m_VkSurface);

	std::vector<VkDeviceQueueCreateInfo> deviceQueueCreateInfos{};
	std::set<uint32_t> uniqueQueueFamilies = {
		indices.graphicsFamily.value(),
		indices.presentFamily.value()
	};

	float queuePriority = 1.0f;
	for (uint32_t queueFamily : uniqueQueueFamilies) {
		VkDeviceQueueCreateInfo queueCreateInfo{};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		deviceQueueCreateInfos.push_back(queueCreateInfo);
	}

	VkPhysicalDeviceFeatures deviceFeatures{};

	VkDeviceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.queueCreateInfoCount = static_cast<uint32_t>(deviceQueueCreateInfos.size());
	createInfo.pQueueCreateInfos = deviceQueueCreateInfos.data();
	createInfo.pEnabledFeatures = &deviceFeatures;
	createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
	createInfo.ppEnabledExtensionNames = deviceExtensions.data();	
	createInfo.enabledLayerCount = 0;

	if (enableValidationLayers) {
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();
	}

	if (vkCreateDevice(m_VkPhysicalDevice, &createInfo, nullptr, &m_VkDevice) != VK_SUCCESS) {
		std::string msg = "Failed to create a Logical Device";
		qFatal(msg.c_str());
	}

	vkGetDeviceQueue(m_VkDevice, indices.graphicsFamily.value(), 0, &m_VkGraphicsQueue);
	vkGetDeviceQueue(m_VkDevice, indices.presentFamily.value(), 0, &m_VkPresentQueue);
}

void VulkanWidget::createSurface()
{
#ifdef VK_USE_PLATFORM_WIN32_KHR

	qDebug() << "Using Win32 Window Handle for Creating the Surface";
	VkWin32SurfaceCreateInfoKHR createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	createInfo.hwnd = reinterpret_cast<HWND>(this->winId());
	createInfo.hinstance = GetModuleHandle(nullptr);

	if (vkCreateWin32SurfaceKHR(m_VkInstance, &createInfo, nullptr, &m_VkSurface) != VK_SUCCESS) {
		qFatal("Failed to create a Window Surface.");
	}

#endif

	if (m_VkSurface == VK_NULL_HANDLE) {
		qFatal("Vulkan Surface failed to be created.");
	}
}

void VulkanWidget::createSwapChain()
{
	SwapChainSupportDetails swapChainSupport = querySwapChainSupport(m_VkPhysicalDevice, m_VkSurface);

	VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
	VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
	VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities, this);

	uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;

	if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
		imageCount = swapChainSupport.capabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = m_VkSurface;

	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	QueueFamilyIndices indices = findQueueFamilies(m_VkPhysicalDevice,m_VkSurface);
	uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

	if (indices.graphicsFamily != indices.presentFamily) {
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else {
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	}

	createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;

	createInfo.oldSwapchain = VK_NULL_HANDLE;

	if (vkCreateSwapchainKHR(m_VkDevice, &createInfo, nullptr, &m_VkSwapChain) != VK_SUCCESS) {
		qFatal("Failed to create a Swap Chain.");
	}

	vkGetSwapchainImagesKHR(m_VkDevice, m_VkSwapChain, &imageCount, nullptr);
	m_VkSwapChainImages.resize(imageCount);
	vkGetSwapchainImagesKHR(m_VkDevice, m_VkSwapChain, &imageCount, m_VkSwapChainImages.data());

	m_VkSwapChainFormat = surfaceFormat.format;
	m_VkSwapChainExtent = extent;
}

void VulkanWidget::createImageViews()
{
	m_VkSwapChainImageViews.resize(m_VkSwapChainImages.size());

	for (size_t i = 0; i < m_VkSwapChainImages.size(); i++) {
		VkImageViewCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image = m_VkSwapChainImages.at(i);
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = m_VkSwapChainFormat;
		createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;

		if (vkCreateImageView(m_VkDevice, &createInfo, nullptr, &m_VkSwapChainImageViews.at(i)) != VK_SUCCESS) {
			qFatal("Unable to create Image View.");
		}
		else {
			qDebug() << "Created Image View for Image" << i;
		}
	}
}

void VulkanWidget::createGraphicsPipeline()
{

}

