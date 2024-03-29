#include <cassert>
#include <cstddef>
#include <glm/detail/qualifier.hpp>
#include <glm/ext/matrix_transform.hpp>
// #include <glm/fwd.hpp>
// #include <process.h>

#define VMA_IMPLEMENTATION
#define VMA_STATIC_VULKAN_FUNCTIONS 0
// #define VMA_DYNAMIC_VULKAN_FUNCTIONS 0
// #define VMA_VULKAN_VERSION 1003000
#include "render.hpp" 
#include "vulkan/vulkan_core.h"

#include <set>
#include <iostream>
#include <limits>
#include <fstream>

using namespace std;
using namespace glm;

//they are global because its easier lol
const vector<const char*> debugInstanceLayers = {
#ifndef VKNDEBUG
    "VK_LAYER_KHRONOS_validation",
#endif
    "VK_LAYER_LUNARG_monitor",
};
vector<const char*> debugInstanceExtensions = {
#ifndef VKNDEBUG
    VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
#endif
};
static int itime = 0;

//no debug device extensions
vector<const char*> instanceLayers = {};
vector<const char*> instanceExtensions = {
    VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME,
};
vector<const char*>   deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    VK_KHR_PUSH_DESCRIPTOR_EXTENSION_NAME,
    // "VK_KHR_shader_non_semantic_info"
};


void Renderer::get_Layers(){
    for(auto layer : debugInstanceLayers){
        instanceLayers.push_back(layer);
    }
    
    //actual verify part
    // for(auto layer : instanceLayers){
        
    // }
}

void Renderer::get_Instance_Extensions(){
    u32 glfwExtCount;
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtCount);
    
    for(u32 i=0; i < glfwExtCount; i++){
        instanceExtensions.push_back(glfwExtensions[i]);
    }
    for(auto ext : debugInstanceExtensions){
        instanceExtensions.push_back(ext);
    }

    //actual verify part
    u32 supportedExtensionCount = 0;
    VK_CHECK(vkEnumerateInstanceExtensionProperties(NULL, &supportedExtensionCount, NULL));
    vector<VkExtensionProperties> supportedInstanceExtensions(supportedExtensionCount);
    VK_CHECK(vkEnumerateInstanceExtensionProperties(NULL, &supportedExtensionCount, supportedInstanceExtensions.data()));    

    for(auto ext : instanceExtensions){
        bool supported = false;
        for(auto sup : supportedInstanceExtensions){
            if(strcmp(ext, sup.extensionName) == 0) supported = true;
        }
        if(not supported) {
            cout << KRED << ext << " not supported" << KEND;
            exit(1);
        }
    }
}

void Renderer::get_PhysicalDevice_Extensions(){
    //empty - they are in global vector. Might change in future tho
}

void Renderer::create_Allocator(){
    VmaAllocatorCreateInfo allocatorInfo = {};
    VmaVulkanFunctions vulkanFunctions;
        vulkanFunctions.vkGetInstanceProcAddr = vkGetInstanceProcAddr;
        vulkanFunctions.vkGetDeviceProcAddr = vkGetDeviceProcAddr;
        vulkanFunctions.vkGetPhysicalDeviceProperties = vkGetPhysicalDeviceProperties;
        vulkanFunctions.vkGetPhysicalDeviceMemoryProperties = vkGetPhysicalDeviceMemoryProperties;
        vulkanFunctions.vkAllocateMemory = vkAllocateMemory;
        vulkanFunctions.vkFreeMemory = vkFreeMemory;
        vulkanFunctions.vkMapMemory = vkMapMemory;
        vulkanFunctions.vkUnmapMemory = vkUnmapMemory;
        vulkanFunctions.vkFlushMappedMemoryRanges = vkFlushMappedMemoryRanges;
        vulkanFunctions.vkInvalidateMappedMemoryRanges = vkInvalidateMappedMemoryRanges;
        vulkanFunctions.vkBindBufferMemory = vkBindBufferMemory;
        vulkanFunctions.vkBindImageMemory = vkBindImageMemory;
        vulkanFunctions.vkGetBufferMemoryRequirements = vkGetBufferMemoryRequirements;
        vulkanFunctions.vkGetImageMemoryRequirements = vkGetImageMemoryRequirements;
        vulkanFunctions.vkCreateBuffer = vkCreateBuffer;
        vulkanFunctions.vkDestroyBuffer = vkDestroyBuffer;
        vulkanFunctions.vkCreateImage = vkCreateImage;
        vulkanFunctions.vkDestroyImage = vkDestroyImage;
        vulkanFunctions.vkCmdCopyBuffer = vkCmdCopyBuffer;
        vulkanFunctions.vkGetBufferMemoryRequirements2KHR = vkGetBufferMemoryRequirements2KHR;
        vulkanFunctions.vkGetImageMemoryRequirements2KHR = vkGetImageMemoryRequirements2KHR;
        vulkanFunctions.vkBindBufferMemory2KHR = vkBindBufferMemory2KHR;
        vulkanFunctions.vkBindImageMemory2KHR = vkBindImageMemory2KHR;
        vulkanFunctions.vkGetPhysicalDeviceMemoryProperties2KHR = vkGetPhysicalDeviceMemoryProperties2KHR;
    allocatorInfo.pVulkanFunctions = (const VmaVulkanFunctions*) &vulkanFunctions;
    allocatorInfo.instance = instance;
    allocatorInfo.physicalDevice = physicalDevice;
    allocatorInfo.device = device;
    VK_CHECK(vmaCreateAllocator(&allocatorInfo, &VMAllocator));
}

void Renderer::create_Window(){
    int glfwRes = glfwInit();
    assert(glfwRes != 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    window.width  = mode->width  / 1;
    window.height = mode->height / 1;
    
    // window.pointer = glfwCreateWindow(window.width, window.height, "renderer_vk", glfwGetPrimaryMonitor(), 0);
    window.pointer = glfwCreateWindow(window.width, window.height, "renderer_vk", 0, 0);
    assert(window.pointer != NULL);
}

void Renderer::create_Instance(){
    VkApplicationInfo app_info = {};
        app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        app_info.pApplicationName = "Lol vulkan raytracer";
        app_info.applicationVersion = VK_MAKE_VERSION(1,0,0);
        app_info.pEngineName = "my vk engine";
        app_info.engineVersion = VK_MAKE_VERSION(1,0,0);
        app_info.apiVersion = VK_API_VERSION_1_3;

    vector<VkValidationFeatureEnableEXT> enabledFeatures = {VK_VALIDATION_FEATURE_ENABLE_BEST_PRACTICES_EXT};

    VkValidationFeaturesEXT features = {};
        features.sType = VK_STRUCTURE_TYPE_VALIDATION_FEATURES_EXT;
        features.enabledValidationFeatureCount = enabledFeatures.size();
        features.pEnabledValidationFeatures    = enabledFeatures.data();

    VkInstanceCreateInfo createInfo = {};
    #ifndef VKNDEBUG 
        // createInfo.pNext = &features; // according to spec
    #endif 
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &app_info;
        createInfo.enabledExtensionCount   = instanceExtensions.size();
        createInfo.ppEnabledExtensionNames = instanceExtensions.data();
        createInfo.enabledLayerCount   = debugInstanceLayers.size();
        createInfo.ppEnabledLayerNames = debugInstanceLayers.data();

    VK_CHECK(vkCreateInstance(&createInfo, NULL, &instance));
}

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData) {
    if(messageType == VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) printf(KCYN);
    if(messageType == VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) printf(KRED); 
    if(messageType == VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT) printf(KBLU); 
    printf("validation layer: %s\n\n" KEND, pCallbackData->pMessage);
    return VK_FALSE;
}

void Renderer::setup_Debug_Messenger(){
    VkDebugUtilsMessengerEXT debugMessenger;
    
    VkDebugUtilsMessengerCreateInfoEXT debugUtilsCreateInfo = {};
        debugUtilsCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        debugUtilsCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        debugUtilsCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        debugUtilsCreateInfo.pfnUserCallback = debugCallback;

    VK_CHECK(vkCreateDebugUtilsMessengerEXT(instance, &debugUtilsCreateInfo, NULL, &debugMessenger));

    this->debugMessenger = debugMessenger;
}

void Renderer::create_Surface(){
    VK_CHECK(glfwCreateWindowSurface(instance, window.pointer, NULL, &surface));
}

QueueFamilyIndices Renderer::find_Queue_Families(VkPhysicalDevice device){
    QueueFamilyIndices indices;

    u32 queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, NULL);
    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

    i32 i = 0;
    for (auto queueFamily : queueFamilies) {
        if ((queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) and (queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT)) {
            indices.graphicalAndCompute = i;
        }

        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

        if (presentSupport) {
            indices.present = i;
        }

        if (indices.isComplete()) {
            break;
        }

        i++;
    }

    return indices;
}

bool Renderer::check_PhysicalDevice_Extension_Support(VkPhysicalDevice device){
    u32 extensionCount;
    vkEnumerateDeviceExtensionProperties(device, NULL, &extensionCount, NULL);
    vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, NULL, &extensionCount, availableExtensions.data());

    //for time complexity? FOR CODE COMPLEXITY HAHAHA
    //does not apply to current code

    vector<const char*> requiredExtensions = deviceExtensions;

    for (auto req : requiredExtensions) {
        bool supports = false;
        for (auto avail : availableExtensions) {
            if(strcmp(req, avail.extensionName) == 0) {
                supports = true;
            }
        }
        if (not supports) {
            // cout << KRED << req << "not supported\n" << KEND;
            return false;
        }
    }
    return true;
}

SwapChainSupportDetails Renderer::query_Swapchain_Support(VkPhysicalDevice device){
    SwapChainSupportDetails details;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

    u32 formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, NULL);
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());

    u32 presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, NULL);
        details.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());

    return details;
}

bool Renderer::check_Format_Support(VkPhysicalDevice device, VkFormat format, VkFormatFeatureFlags features) {
    VkFormatProperties formatProps;
    vkGetPhysicalDeviceFormatProperties(device, format, &formatProps);
    printf("%X\n", features);
    printf("%X\n", formatProps.optimalTilingFeatures);
    printf("%d\n", formatProps.optimalTilingFeatures & features);

    // if (formatProps.optimalTilingFeatures & features) {
    //     return true;
    // }
    // imageFormatProps.
    // return false;
    
    // // cout << res;
    // if (res == VK_SUCCESS) {
    //     return true;
    // }
    return formatProps.optimalTilingFeatures & features;
}

bool Renderer::is_PhysicalDevice_Suitable(VkPhysicalDevice device) {
    QueueFamilyIndices indices = find_Queue_Families(device);
    SwapChainSupportDetails swapChainSupport;
    bool extensionsSupported = check_PhysicalDevice_Extension_Support(device);
    bool imageFormatSupport = check_Format_Support(
        device,
        VK_FORMAT_R8G8B8A8_UNORM,
        VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT
    );
    
    if (extensionsSupported) {
        swapChainSupport  = query_Swapchain_Support(device);
    }

    return indices.isComplete() 
        && extensionsSupported 
        && swapChainSupport.is_Suitable() 
        && imageFormatSupport
    ;   
}

void Renderer::pick_Physical_Device(){
    u32 deviceCount;
    VK_CHECK(vkEnumeratePhysicalDevices(instance, &deviceCount, NULL));
    vector<VkPhysicalDevice> devices (deviceCount);
    VK_CHECK(vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data()));

    physicalDevice = VK_NULL_HANDLE;
    for (auto device : devices) {
        // cout << device << "\n";
        if (is_PhysicalDevice_Suitable(device)){
            physicalDevice = device;//could be moved out btw
            break;
        }
    }
    if(physicalDevice == VK_NULL_HANDLE){
        cout << KRED "no suitable physical device\n" KEND;
        exit(1);
    }
}

