#include "Device.hpp"

#include <set>

namespace clf
{
	Device::Device(Window& window)
		: window(window)
	{
		InitVulkan();
		InitDebugMessenger();
		InitSurface();
		InitPhysicalDevice();
		InitLogicalDevice();
	}

	Device::~Device()
	{
		auto fn = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
		if (enableValidationLayers)
			fn(instance, debugMessenger, nullptr);

		vkDestroyDevice(logicalDevice, nullptr);
		vkDestroySurfaceKHR(instance, surface, nullptr);
		vkDestroyInstance(instance, nullptr);
	}

	VKAPI_ATTR VkBool32 VKAPI_CALL Device::DebugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData)
	{
		auto& msg = pCallbackData->pMessage;
		switch (messageSeverity)
		{
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
			CLF_ERROR(msg); return VK_FALSE;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
			CLF_WARN(msg); return VK_FALSE;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
			CLF_INFO(msg); return VK_TRUE;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
			CLF_TRACE(msg); return VK_TRUE;
		}
	}

	void Device::InitVulkan()
	{
		VkApplicationInfo appInfo{};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "Hello World!";
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = "Cliffine Engine";
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion = VK_API_VERSION_1_3;

		VkInstanceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;

		auto extensions = GetRequiredExtensions();

		createInfo.enabledExtensionCount = (u32)extensions.size();
		createInfo.ppEnabledExtensionNames = extensions.data();

		if (enableValidationLayers)
		{
			createInfo.enabledLayerCount = (u32)validationLayers.size();
			createInfo.ppEnabledLayerNames = validationLayers.data();

			auto debugCreateInfo = CreateDebugMessengerInfo();
			createInfo.pNext = &debugCreateInfo;
			CheckValidationLayerSupport();
		}

		CheckExtensionSupport();

		CLF_ASSERT(vkCreateInstance(&createInfo, nullptr, &instance) == VK_SUCCESS,
			"Failed to initialize Device!");
	}

	void Device::InitDebugMessenger()
	{
		if (!enableValidationLayers) return;

		auto createInfo = CreateDebugMessengerInfo();

		auto fn = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
		CLF_ASSERT(fn(instance, &createInfo, nullptr, &debugMessenger) == VK_SUCCESS,
			"Failed to initialize Debug Messenger");
	}

	void Device::InitSurface()
	{
		window.InitSurface(instance, &surface);
	}

	void Device::InitPhysicalDevice()
	{
		u32 deviceCount = 0;
		vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

		CLF_ASSERT(deviceCount > 0, "Failed to Find GPUs with Device support!");
		vec<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

		for (const auto& device : devices)
			if (IsDeviceSuited(device))
				physicalDevice = device;

		CLF_ASSERT(physicalDevice != nullptr, "Failed to find suitable GPU!");
	}

	void Device::InitLogicalDevice()
	{
		auto indices = FindQueueFamilies(physicalDevice);

		vec<VkDeviceQueueCreateInfo> queueCreateInfos;
		set<u32> families { indices.graphics.value(), indices.present.value() };

		f32 queuePriority = 1.0f;
		for (u32 family : families)
		{
			VkDeviceQueueCreateInfo queueCreateInfo{};
			queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfo.queueFamilyIndex = family;
			queueCreateInfo.queueCount = 1;
			queueCreateInfo.pQueuePriorities = &queuePriority;
			queueCreateInfos.push_back(queueCreateInfo);
		}

		VkDeviceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		createInfo.pQueueCreateInfos = queueCreateInfos.data();
		createInfo.queueCreateInfoCount = (u32)queueCreateInfos.size(); //Only Graphics Capability support

		VkPhysicalDeviceFeatures deviceFeatures{};
		createInfo.pEnabledFeatures = &deviceFeatures;

		createInfo.enabledExtensionCount = (u32)deviceExtensions.size();
		createInfo.ppEnabledExtensionNames = deviceExtensions.data();


		CLF_ASSERT(vkCreateDevice(physicalDevice, &createInfo, nullptr, &logicalDevice) == VK_SUCCESS,
			"Failed to initialize logical device!");

		vkGetDeviceQueue(logicalDevice, indices.graphics.value(), 0, &graphicsQueue);
		vkGetDeviceQueue(logicalDevice, indices.present.value(), 0, &presentQueue);

	}

	VkDebugUtilsMessengerCreateInfoEXT Device::CreateDebugMessengerInfo()
	{
		VkDebugUtilsMessengerCreateInfoEXT createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		createInfo.pfnUserCallback = DebugCallback;

		return createInfo;
	}

	void Device::CheckValidationLayerSupport() const
	{
		uint32_t layerCount;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
		vec<VkLayerProperties> availableLayers(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

		for (const char* layerName : validationLayers)
		{
			bool layerFound = false;
			for (const auto& layerProperties : availableLayers)
				if (strcmp(layerName, layerProperties.layerName) == 0)
				{
					layerFound = true;
					break;
				}

			CLF_ASSERT(layerFound,
				"Unavailable validation layers!");
		}
	}

	void Device::CheckExtensionSupport() const
	{
		u32 extensionCount = 0;
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
		vec<VkExtensionProperties> extensions(extensionCount);
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

		std::stringstream ss;
		for (const auto& extension : extensions)
			ss << '\t' << extension.extensionName << '\n';
		CLF_INFO("Available Extensions:\n{0}", ss.str());
	}
	
	//You could implement scoring system to pick best GPU!
	const bool Device::IsDeviceSuited(const VkPhysicalDevice& device) const
	{
		return IsDeviceExtensionSupported(device) && FindQueueFamilies(device).IsComplete() && FindSwapchainDetails(device).IsAdequate();
	}

	const bool Device::IsDeviceExtensionSupported(const VkPhysicalDevice& device) const
	{
		u32 extensionCount;
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

		vec<VkExtensionProperties> availableExtensions(extensionCount);
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

		set<str> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

		for (const auto& extension : availableExtensions)
			requiredExtensions.erase(extension.extensionName);

		return requiredExtensions.empty();
	}

	const QueueFamilies Device::FindQueueFamilies(const VkPhysicalDevice& device) const
	{
		QueueFamilies indices;
		u32 count;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &count, nullptr);

		vec<VkQueueFamilyProperties> families(count);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &count, families.data());

		u32 i = 0;
		for (const auto& family : families)
		{
			if (indices.IsComplete()) break;

			if (family.queueFlags & VK_QUEUE_GRAPHICS_BIT)
				indices.graphics = i;

			VkBool32 presentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
			if (presentSupport)
				indices.present = i;

			i++;
		}

		return indices;
	}

	const SwapchainDetails Device::FindSwapchainDetails(const VkPhysicalDevice& device) const
	{
		SwapchainDetails details;

		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

		u32 formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);
		details.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());

		u32 presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);
		details.presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());


		return details;
	}

	const vec<const char*> Device::GetRequiredExtensions() const
	{
		u32 glfwExtensionCount;
		const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		vec<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
		if (enableValidationLayers)
			extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

		return extensions;
	}
}