void Renderer::create_Logical_Device(){
    QueueFamilyIndices indices = find_Queue_Families(physicalDevice);

    vector<VkDeviceQueueCreateInfo> queueCreateInfos = {};
    set<u32> uniqueQueueFamilies = {indices.graphicalAndCompute.value(), indices.present.value()};

    float queuePriority = 1.0f; //random priority?
    for (u32 queueFamily : uniqueQueueFamilies) {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    VkPhysicalDeviceFeatures deviceFeatures = {};

    VkDeviceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.queueCreateInfoCount = queueCreateInfos.size();
        createInfo.pQueueCreateInfos    = queueCreateInfos.data();
        createInfo.pEnabledFeatures = &deviceFeatures;
        createInfo.enabledExtensionCount   = deviceExtensions.size();
        createInfo.ppEnabledExtensionNames = deviceExtensions.data();
        createInfo.enabledLayerCount   = instanceLayers.size();
        createInfo.ppEnabledLayerNames = instanceLayers.data();

    VK_CHECK(vkCreateDevice(physicalDevice, &createInfo, NULL, &device));

    vkGetDeviceQueue(device, indices.graphicalAndCompute.value(), 0, &graphicsQueue);
    vkGetDeviceQueue(device, indices.graphicalAndCompute.value(), 0, &computeQueue);
    vkGetDeviceQueue(device, indices.present.value(), 0, &presentQueue);
}

VkSurfaceFormatKHR Renderer::choose_Swap_SurfaceFormat(vector<VkSurfaceFormatKHR> availableFormats) {
    for (auto format : availableFormats) {
        if (format.format == VK_FORMAT_R8G8B8A8_UNORM && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return format;
        }
    }
    cout << KYEL "Where is your VK_FORMAT_R8G8B8A8_UNORM VK_COLOR_SPACE_SRGB_NONLINEAR_KHR?\n" KEND;
    return availableFormats[0];
}

VkPresentModeKHR Renderer::choose_Swap_PresentMode(vector<VkPresentModeKHR> availablePresentModes) {
    for (auto mode : availablePresentModes) {
        if (mode == VK_PRESENT_MODE_FIFO_KHR) {
        // if (mode == VK_PRESENT_MODE_MAILBOX_KHR) {
            return mode;}
    }
    // cout << "fifo\n";
    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D Renderer::choose_Swap_Extent(VkSurfaceCapabilitiesKHR capabilities) {
    if (capabilities.currentExtent.width != numeric_limits<u32>::max()) {
        return capabilities.currentExtent;
    } else {
        i32 width, height;
        glfwGetFramebufferSize(window.pointer, &width, &height);

        VkExtent2D actualExtent {(u32) width, (u32) height};

        actualExtent.width  = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

        return actualExtent;
    }
}

void Renderer::create_Swapchain(){
    SwapChainSupportDetails swapChainSupport = query_Swapchain_Support(physicalDevice);

    VkSurfaceFormatKHR surfaceFormat = choose_Swap_SurfaceFormat(swapChainSupport.formats);
    VkPresentModeKHR presentMode = choose_Swap_PresentMode(swapChainSupport.presentModes);
    VkExtent2D extent = choose_Swap_Extent(swapChainSupport.capabilities);

    u32 imageCount = swapChainSupport.capabilities.minImageCount + 1;
    if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface = surface;
        createInfo.minImageCount = imageCount;
        createInfo.imageFormat = surfaceFormat.format;
        createInfo.imageColorSpace = surfaceFormat.colorSpace;
        createInfo.imageExtent = extent;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_STORAGE_BIT;

    QueueFamilyIndices indices = find_Queue_Families(physicalDevice);
    u32 queueFamilyIndices[] = {indices.graphicalAndCompute.value(), indices.present.value()};

    if (indices.graphicalAndCompute != indices.present) {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    } else {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }

        createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        createInfo.presentMode = presentMode;
        createInfo.clipped = VK_TRUE;
        createInfo.oldSwapchain = VK_NULL_HANDLE;

    VK_CHECK(vkCreateSwapchainKHR(device, &createInfo, NULL, &swapchain));

    vkGetSwapchainImagesKHR(device, swapchain, &imageCount, NULL);
    swapChainImages.resize(imageCount);
    // cout << imageCount << "\n";
    vkGetSwapchainImagesKHR(device, swapchain, &imageCount, swapChainImages.data());

    swapChainImageFormat = surfaceFormat.format;
    swapChainExtent = extent;
}
void Renderer::recreate_Swapchain(){
    // vkDeviceWaitIdle(device);

    // for (auto framebuffer : swapChainFramebuffers) {
    //     vkDestroyFramebuffer(device, framebuffer, NULL);}
    // for (auto imageView : swapChainImageViews) {
    //     vkDestroyImageView(device, imageView, NULL);}
    // vkDestroySwapchainKHR(device, swapchain, NULL);

    // for (i32 i=0; i<MAX_FRAMES_IN_FLIGHT; i++){
    //     vkDestroyImageView(device,      raytracedImageViews[i], NULL);
    //     vkDestroyImageView(device, rayGenPosMatImageViews[i], NULL);
    //     vkDestroyImageView(device,   rayGenNormImageViews[i], NULL);
    //     vkFreeMemory(device,      raytracedImagesMemory[i], NULL);
    //     vkFreeMemory(device, rayGenPosMatImagesMemory[i], NULL);
    //     vkFreeMemory(device,   rayGenNormImagesMemory[i], NULL);
    //     vkDestroyImage(device,      raytracedImages[i], NULL);
    //     vkDestroyImage(device, rayGenPosMatImages[i], NULL);
    //     vkDestroyImage(device,   rayGenNormImages[i], NULL);

    //     vkDestroyFramebuffer(device, rayGenFramebuffers[i], NULL);
    // }

    // create_Swapchain();
    // create_Image_Views();

    // create_Image_Storages(raytracedImages, raytracedImagesMemory, raytracedImageViews,
    //     VK_IMAGE_TYPE_2D,
    //     VK_FORMAT_R8G8B8A8_UNORM,
    //     VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
    //     VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
    //     VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
    //     VK_ACCESS_SHADER_WRITE_BIT);
    // create_Image_Storages(rayGenPosMatImages, rayGenPosMatImagesMemory, rayGenPosMatImageViews,
    //     VK_IMAGE_TYPE_2D,
    //     VK_FORMAT_R32G32B32A32_SFLOAT,
    //     VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
    //     VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
    //     VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
    //     VK_ACCESS_SHADER_WRITE_BIT);
    // create_Image_Storages(rayGenNormImages, rayGenNormImagesMemory, rayGenNormImageViews,
    //     VK_IMAGE_TYPE_2D,
    //     VK_FORMAT_R32G32B32A32_SFLOAT,
    //     VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
    //     VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
    //     VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
    //     VK_ACCESS_SHADER_WRITE_BIT);

    // vector<vector<VkImageView>> swapViews = {swapChainImageViews};
    // create_N_Framebuffers(swapChainFramebuffers, swapViews, graphicalRenderPass, swapChainImages.size(), swapChainExtent.width, swapChainExtent.height);

    // vector<vector<VkImageView>> rayVeiws = {rayGenPosMatImageViews, rayGenNormImageViews};
    // create_N_Framebuffers(rayGenFramebuffers, rayVeiws, rayGenRenderPass, MAX_FRAMES_IN_FLIGHT, swapChainExtent.width, swapChainExtent.height);

}

void Renderer::create_Swapchain_Image_Views(){
    swapChainImageViews.resize(swapChainImages.size());

    for(i32 i=0; i<swapChainImages.size(); i++){
        VkImageViewCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = swapChainImages[i];
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = swapChainImageFormat;
        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;

        VK_CHECK(vkCreateImageView(device, &createInfo, NULL, &swapChainImageViews[i]));
    }
}

vector<char> read_Shader(const char* filename) {
    ifstream file(filename, ios::ate | ios::binary);
    assert(file.is_open());

    u32 fileSize = file.tellg();
    vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);

    file.close();
    return buffer;
}

VkShaderModule Renderer::create_Shader_Module(vector<char>& code) {
    VkShaderModule shaderModule;
    VkShaderModuleCreateInfo createInfo = {};

    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const u32*>(code.data());

    VK_CHECK(vkCreateShaderModule(device, &createInfo, NULL, &shaderModule));

    return shaderModule;
}

void Renderer::create_RenderPass_Graphical(){
    VkAttachmentDescription colorAttachment = {};
        colorAttachment.format = swapChainImageFormat;
        colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        colorAttachment.loadOp  = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE; 
        colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentRef = {};
        colorAttachmentRef.attachment = 0;
        colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass = {};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorAttachmentRef;

    VkSubpassDependency dependency{};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.srcAccessMask = 0;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    
    VkRenderPassCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        createInfo.attachmentCount = 1;
        createInfo.pAttachments = &colorAttachment;
        createInfo.subpassCount = 1;
        createInfo.pSubpasses = &subpass;
        createInfo.dependencyCount = 1;
        createInfo.pDependencies = &dependency;
    
    VK_CHECK(vkCreateRenderPass(device, &createInfo, NULL, &graphicalRenderPass));

}
void Renderer::create_RenderPass_RayGen(){
    VkAttachmentDescription colorAttachmentPosMat = {};
        colorAttachmentPosMat.format = VK_FORMAT_R32G32B32A32_SFLOAT;
        colorAttachmentPosMat.samples = VK_SAMPLE_COUNT_1_BIT;
        colorAttachmentPosMat.loadOp  = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachmentPosMat.storeOp = VK_ATTACHMENT_STORE_OP_STORE; 
        colorAttachmentPosMat.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachmentPosMat.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachmentPosMat.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachmentPosMat.finalLayout = VK_IMAGE_LAYOUT_GENERAL;
    VkAttachmentDescription colorAttachmentNorm = {};
        colorAttachmentNorm.format = VK_FORMAT_R32G32B32A32_SFLOAT;
        colorAttachmentNorm.samples = VK_SAMPLE_COUNT_1_BIT;
        colorAttachmentNorm.loadOp  = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachmentNorm.storeOp = VK_ATTACHMENT_STORE_OP_STORE; 
        colorAttachmentNorm.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachmentNorm.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachmentNorm.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachmentNorm.finalLayout = VK_IMAGE_LAYOUT_GENERAL;
    VkAttachmentDescription depthAttachment = {};
        depthAttachment.format = VK_FORMAT_D32_SFLOAT;
        depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        depthAttachment.loadOp  = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE; 
        depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference colorAttachmentPosMatRef = {};
        colorAttachmentPosMatRef.attachment = 0;
        colorAttachmentPosMatRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    VkAttachmentReference colorAttachmentNormRef = {};
        colorAttachmentNormRef.attachment = 1;
        colorAttachmentNormRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    VkAttachmentReference depthAttachmentRef = {};
        depthAttachmentRef.attachment = 2;
        depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    vector<VkAttachmentDescription> attachments = {colorAttachmentPosMat, colorAttachmentNorm, depthAttachment};
    vector<VkAttachmentReference>   colorAttachmentRefs = {colorAttachmentPosMatRef, colorAttachmentNormRef};

    VkSubpassDescription subpass = {};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.pDepthStencilAttachment = &depthAttachmentRef;
        subpass.colorAttachmentCount = colorAttachmentRefs.size();
        subpass.pColorAttachments = colorAttachmentRefs.data();

    VkSubpassDependency dependency{};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependency.srcAccessMask = 0;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

    
    VkRenderPassCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        createInfo.attachmentCount = attachments.size();
        createInfo.pAttachments    = attachments.data();
        createInfo.subpassCount = 1;
        createInfo.pSubpasses = &subpass;
        createInfo.dependencyCount = 1;
        createInfo.pDependencies = &dependency;
    
    VK_CHECK(vkCreateRenderPass(device, &createInfo, NULL, &rayGenRenderPass));

}

void Renderer::create_Graphics_Pipeline(){
    auto vertShaderCode = read_Shader("shaders/compiled/vert.spv");
    auto fragShaderCode = read_Shader("shaders/compiled/frag.spv");

    graphicsVertShaderModule = create_Shader_Module(vertShaderCode);
    graphicsFragShaderModule = create_Shader_Module(fragShaderCode);

    VkPipelineShaderStageCreateInfo vertShaderStageInfo = {};
        vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
        vertShaderStageInfo.module = graphicsVertShaderModule;
        vertShaderStageInfo.pName = "main";
    VkPipelineShaderStageCreateInfo fragShaderStageInfo = {};
        fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        fragShaderStageInfo.module = graphicsFragShaderModule;
        fragShaderStageInfo.pName = "main";

    //common
    VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

    VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputInfo.vertexBindingDescriptionCount = 0;
        vertexInputInfo.pVertexBindingDescriptions = NULL; // Optional
        vertexInputInfo.vertexAttributeDescriptionCount = 0;
        vertexInputInfo.pVertexAttributeDescriptions = NULL; // Optional
    
    VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
        inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        inputAssembly.primitiveRestartEnable = VK_FALSE;

    VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width  = (float) swapChainExtent.width;
        viewport.height = (float) swapChainExtent.height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

    VkRect2D scissor{};
        scissor.offset = {0, 0};
        scissor.extent = swapChainExtent;  

    vector<VkDynamicState> dynamicStates = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };
    VkPipelineDynamicStateCreateInfo dynamicState = {};
        dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicState.dynamicStateCount = dynamicStates.size();
        dynamicState.pDynamicStates = dynamicStates.data();

    VkPipelineViewportStateCreateInfo viewportState = {};
        viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.viewportCount = 1;
        viewportState.scissorCount = 1;
    
    VkPipelineRasterizationStateCreateInfo rasterizer = {};
        rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizer.depthClampEnable = VK_FALSE;
        rasterizer.rasterizerDiscardEnable = VK_FALSE;
        rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
        rasterizer.lineWidth = 1.0f;
        rasterizer.depthBiasEnable = VK_FALSE;
        rasterizer.depthBiasConstantFactor = 0.0f; // Optional
        rasterizer.depthBiasClamp = 0.0f; // Optional
        rasterizer.depthBiasSlopeFactor = 0.0f; // Optional

    VkPipelineMultisampleStateCreateInfo multisampling = {};
        multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.sampleShadingEnable = VK_FALSE;
        multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        multisampling.minSampleShading = 1.0f; // Optional
        multisampling.pSampleMask = NULL; // Optional
        multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
        multisampling.alphaToOneEnable = VK_FALSE; // Optional
    
    VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
        colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        colorBlendAttachment.blendEnable = VK_FALSE;
        colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
        colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
        colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
        colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
        colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
        colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional

    VkPipelineColorBlendStateCreateInfo colorBlending{};
        colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlending.logicOpEnable = VK_FALSE;
        colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
        colorBlending.attachmentCount = 1;
        colorBlending.pAttachments = &colorBlendAttachment;
        colorBlending.blendConstants[0] = 0.0f; // Optional
        colorBlending.blendConstants[1] = 0.0f; // Optional
        colorBlending.blendConstants[2] = 0.0f; // Optional
        colorBlending.blendConstants[3] = 0.0f; // Optional

    VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 1;
        pipelineLayoutInfo.pSetLayouts = &graphicalDescriptorSetLayout;
        pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
        pipelineLayoutInfo.pPushConstantRanges = NULL; // Optional

    VK_CHECK(vkCreatePipelineLayout(device, &pipelineLayoutInfo, NULL, &graphicsLayout));

    VkGraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.stageCount = 2;
        pipelineInfo.pStages = shaderStages;
        pipelineInfo.pVertexInputState = &vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &inputAssembly;
        pipelineInfo.pViewportState = &viewportState;
        pipelineInfo.pRasterizationState = &rasterizer;
        pipelineInfo.pMultisampleState = &multisampling;
        pipelineInfo.pDepthStencilState = NULL; // Optional
        pipelineInfo.pColorBlendState = &colorBlending;
        pipelineInfo.pDynamicState = &dynamicState;
        
        pipelineInfo.layout = graphicsLayout;
        
        pipelineInfo.renderPass = graphicalRenderPass;
        pipelineInfo.subpass = 0;
        
        pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
        pipelineInfo.basePipelineIndex = -1; // Optional

        VK_CHECK(vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, NULL, &graphicsPipeline));
}

void Renderer::create_RayGen_Pipeline(){
    auto vertShaderCode = read_Shader("shaders/compiled/rayGenVert.spv");
    auto fragShaderCode = read_Shader("shaders/compiled/rayGenFrag.spv");

    rayGenVertShaderModule = create_Shader_Module(vertShaderCode);
    rayGenFragShaderModule = create_Shader_Module(fragShaderCode);

    VkPipelineShaderStageCreateInfo vertShaderStageInfo = {};
        vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
        vertShaderStageInfo.module = rayGenVertShaderModule;
        vertShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo fragShaderStageInfo = {};
        fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        fragShaderStageInfo.module = rayGenFragShaderModule;
        fragShaderStageInfo.pName = "main";

    VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Vertex);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    vector<VkVertexInputAttributeDescription> attributeDescriptions(3);
        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(Vertex, pos);
        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Vertex, norm);
        attributeDescriptions[2].binding = 0;
        attributeDescriptions[2].location = 2;
        attributeDescriptions[2].format = VK_FORMAT_R8_UINT;
        attributeDescriptions[2].offset = offsetof(Vertex, matID);

    VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputInfo.vertexBindingDescriptionCount = 1;
        vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
        vertexInputInfo.vertexAttributeDescriptionCount = attributeDescriptions.size();
        vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();
    
    VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
        inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        inputAssembly.primitiveRestartEnable = VK_FALSE;

    VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width  = (float) swapChainExtent.width;
        viewport.height = (float) swapChainExtent.height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

    VkRect2D scissor{};
        scissor.offset = {0, 0};
        scissor.extent = swapChainExtent;  

    vector<VkDynamicState> dynamicStates = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };
    VkPipelineDynamicStateCreateInfo dynamicState = {};
        dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicState.dynamicStateCount = dynamicStates.size();
        dynamicState.pDynamicStates = dynamicStates.data();

    VkPipelineViewportStateCreateInfo viewportState = {};
        viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.viewportCount = 1;
        viewportState.scissorCount = 1;
    
    VkPipelineRasterizationStateCreateInfo rasterizer = {};
        rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizer.depthClampEnable = VK_FALSE;
        rasterizer.rasterizerDiscardEnable = VK_FALSE;
        rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
        rasterizer.lineWidth = 1.0f;
        rasterizer.depthBiasEnable = VK_FALSE;
        rasterizer.depthBiasConstantFactor = 0.0f; // Optional
        rasterizer.depthBiasClamp = 0.0f; // Optional
        rasterizer.depthBiasSlopeFactor = 0.0f; // Optional

    VkPipelineMultisampleStateCreateInfo multisampling = {};
        multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.sampleShadingEnable = VK_FALSE;
        multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        multisampling.minSampleShading = 1.0f; // Optional
        multisampling.pSampleMask = NULL; // Optional
        multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
        multisampling.alphaToOneEnable = VK_FALSE; // Optional
    
VkPipelineColorBlendAttachmentState colorBlendAttachmentPos = {};
        colorBlendAttachmentPos.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        colorBlendAttachmentPos.blendEnable = VK_FALSE;
        colorBlendAttachmentPos.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
        colorBlendAttachmentPos.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
        colorBlendAttachmentPos.colorBlendOp = VK_BLEND_OP_ADD; // Optional
        colorBlendAttachmentPos.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
        colorBlendAttachmentPos.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
        colorBlendAttachmentPos.alphaBlendOp = VK_BLEND_OP_ADD; // Optional
    VkPipelineColorBlendAttachmentState colorBlendAttachmentNorm = {};
        colorBlendAttachmentNorm.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        colorBlendAttachmentNorm.blendEnable = VK_FALSE;
        colorBlendAttachmentNorm.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
        colorBlendAttachmentNorm.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
        colorBlendAttachmentNorm.colorBlendOp = VK_BLEND_OP_ADD; // Optional
        colorBlendAttachmentNorm.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
        colorBlendAttachmentNorm.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
        colorBlendAttachmentNorm.alphaBlendOp = VK_BLEND_OP_ADD; // Optional
    vector<VkPipelineColorBlendAttachmentState> colorBlendAttachments = {colorBlendAttachmentPos, colorBlendAttachmentNorm};
// printl(colorBlendAttachments.size());
    VkPipelineColorBlendStateCreateInfo colorBlending{};
        colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlending.logicOpEnable = VK_FALSE;
        colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
        colorBlending.attachmentCount = colorBlendAttachments.size();
        colorBlending.pAttachments = colorBlendAttachments.data();
        colorBlending.blendConstants[0] = 0.0f; // Optional
        colorBlending.blendConstants[1] = 0.0f; // Optional
        colorBlending.blendConstants[2] = 0.0f; // Optional
        colorBlending.blendConstants[3] = 0.0f; // Optional

    VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 0;
        pipelineLayoutInfo.pSetLayouts = NULL;
        pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
        pipelineLayoutInfo.pPushConstantRanges = NULL; // Optional

    VK_CHECK(vkCreatePipelineLayout(device, &pipelineLayoutInfo, NULL, &rayGenPipelineLayout));

    VkPipelineDepthStencilStateCreateInfo depthStencil = {VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO};    
        depthStencil.depthTestEnable = VK_TRUE;
        depthStencil.depthWriteEnable = VK_TRUE;
        depthStencil.depthCompareOp = VK_COMPARE_OP_GREATER_OR_EQUAL;
        depthStencil.depthBoundsTestEnable = VK_FALSE;
        depthStencil.minDepthBounds = 0.0f; // Optional
        depthStencil.maxDepthBounds = 1.0f; // Optional
        depthStencil.stencilTestEnable = VK_FALSE;
        depthStencil.front = {}; // Optional
        depthStencil.back = {}; // Optional
    VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};
    VkGraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.stageCount = 2;
        pipelineInfo.pStages = shaderStages;
        pipelineInfo.pVertexInputState = &vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &inputAssembly;
        pipelineInfo.pViewportState = &viewportState;
        pipelineInfo.pRasterizationState = &rasterizer;
        pipelineInfo.pMultisampleState = &multisampling;
        pipelineInfo.pDepthStencilState = &depthStencil; // Optional
        pipelineInfo.pColorBlendState = &colorBlending;
        pipelineInfo.pDynamicState = &dynamicState;
        
        pipelineInfo.layout = rayGenPipelineLayout;
        
        pipelineInfo.renderPass = rayGenRenderPass;
        pipelineInfo.subpass = 0; //first and only subpass in renderPass will use the pipeline
        
        pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
        pipelineInfo.basePipelineIndex = -1; // Optional

        VK_CHECK(vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, NULL, &rayGenPipeline));
}

void Renderer::create_N_Framebuffers(vector<VkFramebuffer> &framebuffers, vector<vector<VkImageView>> &views, VkRenderPass renderPass, u32 N, u32 Width, u32 Height){
    framebuffers.resize(N);

    for (u32 i = 0; i < N; i++) {
        vector<VkImageView> attachments = {};
        for (auto viewVec : views) {
            attachments.push_back(viewVec[i]);
        }

        VkFramebufferCreateInfo framebufferInfo = {};
            framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferInfo.renderPass = renderPass;
            framebufferInfo.attachmentCount = attachments.size();
            framebufferInfo.pAttachments    = attachments.data();
            framebufferInfo.width  = Width;
            framebufferInfo.height = Height;
            framebufferInfo.layers = 1;

        VK_CHECK(vkCreateFramebuffer(device, &framebufferInfo, NULL, &framebuffers[i]));
    }
}

void Renderer::create_Command_Pool(){
    //TODO: could be saved after physical device choosen
    //btw TODO is ignored
    QueueFamilyIndices queueFamilyIndices = find_Queue_Families(physicalDevice);

    VkCommandPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicalAndCompute.value();

    VK_CHECK(vkCreateCommandPool(device, &poolInfo, NULL, &commandPool));
}

void Renderer::create_Command_Buffers(vector<VkCommandBuffer>& commandBuffers, u32 size){
    commandBuffers.resize(size);

    VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = commandPool;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = commandBuffers.size();
    
    VK_CHECK(vkAllocateCommandBuffers(device, &allocInfo, commandBuffers.data())); 
}

void Renderer::record_Command_Buffer_Graphical(VkCommandBuffer commandBuffer, u32 imageIndex){
    VkCommandBufferBeginInfo beginInfo = {};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = 0; // Optional
        beginInfo.pInheritanceInfo = NULL; // Optional

    VK_CHECK(vkBeginCommandBuffer(commandBuffer, &beginInfo));

    transition_Image_Layout_Cmdb(commandBuffer, raytracedImages[currentFrame], VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
        VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
        0, VK_ACCESS_SHADER_READ_BIT
    );
    // transition_Image_Layout_Cmdb(commandBuffer, rayGenPosMatImages[currentFrame], VK_FORMAT_UNDEFINED, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
    //     VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
    //     0, VK_ACCESS_SHADER_READ_BIT
    // );
    // VkCopyImageToImageInfoEXT info = {};
    // info.sType = VK_STRUCTURE_TYPE_COPY_IMAGE_TO_IMAGE_INFO_EXT;
    // info.
    // vkCopyImageToImageEXT(device, info)

    VkClearValue clearColor = {{{0.111f, 0.444f, 0.666f, 1.0f}}};
    VkRenderPassBeginInfo renderPassInfo = {};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = graphicalRenderPass;
        renderPassInfo.framebuffer = swapChainFramebuffers[imageIndex];
        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = swapChainExtent;
        renderPassInfo.clearValueCount = 1;
        renderPassInfo.pClearValues = &clearColor;
    
    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsLayout, 0, 1, &graphicalDescriptorSets[currentFrame], 0, NULL);
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

    VkViewport viewport = {};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width  = (float)(swapChainExtent.width );
        viewport.height = (float)(swapChainExtent.height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

    VkRect2D scissor = {};
        scissor.offset = {0, 0};
        scissor.extent = swapChainExtent;
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    vkCmdDraw(commandBuffer, 3, 1, 0, 0);
    // vkCmdDraw(commandBuffer, 3, 1, 0, 0);

    vkCmdEndRenderPass(commandBuffer);

    VK_CHECK(vkEndCommandBuffer(commandBuffer));
}

void Renderer::record_Command_Buffer_Compute(VkCommandBuffer commandBuffer){
    VkCommandBufferBeginInfo beginInfo = {};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = 0; // Optional
        beginInfo.pInheritanceInfo = NULL; // Optional

    VK_CHECK(vkBeginCommandBuffer(commandBuffer, &beginInfo));

    // raygen;
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, raytracePipeline);


    transition_Image_Layout_Cmdb(commandBuffer, raytracedImages[currentFrame], VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_LAYOUT_GENERAL,
        VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
        0, VK_ACCESS_SHADER_WRITE_BIT
    );

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, raytracePipeline);

    //move to argument  
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, raytraceLayout, 0, 1, &raytraceDescriptorSets[currentFrame], 0, 0);

    // vkCmdDispatch(commandBuffer, 1920/8, 1080/4, 1);
    // float data[32] = {111, 222};
    // for(i32 i=0; i<100; i++){
    //     vkCmdPushConstants(commandBuffer, raytraceLayout, VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(float)*2 , &data);
    //     vkCmdDispatch(commandBuffer, 5, 5, 45);
    //     data[0]++;
    //     data[1]++;
    // }
    vkCmdDispatch(commandBuffer, window.width/8, window.height/4, 1);

    VK_CHECK(vkEndCommandBuffer(commandBuffer));
}

void Renderer::create_Sync_Objects(){
    imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    // blockifyFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    // copyFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    // mapFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    raytraceFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    rayGenFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);

       rayGenInFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
    graphicalInFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
    // blockifyInFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
    // copyInFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
    // mapInFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
      raytraceInFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

    VkSemaphoreCreateInfo semaphoreInfo = {};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    VkFenceCreateInfo fenceInfo{};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (i32 i=0; i < MAX_FRAMES_IN_FLIGHT; i++){
        VK_CHECK(vkCreateSemaphore(device, &semaphoreInfo, NULL,  &imageAvailableSemaphores[i]));
        VK_CHECK(vkCreateSemaphore(device, &semaphoreInfo, NULL,  &renderFinishedSemaphores[i]));
        // VK_CHECK(vkCreateSemaphore(device, &semaphoreInfo, NULL, &blockifyFinishedSemaphores[i]));
        // VK_CHECK(vkCreateSemaphore(device, &semaphoreInfo, NULL, &copyFinishedSemaphores[i]));
        // VK_CHECK(vkCreateSemaphore(device, &semaphoreInfo, NULL, &mapFinishedSemaphores[i]));
        VK_CHECK(vkCreateSemaphore(device, &semaphoreInfo, NULL, &raytraceFinishedSemaphores[i]));
        VK_CHECK(vkCreateSemaphore(device, &semaphoreInfo, NULL,  &rayGenFinishedSemaphores[i]));
        VK_CHECK(vkCreateFence(device, &fenceInfo, NULL, &graphicalInFlightFences[i]));
        // VK_CHECK(vkCreateFence(device, &fenceInfo, NULL,   &blockifyInFlightFences[i]));
        // VK_CHECK(vkCreateFence(device, &fenceInfo, NULL,   &copyInFlightFences[i]));
        // VK_CHECK(vkCreateFence(device, &fenceInfo, NULL,   &mapInFlightFences[i]));
        VK_CHECK(vkCreateFence(device, &fenceInfo, NULL,   &raytraceInFlightFences[i]));
        VK_CHECK(vkCreateFence(device, &fenceInfo, NULL,    &rayGenInFlightFences[i]));
    }
}

void Renderer::start_Frame(){
    //maybe waiting for all fences here
}

void Renderer::startRaygen(){
    vkWaitForFences(device, 1, &rayGenInFlightFences[currentFrame], VK_TRUE, UINT32_MAX);
    vkResetFences  (device, 1, &rayGenInFlightFences[currentFrame]);

    VkCommandBuffer &commandBuffer = rayGenCommandBuffers[currentFrame];
    vkResetCommandBuffer(commandBuffer, 0);
    VkCommandBufferBeginInfo beginInfo = {};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = 0; // Optional
        beginInfo.pInheritanceInfo = NULL; // Optional
    VK_CHECK(vkBeginCommandBuffer(commandBuffer, &beginInfo));

    vector<VkClearValue> clearColors = {
        {{0.111f, 0.666f, 0.111f, 1.0f}}, 
        {{0.111f, 0.666f, 0.111f, 1.0f}}, 
        {{0.0, 0}}
    };
    VkRenderPassBeginInfo renderPassInfo = {};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = rayGenRenderPass;
        renderPassInfo.framebuffer = rayGenFramebuffers[currentFrame];
        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = swapChainExtent;
        renderPassInfo.clearValueCount = clearColors.size();
        renderPassInfo.pClearValues    = clearColors.data();
    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, rayGenPipeline);

    VkViewport viewport = {};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width  = (float)(swapChainExtent.width );
        viewport.height = (float)(swapChainExtent.height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

    VkRect2D scissor = {};
        scissor.offset = {0, 0};
        scissor.extent = swapChainExtent;
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
}
void Renderer::RaygenMesh(Mesh &mesh){
    VkCommandBuffer &commandBuffer = rayGenCommandBuffers[currentFrame];
        VkBuffer vertexBuffers[] = {mesh.vertexes.buf[currentFrame]};
        VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
    vkCmdBindIndexBuffer(commandBuffer, mesh.indexes.buf[currentFrame], 0, VK_INDEX_TYPE_UINT32);
    vkCmdDrawIndexed(commandBuffer, mesh.icount, 1, 0, 0, 0);
}
void Renderer::endRaygen(){
    VkCommandBuffer &commandBuffer = rayGenCommandBuffers[currentFrame];
    vkCmdEndRenderPass(commandBuffer);
    VK_CHECK(vkEndCommandBuffer(commandBuffer));

    VkSubmitInfo submitInfo = {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &rayGenCommandBuffers[currentFrame];
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = &rayGenFinishedSemaphores[currentFrame];
    VK_CHECK(vkQueueSubmit(graphicsQueue, 1, &submitInfo, rayGenInFlightFences[currentFrame]));
}
void Renderer::startCompute(){
    VkCommandBuffer &commandBuffer = computeCommandBuffers[currentFrame];
    vkWaitForFences(device, 1, &raytraceInFlightFences[currentFrame], VK_TRUE, UINT32_MAX);
    vkResetFences  (device, 1, &raytraceInFlightFences[currentFrame]);

    
    vkResetCommandBuffer(computeCommandBuffers[currentFrame], /*VkCommandBufferResetFlagBits*/ 0);
    VkCommandBufferBeginInfo beginInfo = {};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = 0; // Optional
        beginInfo.pInheritanceInfo = NULL; // Optional
    VK_CHECK(vkBeginCommandBuffer(commandBuffer, &beginInfo));

    vkCmdFillBuffer(commandBuffer,    copyCounterBuffers[currentFrame], sizeof(int)*2, sizeof(int)*1, 0);
    vkCmdFillBuffer(commandBuffer, paletteCounterBuffers[currentFrame], sizeof(int)*0, sizeof(int)*1, 2);
    //TODO: remove
    copy_Whole_Image({16,16, 16*BLOCK_PALETTE_SIZE}, commandBuffer, 
    originBlockPaletteImages[currentFrame], raytraceBlockPaletteImages[currentFrame]);
}
void Renderer::startBlockify(){
    VkCommandBuffer &commandBuffer = computeCommandBuffers[currentFrame];

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, blockifyPipeline);
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, blockifyLayout, 0, 1, &blockifyDescriptorSets[currentFrame], 0, 0);
}
// allocates temp block in palette for every block that intersects with every mesh blockified
void Renderer::blockifyMesh(Mesh& mesh){
    VkCommandBuffer &commandBuffer = computeCommandBuffers[currentFrame];

    vkCmdPushConstants(commandBuffer, blockifyLayout, VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(mat4), &mesh.transform);
    vkCmdPushConstants(commandBuffer, blockifyLayout, VK_SHADER_STAGE_COMPUTE_BIT, sizeof(mat4), sizeof(int), &itime);
    // vkCmdDispatch(commandBuffer, (mesh.size.x+15+16*2)/16, 
    //                              (mesh.size.y+15+16*2)/16, 
    //                              (mesh.size.z+15+16*2)/16); //adds 1 block to size for each axis
    vkCmdDispatch(commandBuffer, 4, 
                                 4, 
                                 4); //adds 1 block to size for each axis
}
void Renderer::endBlockify(){
    VkCommandBuffer &commandBuffer = computeCommandBuffers[currentFrame];

    VkBufferMemoryBarrier copy_barrier{};
        copy_barrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
        copy_barrier.size = VK_WHOLE_SIZE;
        copy_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        copy_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        copy_barrier.buffer = copyCounterBuffers[currentFrame];
        copy_barrier.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT| VK_ACCESS_SHADER_READ_BIT;
        copy_barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT;
    // vkcmdbarrer
    vkCmdPipelineBarrier(
        commandBuffer,
        VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
        0,
        0, NULL,
        1, &copy_barrier,
        0, NULL
    );
    VkImageMemoryBarrier block_barrier{};
        block_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        block_barrier.oldLayout = VK_IMAGE_LAYOUT_GENERAL;
        block_barrier.newLayout = VK_IMAGE_LAYOUT_GENERAL;
        block_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        block_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        block_barrier.image = raytraceBlocksImages[currentFrame];
        block_barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        block_barrier.subresourceRange.baseMipLevel = 0;
        block_barrier.subresourceRange.levelCount = 1;
        block_barrier.subresourceRange.baseArrayLayer = 0;
        block_barrier.subresourceRange.layerCount = 1;
        block_barrier.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT| VK_ACCESS_SHADER_READ_BIT;
        block_barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT;
    vkCmdPipelineBarrier( 
        commandBuffer,
        VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
        VK_DEPENDENCY_BY_REGION_BIT,
        0, NULL,
        0, NULL,
        1, &block_barrier
    );
}

void Renderer::execCopies(){
    VkCommandBuffer &commandBuffer = computeCommandBuffers[currentFrame];

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, copyPipeline);//TODO:move away
        vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, copyLayout, 0, 1, &copyDescriptorSets[currentFrame], 0, 0);
        vkCmdDispatchIndirect(commandBuffer, copyCounterBuffers[currentFrame], 0);
        // vkCmdDispatch(commandBuffer, 16,16,16);
        VkImageMemoryBarrier barrier{};
            barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
            barrier.oldLayout = VK_IMAGE_LAYOUT_GENERAL;
            barrier.newLayout = VK_IMAGE_LAYOUT_GENERAL;
            barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barrier.image = raytraceBlockPaletteImages[currentFrame];
            barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            barrier.subresourceRange.baseMipLevel = 0;
            barrier.subresourceRange.levelCount = 1;
            barrier.subresourceRange.baseArrayLayer = 0;
            barrier.subresourceRange.layerCount = 1;
            barrier.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT| VK_ACCESS_SHADER_READ_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT;
        vkCmdPipelineBarrier(
            commandBuffer,
            VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
            0,
            0, NULL,
            0, NULL,
            1, &barrier
        );
}

void Renderer::startMap(){
    VkCommandBuffer &commandBuffer = computeCommandBuffers[currentFrame];
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, mapPipeline);
}
void Renderer::mapMesh(Mesh& mesh){
    VkCommandBuffer &commandBuffer = computeCommandBuffers[currentFrame];
    
    //setting dynamic descriptors
        VkDescriptorImageInfo
            modelVoxelsInfo = {};
            modelVoxelsInfo.imageView = mesh.voxels.view[currentFrame]; //CHANGE ME
            modelVoxelsInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
        VkDescriptorImageInfo 
            blocksInfo = {};
            blocksInfo.imageView = raytraceBlocksImageViews[currentFrame];
            blocksInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
        VkDescriptorImageInfo 
            blockPaletteInfo = {};
            blockPaletteInfo.imageView = raytraceBlockPaletteImageViews[currentFrame];
            blockPaletteInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;

        VkWriteDescriptorSet 
            modelVoxelsWrite = {VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET};
            modelVoxelsWrite.dstSet = NULL;
            modelVoxelsWrite.dstBinding = 0;
            modelVoxelsWrite.dstArrayElement = 0;
            modelVoxelsWrite.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
            modelVoxelsWrite.descriptorCount = 1;
            modelVoxelsWrite.pImageInfo = &modelVoxelsInfo;
        VkWriteDescriptorSet 
            blocksWrite = {VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET};
            blocksWrite.dstSet = NULL;
            blocksWrite.dstBinding = 1;
            blocksWrite.dstArrayElement = 0;
            blocksWrite.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
            blocksWrite.descriptorCount = 1;
            blocksWrite.pImageInfo = &blocksInfo;
        VkWriteDescriptorSet 
            blockPaletteWrite = {VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET};
            blockPaletteWrite.dstSet = NULL;
            blockPaletteWrite.dstBinding = 2;
            blockPaletteWrite.dstArrayElement = 0;
            blockPaletteWrite.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
            blockPaletteWrite.descriptorCount = 1;
            blockPaletteWrite.pImageInfo = &blockPaletteInfo;
        vector<VkWriteDescriptorSet> descriptorWrites = {modelVoxelsWrite, blocksWrite, blockPaletteWrite};

    vkCmdPushDescriptorSetKHR(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, mapLayout, 0, descriptorWrites.size(), descriptorWrites.data());
    vkCmdDispatch(commandBuffer, mesh.size.x, mesh.size.y, mesh.size.z);
}
void Renderer::endMap(){
    VkCommandBuffer &commandBuffer = computeCommandBuffers[currentFrame];

    VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout = VK_IMAGE_LAYOUT_GENERAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_GENERAL;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = raytraceBlockPaletteImages[currentFrame];
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = 1;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;
        barrier.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT | VK_ACCESS_SHADER_READ_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT  | VK_ACCESS_SHADER_WRITE_BIT;
    vkCmdPipelineBarrier(
        commandBuffer,
        VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
        0,
        0, NULL,
        0, NULL,
        1, &barrier
    );
}
void Renderer::raytrace(){
    VkCommandBuffer &commandBuffer = computeCommandBuffers[currentFrame];

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, raytracePipeline);
        transition_Image_Layout_Cmdb(commandBuffer, raytracedImages[currentFrame], VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_LAYOUT_GENERAL,
            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
            0, VK_ACCESS_SHADER_WRITE_BIT);
        // copy_Whole_Image({8,8,8}, commandBuffer, 
        //     originBlocksImages[currentFrame],       raytraceBlocksImages[currentFrame]);

        // vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, raytracePipeline);
        vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, raytraceLayout, 0, 1, &raytraceDescriptorSets[currentFrame], 0, 0);

        itime++;
        vkCmdPushConstants(commandBuffer, raytraceLayout, VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(i32)*1 , &itime);
        vkCmdDispatch(commandBuffer, window.width/8, window.height/4, 1);
        VK_CHECK(vkEndCommandBuffer(commandBuffer));
}
void Renderer::endCompute(){
        vector<VkSemaphore> computeWaitSemaphores = {rayGenFinishedSemaphores[currentFrame]};
        VkPipelineStageFlags computeWaitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        VkSubmitInfo submitInfo = {};
            submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            submitInfo.pWaitDstStageMask = computeWaitStages;
            submitInfo.waitSemaphoreCount = computeWaitSemaphores.size();
            submitInfo.pWaitSemaphores    = computeWaitSemaphores.data();
            submitInfo.commandBufferCount = 1;
            submitInfo.pCommandBuffers = &computeCommandBuffers[currentFrame];
            submitInfo.signalSemaphoreCount = 1;
            submitInfo.pSignalSemaphores = &raytraceFinishedSemaphores[currentFrame];
    VK_CHECK(vkQueueSubmit(computeQueue, 1, &submitInfo, raytraceInFlightFences[currentFrame]));
}
void Renderer::present(){
    VkCommandBuffer &commandBuffer = graphicalCommandBuffers[currentFrame];

    VkResult result = vkAcquireNextImageKHR(device, swapchain, UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);
    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        cout << KRED"failed to present swap chain image!\n" KEND;
        recreate_Swapchain();
        return; // can be avoided, but it is just 1 frame 
    } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        cout << KRED"failed to acquire swap chain image!\n" KEND;
        exit(result);
    }
    vkWaitForFences(device, 1, &graphicalInFlightFences[currentFrame], VK_TRUE, UINT32_MAX);
    vkResetFences  (device, 1, &graphicalInFlightFences[currentFrame]);

    vkResetCommandBuffer(graphicalCommandBuffers[currentFrame], 0);
    VkCommandBufferBeginInfo beginInfo = {};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = 0; // Optional
        beginInfo.pInheritanceInfo = NULL; // Optional

    VK_CHECK(vkBeginCommandBuffer(commandBuffer, &beginInfo));

    transition_Image_Layout_Cmdb(commandBuffer, raytracedImages[currentFrame], VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
        VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
        0, VK_ACCESS_SHADER_READ_BIT
    );

    VkClearValue clearColor = {{{0.111f, 0.444f, 0.666f, 1.0f}}};
    VkRenderPassBeginInfo renderPassInfo = {};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = graphicalRenderPass;
        renderPassInfo.framebuffer = swapChainFramebuffers[imageIndex];
        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = swapChainExtent;
        renderPassInfo.clearValueCount = 1;
        renderPassInfo.pClearValues = &clearColor;
    
    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsLayout, 0, 1, &graphicalDescriptorSets[currentFrame], 0, NULL);
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

    VkViewport viewport = {};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width  = (float)(swapChainExtent.width );
        viewport.height = (float)(swapChainExtent.height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

    VkRect2D scissor = {};
        scissor.offset = {0, 0};
        scissor.extent = swapChainExtent;
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
    
    vkCmdDraw(commandBuffer, 3, 1, 0, 0);
    // vkCmdDraw(commandBuffer, 3, 1, 0, 0);

    vkCmdEndRenderPass(commandBuffer);

    VK_CHECK(vkEndCommandBuffer(commandBuffer));

    // vector<VkSemaphore> waitSemaphores = {imageAvailableSemaphores[currentFrame]};
    vector<VkSemaphore> waitSemaphores = {raytraceFinishedSemaphores[currentFrame], imageAvailableSemaphores[currentFrame]};
    vector<VkSemaphore> signalSemaphores = {renderFinishedSemaphores[currentFrame]};
    //TODO: replace with VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_VERTEX_INPUT_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    // VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    // VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    VkSubmitInfo submitInfo = {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.waitSemaphoreCount = waitSemaphores.size();
        submitInfo.pWaitSemaphores    = waitSemaphores.data();
        submitInfo.pWaitDstStageMask = waitStages;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &graphicalCommandBuffers[currentFrame];
        submitInfo.signalSemaphoreCount = signalSemaphores.size();
        submitInfo.pSignalSemaphores    = signalSemaphores.data();

    VK_CHECK(vkQueueSubmit(graphicsQueue, 1, &submitInfo, graphicalInFlightFences[currentFrame]));

    VkSwapchainKHR swapchains[] = {swapchain};
    VkPresentInfoKHR presentInfo = {};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = signalSemaphores.size();
        presentInfo.pWaitSemaphores = signalSemaphores.data();
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapchains;
        presentInfo.pImageIndices = &imageIndex;
        presentInfo.pResults = NULL; // Optional

    result = vkQueuePresentKHR(presentQueue, &presentInfo);
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
        cout << KRED"failed to present swap chain image!\n" KEND;
        recreate_Swapchain();
    } else if (result != VK_SUCCESS) {
        cout << KRED"failed to present swap chain image!\n" KEND;
        exit(result);
    }
}
void Renderer::end_Frame(){
    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void Renderer::create_Image_Storages(vector<VkImage> &images, vector<VmaAllocation> &allocs, vector<VkImageView> &views, 
    VkImageType type, VkFormat format, VkImageUsageFlags usage, VkImageAspectFlags aspect, VkImageLayout layout, VkPipelineStageFlagBits pipeStage, VkAccessFlags access, 
    uvec3 hwd){
    
    images.resize(MAX_FRAMES_IN_FLIGHT);
    allocs.resize(MAX_FRAMES_IN_FLIGHT);
     views.resize(MAX_FRAMES_IN_FLIGHT);
    
    for (i32 i=0; i < MAX_FRAMES_IN_FLIGHT; i++){
        VkImageCreateInfo imageInfo = {};
            imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
            imageInfo.imageType = type;
            imageInfo.format = format;
            imageInfo.mipLevels = 1;
            imageInfo.arrayLayers = 1;
            imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
            imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
            imageInfo.usage = usage;
            imageInfo.extent.height = hwd.x;
            imageInfo.extent.width  = hwd.y;
            imageInfo.extent.depth  = hwd.z;
            imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        VmaAllocationCreateInfo allocInfo = {};
            allocInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
        VK_CHECK(vmaCreateImage(VMAllocator, &imageInfo, &allocInfo, &images[i], &allocs[i], NULL));

        VkImageViewCreateInfo viewInfo = {};
            viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            viewInfo.image = images[i];
            if(type == VK_IMAGE_TYPE_2D){
                viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            } else if(type == VK_IMAGE_TYPE_3D) {
                viewInfo.viewType = VK_IMAGE_VIEW_TYPE_3D;
            }
            viewInfo.format = format;
            viewInfo.subresourceRange.aspectMask = aspect;
            viewInfo.subresourceRange.baseMipLevel = 0;
            viewInfo.subresourceRange.baseArrayLayer = 0;
            viewInfo.subresourceRange.levelCount = 1;
            viewInfo.subresourceRange.layerCount = 1;
        VK_CHECK(vkCreateImageView(device, &viewInfo, NULL, &views[i]));

        transition_Image_Layout_Singletime(images[i], format, aspect, VK_IMAGE_LAYOUT_UNDEFINED, layout,
            VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, pipeStage,
            0, access
        );
    }
}
//fill manually with cmd or copy
void Renderer::create_Buffer_Storages(vector<VkBuffer> &buffers, vector<VmaAllocation> &allocs, VkBufferUsageFlags usage, u32 size){
    buffers.resize(MAX_FRAMES_IN_FLIGHT);
    allocs.resize(MAX_FRAMES_IN_FLIGHT);
    
    for (i32 i=0; i < MAX_FRAMES_IN_FLIGHT; i++){
        VkBufferCreateInfo bufferInfo = {};
            bufferInfo.size = size;
            bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
            bufferInfo.usage = usage;
        VmaAllocationCreateInfo allocInfo = {};
            allocInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
        VK_CHECK(vmaCreateBuffer(VMAllocator, &bufferInfo, &allocInfo, &buffers[i], &allocs[i], NULL));
    }
}
//TODO: need to find memory, verify flags
//make so each chunk is stored in staging buffer and then copied to fast memory
tuple<Buffer, Buffer> Renderer::create_RayGen_VertexBuffers(vector<Vertex> vertices, vector<u32> indices){
    VkDeviceSize bufferSizeV = sizeof(Vertex)*vertices.size();
    cout << bufferSizeV;
    VkDeviceSize bufferSizeI = sizeof(u32   )* indices.size();

    vector<VkBuffer> vertexBuffers(MAX_FRAMES_IN_FLIGHT);
    vector<VkBuffer>  indexBuffers(MAX_FRAMES_IN_FLIGHT);
    vector<VmaAllocation> vertexAllocations(MAX_FRAMES_IN_FLIGHT);
    vector<VmaAllocation>  indexAllocations(MAX_FRAMES_IN_FLIGHT);

    VkBufferCreateInfo stagingBufferInfo = {VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
        stagingBufferInfo.size = bufferSizeV;
        stagingBufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    VmaAllocationCreateInfo stagingAllocInfo = {};
        stagingAllocInfo.usage = VMA_MEMORY_USAGE_AUTO;
        stagingAllocInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
    VkBuffer stagingBufferV = {};
    VmaAllocation stagingAllocationV = {};
    VkBuffer stagingBufferI = {};
    VmaAllocation stagingAllocationI = {};
    vmaCreateBuffer(VMAllocator, &stagingBufferInfo, &stagingAllocInfo, &stagingBufferV, &stagingAllocationV, NULL);
    stagingBufferInfo.size = bufferSizeI;
    vmaCreateBuffer(VMAllocator, &stagingBufferInfo, &stagingAllocInfo, &stagingBufferI, &stagingAllocationI, NULL);

    void* data;
assert (VMAllocator);
assert (stagingAllocationV);
assert (&data);
    vmaMapMemory(VMAllocator, stagingAllocationV, &data);
        memcpy(data, vertices.data(), bufferSizeV);
    vmaUnmapMemory(VMAllocator, stagingAllocationV);

    vmaMapMemory(VMAllocator, stagingAllocationI, &data);
        memcpy(data, indices.data(), bufferSizeI);
    vmaUnmapMemory(VMAllocator, stagingAllocationI);

    for(i32 i=0; i<MAX_FRAMES_IN_FLIGHT; i++){
        VkBufferCreateInfo bufferInfo = {VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
            bufferInfo.size = bufferSizeV;
            bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        VmaAllocationCreateInfo allocInfo = {};
            allocInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
        vmaCreateBuffer(VMAllocator, &bufferInfo, &allocInfo, &vertexBuffers[i], &vertexAllocations[i], NULL);
            bufferInfo.size = bufferSizeI;
            bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
        vmaCreateBuffer(VMAllocator, &bufferInfo, &allocInfo, &indexBuffers[i], &indexAllocations[i], NULL);
               
        copy_Buffer(stagingBufferV, vertexBuffers[i], bufferSizeV);
        copy_Buffer(stagingBufferI,  indexBuffers[i], bufferSizeI);
    }

    vmaDestroyBuffer(VMAllocator, stagingBufferV, stagingAllocationV);
    vmaDestroyBuffer(VMAllocator, stagingBufferI, stagingAllocationI);

    return {
        {vertexBuffers, vertexAllocations}, 
        {indexBuffers, indexAllocations}
    };
}
Image Renderer::create_RayTrace_VoxelImages(MatID_t* voxels, ivec3 size){
    VkDeviceSize bufferSize = sizeof(MatID_t)*size.x*size.y*size.z;

    vector<VkImage>   images(MAX_FRAMES_IN_FLIGHT);
    vector<VkImageView>  views(MAX_FRAMES_IN_FLIGHT);
    vector<VmaAllocation>  allocations(MAX_FRAMES_IN_FLIGHT);

    create_Image_Storages(images, allocations, views,
        VK_IMAGE_TYPE_3D,
        VK_FORMAT_R8_UINT,
        VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        VK_IMAGE_ASPECT_COLOR_BIT,
        VK_IMAGE_LAYOUT_GENERAL,
        VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
        VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_MEMORY_READ_BIT,
        {16, 16, 16}); //TODO: dynamic

    VkBufferCreateInfo stagingBufferInfo = {VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
        stagingBufferInfo.size = bufferSize;
        stagingBufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    VmaAllocationCreateInfo stagingAllocInfo = {};
        stagingAllocInfo.usage = VMA_MEMORY_USAGE_AUTO;
        stagingAllocInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
    VkBuffer stagingBuffer = {};
    VmaAllocation stagingAllocation = {};
    vmaCreateBuffer(VMAllocator, &stagingBufferInfo, &stagingAllocInfo, &stagingBuffer, &stagingAllocation, NULL);

    void* data;
    vmaMapMemory(VMAllocator, stagingAllocation, &data);
        memcpy(data, voxels, bufferSize);
    vmaUnmapMemory(VMAllocator, stagingAllocation);

    for(i32 i=0; i<MAX_FRAMES_IN_FLIGHT; i++){
        copy_Buffer(stagingBuffer, images[i], bufferSize, {16,16,16}, VK_IMAGE_LAYOUT_GENERAL);
    }
    return {images, views, allocations};
}

static u32 STORAGE_BUFFER_DESCRIPTOR_COUNT = 0;
static u32 STORAGE_IMAGE_DESCRIPTOR_COUNT = 0;
static u32 COMBINED_IMAGE_SAMPLER_DESCRIPTOR_COUNT = 0;
static void count_descriptor(const VkDescriptorType type){
    if(type == VK_DESCRIPTOR_TYPE_STORAGE_BUFFER){
        STORAGE_BUFFER_DESCRIPTOR_COUNT++;
    } else if(type == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER){
        COMBINED_IMAGE_SAMPLER_DESCRIPTOR_COUNT++;
    } else if(type == VK_DESCRIPTOR_TYPE_STORAGE_IMAGE){
        STORAGE_IMAGE_DESCRIPTOR_COUNT++;
    } else {
        cout << KRED "ADD DESCRIPTOR TO COUNTER\n" KEND;
        abort();
    }
}
//in order
static void create_Descriptor_Set_Layout_Helper(vector<VkDescriptorType> descriptorTypes, VkShaderStageFlags stageFlags, VkDescriptorSetLayout& layout, VkDevice device, VkDescriptorSetLayoutCreateFlags flags = 0){
    vector<VkDescriptorSetLayoutBinding> bindings = {};

    for (i32 i=0; i<descriptorTypes.size(); i++) {
        count_descriptor(descriptorTypes[i]);
        
        VkDescriptorSetLayoutBinding 
            bind = {};
            bind.binding = i;
            bind.descriptorType = descriptorTypes[i];
            bind.descriptorCount = 1;
            bind.stageFlags = stageFlags;
        bindings.push_back(bind);
    }

    VkDescriptorSetLayoutCreateInfo 
        layoutInfo = {};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.flags = flags;
        layoutInfo.bindingCount = bindings.size();
        layoutInfo.pBindings    = bindings.data();
    VK_CHECK(vkCreateDescriptorSetLayout(device, &layoutInfo, NULL, &layout));
}

void Renderer::create_Descriptor_Set_Layouts(){
    //descriptors are counted after this
    create_Descriptor_Set_Layout_Helper({
        VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, //palette counter
        VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, //copy buffer - XYZ for indirect and copy operaitions
        VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,  //blocks
        }, 
        VK_SHADER_STAGE_COMPUTE_BIT, blockifyDescriptorSetLayout, device);
    
    create_Descriptor_Set_Layout_Helper({
        VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,  //1frame voxel palette
        VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, //copy buffer - XYZ for indirect and copy operaitions
        }, 
        VK_SHADER_STAGE_COMPUTE_BIT, copyDescriptorSetLayout, device);

    create_Descriptor_Set_Layout_Helper({
        VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, // modelVoxels
        VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, // blocks
        VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, // blockPalette
        }, 
        VK_SHADER_STAGE_COMPUTE_BIT, mapDescriptorSetLayout, device,
        VK_DESCRIPTOR_SET_LAYOUT_CREATE_PUSH_DESCRIPTOR_BIT_KHR);

    create_Descriptor_Set_Layout_Helper({
        VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, //pos mat
        VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, //norm
        VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, //blocks
        VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, //block palette
        VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, //voxel palette
        VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, //out frame (raytraced)
        }, 
        VK_SHADER_STAGE_COMPUTE_BIT, raytraceDescriptorSetLayout, device);

    create_Descriptor_Set_Layout_Helper({
        VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, //in frame (raytraced)
        }, 
        VK_SHADER_STAGE_FRAGMENT_BIT, graphicalDescriptorSetLayout, device);
}

void Renderer::create_Descriptor_Pool(){
    printl(STORAGE_IMAGE_DESCRIPTOR_COUNT);
    printl(COMBINED_IMAGE_SAMPLER_DESCRIPTOR_COUNT);
    printl(STORAGE_BUFFER_DESCRIPTOR_COUNT);

    VkDescriptorPoolSize STORAGE_IMAGE_PoolSize = {};
        STORAGE_IMAGE_PoolSize.type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
        STORAGE_IMAGE_PoolSize.descriptorCount = STORAGE_IMAGE_DESCRIPTOR_COUNT;
    VkDescriptorPoolSize COMBINED_IMAGE_SAMPLER_PoolSize = {};
        COMBINED_IMAGE_SAMPLER_PoolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        COMBINED_IMAGE_SAMPLER_PoolSize.descriptorCount = COMBINED_IMAGE_SAMPLER_DESCRIPTOR_COUNT;
    VkDescriptorPoolSize STORAGE_BUFFER_PoolSize = {};
        STORAGE_BUFFER_PoolSize.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        STORAGE_BUFFER_PoolSize.descriptorCount = STORAGE_BUFFER_DESCRIPTOR_COUNT;

    vector<VkDescriptorPoolSize> poolSizes = {
        STORAGE_IMAGE_PoolSize, 
        COMBINED_IMAGE_SAMPLER_PoolSize, 
        STORAGE_BUFFER_PoolSize};

    VkDescriptorPoolCreateInfo poolInfo = {};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = poolSizes.size();
        poolInfo.pPoolSizes = poolSizes.data();
        poolInfo.maxSets = MAX_FRAMES_IN_FLIGHT*5; //becuase frames_in_flight multiply of 5 differents sets, each for shader 

    VK_CHECK(vkCreateDescriptorPool(device, &poolInfo, NULL, &descriptorPool));
}

static void allocate_Descriptors_helper(vector<VkDescriptorSet>& sets, VkDescriptorSetLayout layout, VkDescriptorPool pool, VkDevice device){
    sets.resize(MAX_FRAMES_IN_FLIGHT);
    vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, layout);
    VkDescriptorSetAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = pool;
        allocInfo.descriptorSetCount = layouts.size();
        allocInfo.pSetLayouts        = layouts.data();
    VK_CHECK(vkAllocateDescriptorSets(device, &allocInfo, sets.data()));
}
void Renderer::allocate_Descriptors(){
    allocate_Descriptors_helper( raytraceDescriptorSets,  raytraceDescriptorSetLayout, descriptorPool, device); 
    allocate_Descriptors_helper( blockifyDescriptorSets,  blockifyDescriptorSetLayout, descriptorPool, device); 
    allocate_Descriptors_helper(     copyDescriptorSets,      copyDescriptorSetLayout, descriptorPool, device); 
    //we do not allocate this because it's descriptors are managed trough push_descriptor mechanism
    // allocate_Descriptors_helper(      mapDescriptorSets,       mapDescriptorSetLayout, descriptorPool, device); 
    allocate_Descriptors_helper(graphicalDescriptorSets, graphicalDescriptorSetLayout, descriptorPool, device);     
}

// static void setup_descriptors_helper(vector<vector<VkImageView>> views
void Renderer::setup_Blockify_Descriptors(){
    for (u32 i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        VkDescriptorBufferInfo paletteCounterBufferInfo = {};
            paletteCounterBufferInfo.buffer = paletteCounterBuffers[i];
            paletteCounterBufferInfo.offset = 0;
            paletteCounterBufferInfo.range = VK_WHOLE_SIZE;
        VkDescriptorBufferInfo copyCounterBufferInfo = {};
            copyCounterBufferInfo.buffer = copyCounterBuffers[i];
            copyCounterBufferInfo.offset = 0;
            copyCounterBufferInfo.range = VK_WHOLE_SIZE;
        VkDescriptorImageInfo inputBlocksInfo = {};
            inputBlocksInfo.imageView = raytraceBlocksImageViews[i];
            inputBlocksInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;

        VkWriteDescriptorSet paletteCounterWrite = {VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET};
            paletteCounterWrite.dstSet = blockifyDescriptorSets[i];
            paletteCounterWrite.dstBinding = 0;
            paletteCounterWrite.dstArrayElement = 0;
            paletteCounterWrite.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
            paletteCounterWrite.descriptorCount = 1;
            paletteCounterWrite.pBufferInfo = &paletteCounterBufferInfo;
        VkWriteDescriptorSet copyCounterWrite = {VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET};
            copyCounterWrite.dstSet = blockifyDescriptorSets[i];
            copyCounterWrite.dstBinding = 1;
            copyCounterWrite.dstArrayElement = 0;
            copyCounterWrite.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
            copyCounterWrite.descriptorCount = 1;
            copyCounterWrite.pBufferInfo = &copyCounterBufferInfo;
        VkWriteDescriptorSet blocksWrite = {VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET};
            blocksWrite.dstSet = blockifyDescriptorSets[i];
            blocksWrite.dstBinding = 2;
            blocksWrite.dstArrayElement = 0;
            blocksWrite.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
            blocksWrite.descriptorCount = 1;
            blocksWrite.pImageInfo = &inputBlocksInfo;
        vector<VkWriteDescriptorSet> descriptorWrites = {paletteCounterWrite, copyCounterWrite, blocksWrite};

        vkUpdateDescriptorSets(device, descriptorWrites.size(), descriptorWrites.data(), 0, NULL);
    }
}
void Renderer::setup_Copy_Descriptors(){
    for (u32 i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        VkDescriptorImageInfo inputBlockPaletteInfo = {};
            inputBlockPaletteInfo.imageView = raytraceBlockPaletteImageViews[i];
            inputBlockPaletteInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
        VkDescriptorBufferInfo copyCounterBufferInfo = {};
            copyCounterBufferInfo.buffer = copyCounterBuffers[i];
            copyCounterBufferInfo.offset = 0;
            copyCounterBufferInfo.range = VK_WHOLE_SIZE;

        VkWriteDescriptorSet blockPaletteWrite = {VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET};
            blockPaletteWrite.dstSet = copyDescriptorSets[i];
            blockPaletteWrite.dstBinding = 0;
            blockPaletteWrite.dstArrayElement = 0;
            blockPaletteWrite.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
            blockPaletteWrite.descriptorCount = 1;
            blockPaletteWrite.pImageInfo = &inputBlockPaletteInfo;
        VkWriteDescriptorSet copyCounterWrite = {VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET};
            copyCounterWrite.dstSet = copyDescriptorSets[i];
            copyCounterWrite.dstBinding = 1;
            copyCounterWrite.dstArrayElement = 0;
            copyCounterWrite.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
            copyCounterWrite.descriptorCount = 1;
            copyCounterWrite.pBufferInfo = &copyCounterBufferInfo;
        vector<VkWriteDescriptorSet> descriptorWrites = {blockPaletteWrite, copyCounterWrite};

        vkUpdateDescriptorSets(device, descriptorWrites.size(), descriptorWrites.data(), 0, NULL);
    }
}
//SHOULD NOT EXIST push descriptors to command buffer instead
void Renderer::setup_Map_Descriptors(){
    // for (u32 i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
    // }
}
void Renderer::setup_Raytrace_Descriptors(){
    for (u32 i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        VkDescriptorImageInfo inputPosMatInfo = {};
            inputPosMatInfo.imageView = rayGenPosMatImageViews[i];
            inputPosMatInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
        VkDescriptorImageInfo inputNormInfo = {};
            inputNormInfo.imageView = rayGenNormImageViews[i];
            inputNormInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
        VkDescriptorImageInfo inputBlockInfo = {};
            inputBlockInfo.imageView = raytraceBlocksImageViews[i];
            inputBlockInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
        VkDescriptorImageInfo inputBlockPaletteInfo = {};
            inputBlockPaletteInfo.imageView = raytraceBlockPaletteImageViews[i];
            inputBlockPaletteInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
        VkDescriptorImageInfo inputVoxelPaletteInfo = {};
            inputVoxelPaletteInfo.imageView = raytraceVoxelPaletteImageViews[i];
            inputVoxelPaletteInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
        VkDescriptorImageInfo outputFrameInfo = {};
            outputFrameInfo.imageView = raytracedImageViews[i];
            outputFrameInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
            
        VkWriteDescriptorSet PosMatWrite = {VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET};
            PosMatWrite.dstSet = raytraceDescriptorSets[i];
            PosMatWrite.dstBinding = 0;
            PosMatWrite.dstArrayElement = 0;
            PosMatWrite.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
            PosMatWrite.descriptorCount = 1;
            PosMatWrite.pImageInfo = &inputPosMatInfo;
        VkWriteDescriptorSet NormWrite = {VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET};
            NormWrite.dstSet = raytraceDescriptorSets[i];
            NormWrite.dstBinding = 1;
            NormWrite.dstArrayElement = 0;
            NormWrite.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
            NormWrite.descriptorCount = 1;
            NormWrite.pImageInfo = &inputNormInfo;
        VkWriteDescriptorSet blockWrite = {VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET};
            blockWrite.dstSet = raytraceDescriptorSets[i];
            blockWrite.dstBinding = 2;
            blockWrite.dstArrayElement = 0;
            blockWrite.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
            blockWrite.descriptorCount = 1;
            blockWrite.pImageInfo = &inputBlockInfo;
        VkWriteDescriptorSet blockPaletteWrite = {VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET};
            blockPaletteWrite.dstSet = raytraceDescriptorSets[i];
            blockPaletteWrite.dstBinding = 3;
            blockPaletteWrite.dstArrayElement = 0;
            blockPaletteWrite.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
            blockPaletteWrite.descriptorCount = 1;
            blockPaletteWrite.pImageInfo = &inputBlockPaletteInfo;
        VkWriteDescriptorSet voxelPaletteWrite = {VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET};
            voxelPaletteWrite.dstSet = raytraceDescriptorSets[i];
            voxelPaletteWrite.dstBinding = 4;
            voxelPaletteWrite.dstArrayElement = 0;
            voxelPaletteWrite.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
            voxelPaletteWrite.descriptorCount = 1;
            voxelPaletteWrite.pImageInfo = &inputVoxelPaletteInfo;
        VkWriteDescriptorSet outWrite = {VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET};
            outWrite.dstSet = raytraceDescriptorSets[i];
            outWrite.dstBinding = 5 ;
            outWrite.dstArrayElement = 0;
            outWrite.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
            outWrite.descriptorCount = 1;
            outWrite.pImageInfo = &outputFrameInfo;
        vector<VkWriteDescriptorSet> descriptorWrites = {PosMatWrite, NormWrite, blockWrite, blockPaletteWrite, voxelPaletteWrite, outWrite};

        vkUpdateDescriptorSets(device, descriptorWrites.size(), descriptorWrites.data(), 0, NULL);
    }
}

void Renderer::create_samplers(){
    VkSamplerCreateInfo samplerInfo = {};
        samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerInfo.magFilter = VK_FILTER_LINEAR;
        samplerInfo.minFilter = VK_FILTER_LINEAR;
        samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.anisotropyEnable = VK_FALSE;
        // samplerInfo.maxAnisotropy = 0;
        samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        samplerInfo.unnormalizedCoordinates = VK_FALSE;
        samplerInfo.compareEnable = VK_FALSE;
        samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
        samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        samplerInfo.mipLodBias = 0.0f;
        samplerInfo.minLod = 0.0f;
        samplerInfo.maxLod = 0.0f;
    
    raytracedImageSamplers.resize(MAX_FRAMES_IN_FLIGHT);
    for (auto& sampler : raytracedImageSamplers) {
        VK_CHECK(vkCreateSampler(device, &samplerInfo, NULL, &sampler));
    }
}

void Renderer::setup_Graphical_Descriptors(){
    for (u32 i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        VkDescriptorImageInfo storageImageInfo = {};
            storageImageInfo.imageView = raytracedImageViews[i];
            storageImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            storageImageInfo.sampler = raytracedImageSamplers[i];
        VkWriteDescriptorSet descriptorWrite = {};
            descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrite.dstSet = graphicalDescriptorSets[i];
            descriptorWrite.dstBinding = 0;
            descriptorWrite.dstArrayElement = 0;
            descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            descriptorWrite.descriptorCount = 1;
            descriptorWrite.pImageInfo = &storageImageInfo;
        vkUpdateDescriptorSets(device, 1, &descriptorWrite, 0, NULL);
    }
}

void Renderer::create_Blockify_Pipeline(){
    auto compShaderCode = read_Shader("shaders/compiled/blockify.spv");

    blockifyShaderModule = create_Shader_Module(compShaderCode);
    
    //single stage
    VkPipelineShaderStageCreateInfo compShaderStageInfo = {};
        compShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        compShaderStageInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
        compShaderStageInfo.module = blockifyShaderModule;
        compShaderStageInfo.pName = "main";

    VkPushConstantRange pushRange = {};
        // pushRange.size = 256;
        pushRange.size = sizeof(mat4) + sizeof(int); //trans
        pushRange.offset = 0;
        pushRange.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

    VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO; 
        pipelineLayoutInfo.setLayoutCount = 1; // 1 input (image from swapchain)  for now
        pipelineLayoutInfo.pSetLayouts = &blockifyDescriptorSetLayout;
        pipelineLayoutInfo.pushConstantRangeCount = 1;
        pipelineLayoutInfo.pPushConstantRanges = &pushRange;
        // pipelineLayoutInfo.pushConstantRangeCount = 0;
        // pipelineLayoutInfo.pPushConstantRanges = NULL;
    VK_CHECK(vkCreatePipelineLayout(device, &pipelineLayoutInfo, NULL, &blockifyLayout));

    VkComputePipelineCreateInfo pipelineInfo = {};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
        pipelineInfo.stage = compShaderStageInfo; //always single stage so no pointer :)
        pipelineInfo.layout = blockifyLayout;

    VK_CHECK(vkCreateComputePipelines(device, NULL, 1, &pipelineInfo, NULL, &blockifyPipeline));
//I<3Vk
}
void Renderer::create_Copy_Pipeline(){
    auto compShaderCode = read_Shader("shaders/compiled/copy.spv");

    copyShaderModule = create_Shader_Module(compShaderCode);
    
    //single stage
    VkPipelineShaderStageCreateInfo compShaderStageInfo = {};
        compShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        compShaderStageInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
        compShaderStageInfo.module = copyShaderModule;
        compShaderStageInfo.pName = "main";

    VkPushConstantRange pushRange = {};
        // pushRange.size = 256;
        pushRange.size = 0;
        pushRange.offset = 0;
        pushRange.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

    VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO; 
        pipelineLayoutInfo.setLayoutCount = 1; // 1 input (image from swapchain)  for now
        pipelineLayoutInfo.pSetLayouts = &copyDescriptorSetLayout;
        // pipelineLayoutInfo.pushConstantRangeCount = 1;
        // pipelineLayoutInfo.pPushConstantRanges = &pushRange;
        pipelineLayoutInfo.pushConstantRangeCount = 0;
        pipelineLayoutInfo.pPushConstantRanges = NULL;
    VK_CHECK(vkCreatePipelineLayout(device, &pipelineLayoutInfo, NULL, &copyLayout));

    VkComputePipelineCreateInfo pipelineInfo = {};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
        pipelineInfo.stage = compShaderStageInfo; //always single stage so no pointer :)
        pipelineInfo.layout = copyLayout;

    VK_CHECK(vkCreateComputePipelines(device, NULL, 1, &pipelineInfo, NULL, &copyPipeline));
//I<3Vk
}
void Renderer::create_Map_Pipeline(){
    auto compShaderCode = read_Shader("shaders/compiled/map.spv");

    mapShaderModule = create_Shader_Module(compShaderCode);
    
    //single stage
    VkPipelineShaderStageCreateInfo compShaderStageInfo = {};
        compShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        compShaderStageInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
        compShaderStageInfo.module = mapShaderModule;
        compShaderStageInfo.pName = "main";

    VkPushConstantRange pushRange = {};
        // pushRange.size = 256;
        pushRange.size = sizeof(mat4);
        pushRange.offset = 0;
        pushRange.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

    VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO; 
        pipelineLayoutInfo.setLayoutCount = 1; // 1 input (image from swapchain)  for now
        pipelineLayoutInfo.pSetLayouts = &mapDescriptorSetLayout;
        pipelineLayoutInfo.pushConstantRangeCount = 1;
        pipelineLayoutInfo.pPushConstantRanges = &pushRange;
        // pipelineLayoutInfo.pushConstantRangeCount = 0;
        // pipelineLayoutInfo.pPushConstantRanges = NULL;
    VK_CHECK(vkCreatePipelineLayout(device, &pipelineLayoutInfo, NULL, &mapLayout));

    VkComputePipelineCreateInfo pipelineInfo = {};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
        pipelineInfo.stage = compShaderStageInfo; //always single stage so no pointer :)
        pipelineInfo.layout = mapLayout;

    VK_CHECK(vkCreateComputePipelines(device, NULL, 1, &pipelineInfo, NULL, &mapPipeline));
//I<3Vk
}
void Renderer::create_Raytrace_Pipeline(){
    auto compShaderCode = read_Shader("shaders/compiled/comp.spv");

    raytraceShaderModule = create_Shader_Module(compShaderCode);
    
    //single stage
    VkPipelineShaderStageCreateInfo compShaderStageInfo = {};
        compShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        compShaderStageInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
        compShaderStageInfo.module = raytraceShaderModule;
        compShaderStageInfo.pName = "main";

    VkPushConstantRange pushRange = {};
        // pushRange.size = 256;
        pushRange.size = sizeof(i32)*1;
        pushRange.offset = 0;
        pushRange.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

    VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO; 
        pipelineLayoutInfo.setLayoutCount = 1; // 1 input (image from swapchain)  for now
        pipelineLayoutInfo.pSetLayouts = &raytraceDescriptorSetLayout;
        pipelineLayoutInfo.pushConstantRangeCount = 1;
        pipelineLayoutInfo.pPushConstantRanges = &pushRange;
        // pipelineLayoutInfo.pushConstantRangeCount = 0;
        // pipelineLayoutInfo.pPushConstantRanges = NULL;
    VK_CHECK(vkCreatePipelineLayout(device, &pipelineLayoutInfo, NULL, &raytraceLayout));

    VkComputePipelineCreateInfo pipelineInfo = {};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
        pipelineInfo.stage = compShaderStageInfo; //always single stage so no pointer :)
        pipelineInfo.layout = raytraceLayout;

    VK_CHECK(vkCreateComputePipelines(device, NULL, 1, &pipelineInfo, NULL, &raytracePipeline));
//I<3Vk
}

u32 Renderer::find_Memory_Type(u32 typeFilter, VkMemoryPropertyFlags properties){
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);    

    for (u32 i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }
    cout << "memory type not found";
    abort();
}

void Renderer::create_Buffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory) {
    VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = size;
        bufferInfo.usage = usage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VK_CHECK(vkCreateBuffer(device, &bufferInfo, NULL, &buffer));

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(device, buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = find_Memory_Type(memRequirements.memoryTypeBits, properties);

    VK_CHECK(vkAllocateMemory(device, &allocInfo, NULL, &bufferMemory));

    VK_CHECK(vkBindBufferMemory(device, buffer, bufferMemory, 0));
}

void Renderer::copy_Buffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) {
    VkCommandBuffer commandBuffer= begin_Single_Time_Commands();

    VkBufferCopy copyRegion{};
    copyRegion.size = size;
    vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

    end_Single_Time_Commands(commandBuffer);
}
void Renderer::copy_Buffer(VkBuffer srcBuffer, VkImage dstImage, VkDeviceSize size, uvec3 hwd, VkImageLayout layout) {
    VkCommandBuffer commandBuffer= begin_Single_Time_Commands();

    VkBufferImageCopy copyRegion = {};
    // copyRegion.size = size;
        copyRegion.imageExtent.height = hwd.x;
        copyRegion.imageExtent.width  = hwd.y;
        copyRegion.imageExtent.depth  = hwd.z;
        copyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        copyRegion.imageSubresource.layerCount = 1;
    vkCmdCopyBufferToImage(commandBuffer, srcBuffer, dstImage, layout, 1, &copyRegion);

    end_Single_Time_Commands(commandBuffer);
}

VkCommandBuffer Renderer::begin_Single_Time_Commands() {
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = commandPool;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    return commandBuffer;
}
void Renderer::end_Single_Time_Commands(VkCommandBuffer commandBuffer) {
    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(graphicsQueue);

    vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
}

void Renderer::transition_Image_Layout_Singletime(VkImage image, VkFormat format, VkImageAspectFlags aspect,VkImageLayout oldLayout, VkImageLayout newLayout,
        VkPipelineStageFlags sourceStage, VkPipelineStageFlags destinationStage, VkAccessFlags srcAccessMask, VkAccessFlags dstAccessMask) {

    VkCommandBuffer commandBuffer= begin_Single_Time_Commands();

    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = image;
    barrier.subresourceRange.aspectMask = aspect;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    barrier.srcAccessMask = srcAccessMask;
    barrier.dstAccessMask = dstAccessMask;

    vkCmdPipelineBarrier(
        commandBuffer,
        sourceStage, destinationStage,
        0,
        0, NULL,
        0, NULL,
        1, &barrier
    );

    end_Single_Time_Commands(commandBuffer);
}

void Renderer::copy_Whole_Image(VkExtent3D extent, VkCommandBuffer cmdbuf, VkImage src, VkImage dst){
    VkImageCopy copy_op = {};
        copy_op.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        copy_op.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        copy_op.srcSubresource.layerCount = 1;
        copy_op.dstSubresource.layerCount = 1;
        copy_op.srcSubresource.baseArrayLayer = 0;
        copy_op.dstSubresource.baseArrayLayer = 0;
        copy_op.srcSubresource.mipLevel = 0;
        copy_op.dstSubresource.mipLevel = 0;
        copy_op.srcOffset = {0, 0,0};
        copy_op.dstOffset = {0, 0,0};
        copy_op.extent = extent;
    vkCmdCopyImage(cmdbuf, 
        src,
        VK_IMAGE_LAYOUT_GENERAL, //TODO:
        dst,
        VK_IMAGE_LAYOUT_GENERAL, //TODO:
        1,
        &copy_op
    );
    VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout = VK_IMAGE_LAYOUT_GENERAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_GENERAL;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = dst;
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = 1;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT ;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT ;

    vkCmdPipelineBarrier(
        cmdbuf,
        VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
        0,
        0, NULL,
        0, NULL,
        1, &barrier
    );
}

void Renderer::transition_Image_Layout_Cmdb(VkCommandBuffer commandBuffer, VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout,
        VkPipelineStageFlags sourceStage, VkPipelineStageFlags destinationStage, VkAccessFlags srcAccessMask, VkAccessFlags dstAccessMask) {

    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = image;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    barrier.srcAccessMask = srcAccessMask;
    barrier.dstAccessMask = dstAccessMask;

    vkCmdPipelineBarrier(
        commandBuffer,
        sourceStage, destinationStage,
        0,
        0, NULL,
        0, NULL,
        1, &barrier
    );
}

//NOTE: all following funcs are not updates - they are just "loads"

//TODO: make use of frames in flight - do not wait for this things
//TODO: do smth with frames in flight
void Renderer::update_Block_Palette(Block* blockPalette){
    VkDeviceSize bufferSize = sizeof(Block)*BLOCK_PALETTE_SIZE;
    table3d<MatID_t> blockPaletteLinear = {};
    blockPaletteLinear.set_size(16, 16, 16*BLOCK_PALETTE_SIZE);
    for(i32 N=0; N<BLOCK_PALETTE_SIZE; N++){
    for(i32 x=0; x<BLOCK_SIZE; x++){
    for(i32 y=0; y<BLOCK_SIZE; y++){
    for(i32 z=0; z<BLOCK_SIZE; z++){
        blockPaletteLinear(x,y,z+16*N) = blockPalette[N].voxels[x][y][z].matID;
    }}}}
    // cout << "(i32)blockPaletteLinear(0,0,0)" " "<< (i32)((i32*)blockPaletteLinear.data())[0] << "\n";
    // printl((i32)blockPaletteLinear(0,0,0));
    VkBufferCreateInfo stagingBufferInfo = {VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
        stagingBufferInfo.size = bufferSize;
        stagingBufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    VmaAllocationCreateInfo stagingAllocInfo = {};
        stagingAllocInfo.usage = VMA_MEMORY_USAGE_AUTO;
        stagingAllocInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
    VkBuffer stagingBuffer = {};
    VmaAllocation stagingAllocation = {};
    void* data = NULL;
    printl((u64) VMAllocator);
    printl((u64) stagingAllocation);
    printl((u64) data);
    // fflush(stdout);
    vmaCreateBuffer(VMAllocator, &stagingBufferInfo, &stagingAllocInfo, &stagingBuffer, &stagingAllocation, NULL);

    vmaMapMemory(VMAllocator, stagingAllocation, &data);
        memcpy(data, blockPaletteLinear.data(), bufferSize);
        // cout << "\n PIZDEC " << *((i32*) data);
        // cout << "\n PIZDEC " << *((i32*) blockPaletteLinear.data());
        // cout << "\n PIZDEC " << (i32)*((char*)data);
        // cout << "\n PIZDEC " << (i32)*((char*)blockPaletteLinear.data());
    vmaUnmapMemory(VMAllocator, stagingAllocation);
    // vmaMapMemory(VMAllocator, stagingAllocation, &data);
    // cout << "\n PIZDEC" << *((i32*)data);
    // cout << "\n PIZDEC" << *((char*)data);
    // vmaUnmapMemory(VMAllocator, stagingAllocation);

    for(i32 i=0; i<MAX_FRAMES_IN_FLIGHT; i++){
        copy_Buffer(stagingBuffer, originBlockPaletteImages[i], bufferSize, uvec3(16,16,BLOCK_PALETTE_SIZE*16), VK_IMAGE_LAYOUT_GENERAL);
    }

    vmaDestroyBuffer(VMAllocator, stagingBuffer, stagingAllocation);
}

//TODO: do smth with frames in flight
void Renderer::update_Voxel_Palette(Material* materialPalette){
    VkDeviceSize bufferSize = sizeof(Material)*256;

    VkBufferCreateInfo stagingBufferInfo = {VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
        stagingBufferInfo.size = bufferSize;
        stagingBufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    VmaAllocationCreateInfo stagingAllocInfo = {};
        stagingAllocInfo.usage = VMA_MEMORY_USAGE_AUTO;
        stagingAllocInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
    VkBuffer stagingBuffer = {};
    VmaAllocation stagingAllocation = {};
    vmaCreateBuffer(VMAllocator, &stagingBufferInfo, &stagingAllocInfo, &stagingBuffer, &stagingAllocation, NULL);

    void* data;
    vmaMapMemory(VMAllocator, stagingAllocation, &data);
        memcpy(data, materialPalette, bufferSize);
    vmaUnmapMemory(VMAllocator, stagingAllocation);

    for(i32 i=0; i<MAX_FRAMES_IN_FLIGHT; i++){
        copy_Buffer(stagingBuffer, raytraceVoxelPaletteImages[i], bufferSize, uvec3(256,6,1), VK_IMAGE_LAYOUT_GENERAL);
    }

    vmaDestroyBuffer(VMAllocator, stagingBuffer, stagingAllocation);
}

//TODO: do smth with frames in flight
void Renderer::update_Blocks(BlockID_t* blocks){
    VkDeviceSize bufferSize = sizeof(BlockID_t)*8*8*8;

    VkBufferCreateInfo stagingBufferInfo = {VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
        stagingBufferInfo.size = bufferSize;
        stagingBufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    VmaAllocationCreateInfo stagingAllocInfo = {};
        stagingAllocInfo.usage = VMA_MEMORY_USAGE_AUTO;
        stagingAllocInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
    VkBuffer stagingBuffer = {};
    VmaAllocation stagingAllocation = {};
    vmaCreateBuffer(VMAllocator, &stagingBufferInfo, &stagingAllocInfo, &stagingBuffer, &stagingAllocation, NULL);

    void* data;
    vmaMapMemory(VMAllocator, stagingAllocation, &data);
        memcpy(data, blocks, bufferSize);
    vmaUnmapMemory(VMAllocator, stagingAllocation);

    for(i32 i=0; i<MAX_FRAMES_IN_FLIGHT; i++){
        VkBufferCreateInfo bufferInfo = {VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
            bufferInfo.size = bufferSize;
            bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        VmaAllocationCreateInfo allocInfo = {};

        copy_Buffer(stagingBuffer, originBlocksImages[i], bufferSize, uvec3(8,8,8), VK_IMAGE_LAYOUT_GENERAL);
    }

    vmaDestroyBuffer(VMAllocator, stagingBuffer, stagingAllocation);
}