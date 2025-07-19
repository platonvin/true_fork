module;

import ring;
import glfw;
import glm;
import std;

#include <cassert>

using namespace std;
using namespace glm;
using namespace glfw;

#define VK_NO_PROTOTYPES
#include <vma/vk_mem_alloc.h>
#include <volk/volk.h>

export module lumal:types;

export namespace Lumal {
typedef struct Buffer {
    VkBuffer buffer;
    VmaAllocation alloc;
    void* mapped = NULL;
} Buffer;

typedef struct Image {
    VkImage image;
    VkImageView view; // main view
    vector<VkImageView> mip_views;
    VmaAllocation alloc;
    VkFormat format;
    // VkImageLayout layout; everything is in GENERAL.
    VkImageAspectFlags aspect;
    VkExtent3D extent;
    int mip_levels = 1;
} Image;

typedef struct ImageDeletion {
    Image image;
    int life_counter;
} ImageDeletion;

typedef struct BufferDeletion {
    Buffer buffer;
    int life_counter;
} BufferDeletion;

// used for configuring raster pipeline
typedef struct AttrFormOffs {
    VkFormat format;
    uint32_t offset;
} AttrFormOffs;

enum BlendAttachment {
    NO_BLEND,
    BLEND_MIX,
    BLEND_SUB,
    BLEND_REPLACE_IF_GREATER, // basically max
    BLEND_REPLACE_IF_LESS, // basically max
};

enum DepthTesting {
    DEPTH_TEST_NONE_BIT = 0,
    DEPTH_TEST_READ_BIT = 1 << 0,
    DEPTH_TEST_WRITE_BIT = 1 << 1,
};

inline DepthTesting operator|(DepthTesting a, DepthTesting b) {
    return DepthTesting(int(a) | int(b));
}

// it was not discard in fragment. FML
enum Discard {
    NO_DISCARD,
    DO_DISCARD,
};

enum LoadStoreOp {
    DontCare,
    Clear,
    Store,
    Load,
};

const VkStencilOpState NO_STENCIL = {};

typedef struct RasterPipe {
    VkPipeline line;
    VkPipelineLayout lineLayout;

    ring<VkDescriptorSet> sets;
    VkDescriptorSetLayout setLayout;

    VkRenderPass renderPass; // we dont really need to store it in here but why not
    i32 subpassId;
} RasterPipe;

typedef struct ComputePipe {
    VkPipeline line = 0;
    VkPipelineLayout lineLayout = 0;

    ring<VkDescriptorSet> sets = {};
    VkDescriptorSetLayout setLayout = {};
} ComputePipe;

typedef struct AttachmentDescription {
    ring<Image>* images;
    LoadStoreOp load, store, sload, sstore;
    VkClearValue clear = {};
    VkImageLayout finalLayout = VK_IMAGE_LAYOUT_GENERAL;
} AttachmentDescription;

typedef struct SubpassAttachments {
    vector<RasterPipe*> pipes = {};
    // vector<Image*> a_input;
    vector<ring<Image>*> a_input = {};
    // vector<Image*> a_color;
    vector<ring<Image>*> a_color = {};
    // Image* a_depth;
    ring<Image>* a_depth = {};
} SubpassAttachments;

typedef struct SubpassAttachmentRefs {
    vector<VkAttachmentReference> a_input = {};
    vector<VkAttachmentReference> a_color = {};
    VkAttachmentReference a_depth = {};
} SubpassAttachmentRefs;

typedef struct RenderPass {
    vector<VkClearValue> clear_colors = {};
    ring<VkFramebuffer> framebuffers = {};

    VkExtent2D extent = {};
    VkRenderPass rpass = 0;
} RenderPass;

enum RelativeDescriptorPos {
    NONE, // what?
    PREVIOUS, // Relatove Descriptor position previous - for accumulators
    CURRENT, // Relatove Descriptor position matching - common cpu-paired
    FIRST, // Relatove Descriptor position first    - for gpu-only
};

typedef Lumal::RelativeDescriptorPos RDP;
constexpr auto NO_SAMPLER = ((VkSampler)(0));
constexpr auto NO_LAYOUT = ((VkImageLayout)(0));

typedef struct ShaderStage {
    const char* src = 0;
    VkShaderStageFlagBits stage = (VkShaderStageFlagBits)0;
} ShaderStage;

typedef struct DescriptorInfo {
    VkDescriptorType type = VK_DESCRIPTOR_TYPE_SAMPLER;
    RelativeDescriptorPos relativePos = RelativeDescriptorPos::NONE;
    ring<Buffer>* buffers = nullptr;
    ring<Image>* images = nullptr;
    VkSampler imageSampler = 0;
    VkImageLayout imageLayout = VK_IMAGE_LAYOUT_UNDEFINED; // ones that will be in use, not current
    VkShaderStageFlags stages = 0;
} DescriptorInfo;

typedef struct ShortDescriptorInfo {
    VkDescriptorType type = VK_DESCRIPTOR_TYPE_SAMPLER;
    VkShaderStageFlags stages = 0;
} ShortDescriptorInfo;

typedef struct DelayedDescriptorSetup {
    VkDescriptorSetLayout* setLayout = 0;
    ring<VkDescriptorSet>* sets = {};
    vector<DescriptorInfo> descriptions = {};
    VkShaderStageFlags stages = 0;
    VkDescriptorSetLayoutCreateFlags createFlags = 0;
} DelayedDescriptorSetup;

#define MAKE_DESCRIPTOR_TYPE(name) i32 name##_COUNTER = 0;

typedef struct DescriptorCounter {
#include "descriptor_types.hpp"
} DescriptorCounter;

#undef MAKE_DESCRIPTOR_TYPE

typedef struct Window {
    GLFWwindow* pointer;
    int width;
    int height;
} Window;

typedef struct QueueFamilyIndices {
    std::optional<u32> graphicalAndCompute;
    std::optional<u32> present;

  public:
    bool isComplete() {
        return graphicalAndCompute.has_value() && present.has_value();
    }
} QueueFamilyIndices;

typedef struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    vector<VkSurfaceFormatKHR> formats;
    vector<VkPresentModeKHR> presentModes;

  public:
    bool is_Suitable() {
        return (not formats.empty()) and (not presentModes.empty());
    }
} SwapChainSupportDetails;

typedef struct Settings {
    int timestampCount = 1024;
    int fif = 2;
    bool vsync = false;
    bool fullscreen = false;
    bool debug = false;
    bool profile = false;
    VkPhysicalDeviceFeatures deviceFeatures = {};
    VkPhysicalDeviceVulkan11Features deviceFeatures11 = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES};
    VkPhysicalDeviceVulkan12Features deviceFeatures12 = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES};
    VkPhysicalDeviceFeatures2 physical_features2 = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2};

    vector<const char*> instanceLayers = {};
    vector<const char*> instanceExtensions = {
        VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME,
        VK_KHR_SURFACE_EXTENSION_NAME,
        // "VK_KHR_win32_surface",
    };
    vector<const char*> deviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
        VK_EXT_HOST_QUERY_RESET_EXTENSION_NAME, // for measuring frame times - dynamic quality to
        // perfomance exchange
        // VK_NV_DEVICE_DIAGNOSTIC_CHECKPOINTS_EXTENSION_NAME
    };

    Settings() {
        deviceFeatures12.pNext = &deviceFeatures11;
        physical_features2.pNext = &deviceFeatures12;
    }

    void addExtraFeaturesPnext(auto* features_ptr) {
        void* pnext = physical_features2.pNext;
        physical_features2.pNext = features_ptr;
        features_ptr->pNext = pnext;
    }

} Settings;

// hash for VkSamplerCreateInfo
struct VkSamplerCreateInfoHash {
    size_t operator()(const VkSamplerCreateInfo& k) const {
        size_t res = 17;
        res = res * 31 + std::hash<VkStructureType>()(k.sType);
        res = res * 31 + std::hash<VkFilter>()(k.magFilter);
        res = res * 31 + std::hash<VkFilter>()(k.minFilter);
        res = res * 31 + std::hash<VkSamplerAddressMode>()(k.addressModeU);
        res = res * 31 + std::hash<VkSamplerAddressMode>()(k.addressModeV);
        res = res * 31 + std::hash<VkSamplerAddressMode>()(k.addressModeW);
        res = res * 31 + std::hash<VkBool32>()(k.anisotropyEnable);
        res = res * 31 + std::hash<VkBorderColor>()(k.borderColor);
        res = res * 31 + std::hash<VkBool32>()(k.unnormalizedCoordinates);
        res = res * 31 + std::hash<VkBool32>()(k.compareEnable);
        res = res * 31 + std::hash<VkCompareOp>()(k.compareOp);
        res = res * 31 + std::hash<VkSamplerMipmapMode>()(k.mipmapMode);
        res = res * 31 + std::hash<float>()(k.mipLodBias);
        res = res * 31 + std::hash<float>()(k.minLod);
        res = res * 31 + std::hash<float>()(k.maxLod);
        return res;
    }
};

struct VkSamplerCreateInfoEqual {
    bool operator()(const VkSamplerCreateInfo& lhs, const VkSamplerCreateInfo& rhs) const {
        return lhs.sType == rhs.sType && lhs.magFilter == rhs.magFilter && lhs.minFilter == rhs.minFilter
            && lhs.addressModeU == rhs.addressModeU && lhs.addressModeV == rhs.addressModeV && lhs.addressModeW == rhs.addressModeW
            && lhs.anisotropyEnable == rhs.anisotropyEnable && lhs.borderColor == rhs.borderColor
            && lhs.unnormalizedCoordinates == rhs.unnormalizedCoordinates && lhs.compareEnable == rhs.compareEnable
            && lhs.compareOp == rhs.compareOp && lhs.mipmapMode == rhs.mipmapMode && lhs.mipLodBias == rhs.mipLodBias
            && lhs.minLod == rhs.minLod && lhs.maxLod == rhs.maxLod;
    }
};

// conversion functions
inline VkExtent2D extent2d(ivec2 ext) {
    return {u32(ext.x), u32(ext.y)};
}

inline VkExtent2D extent2d(uvec2 ext) {
    return {u32(ext.x), u32(ext.y)};
}

inline VkExtent2D extent2d(ivec3 ext) {
    return {u32(ext.x), u32(ext.y)};
}

inline VkExtent2D extent2d(uvec3 ext) {
    return {u32(ext.x), u32(ext.y)};
}

inline VkExtent3D extent3d(ivec3 ext) {
    return {u32(ext.x), u32(ext.y), u32(ext.z)};
}

inline VkExtent3D extent3d(uvec3 ext) {
    return {u32(ext.x), u32(ext.y), u32(ext.z)};
}

struct CommandBuffer {
    VkCommandBuffer commandBuffer;

    // has to be stored in here to allow caching
    VkPipeline CACHED_BOUND_PIPELINE = VK_NULL_HANDLE;
    VkBuffer CACHED_BOUND_VERTEX_BUFFER = VK_NULL_HANDLE;
    VkBuffer CACHED_BOUND_INDEX_BUFFER = VK_NULL_HANDLE;

    void cmdBindVertexBuffers(uint32_t firstBinding, uint32_t bindingCount, const VkBuffer* pBuffers, const VkDeviceSize* pOffsets);
    void cmdBindIndexBuffer(VkBuffer buffer, VkDeviceSize offset, VkIndexType indexType);
    void cmdBindDescriptorSets(
        VkPipelineBindPoint pipelineBindPoint,
        VkPipelineLayout layout,
        uint32_t firstSet,
        uint32_t descriptorSetCount,
        const VkDescriptorSet* pDescriptorSets,
        uint32_t dynamicOffsetCount,
        const uint32_t* pDynamicOffsets
    );
    void cmdBindPipe(RasterPipe* pipe);
    void cmdBindPipe(ComputePipe* pipe);
    void cmdBindPipeline(VkPipelineBindPoint pipelineBindPoint, VkPipeline pipeline);

    void cmdDraw(u32 vertexCount, u32 instanceCount, u32 firstVertex, u32 firstInstance);
    void cmdDrawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance);
    void cmdDispatch(u32 groupCountX, u32 groupCountY, u32 groupCountZ);
    void cmdBeginRenderPass(RenderPass* rpass);
    void cmdNextSubpass(RenderPass* rpass);
    void cmdEndRenderPass(RenderPass* rpass);
    void cmdPipelineBarrier(
        VkPipelineStageFlags srcStageMask,
        VkPipelineStageFlags dstStageMask,
        VkAccessFlags srcAccessMask,
        VkAccessFlags dstAccessMask,
        Buffer buffer
    );
    void cmdPipelineBarrier(
        VkPipelineStageFlags srcStageMask,
        VkPipelineStageFlags dstStageMask,
        VkAccessFlags srcAccessMask,
        VkAccessFlags dstAccessMask,
        Image image
    );
    void cmdPipelineBarrier(VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask);
    void cmdExplicitTransLayoutBarrier(
        VkImageLayout srcLayout,
        VkImageLayout targetLayout,
        VkPipelineStageFlags srcStageMask,
        VkPipelineStageFlags dstStageMask,
        VkAccessFlags srcAccessMask,
        VkAccessFlags dstAccessMask,
        Image* image
    );
    void cmdSetViewport(int width, int height);
    void cmdSetViewport(VkExtent2D extent);
};

struct Renderer {
  public:
    // syntactic sugar is bitter
    Renderer() : descriptorBuilder(*this), pipeBuilder(*this), renderPassBuilder(*this) {};

    void init(Settings settings);
    void cleanup();

    double deltaTime = 0;
    bool resized = false;
    Settings settings = {};

    void start_frame(vector<CommandBuffer> commandBuffers);
    void present();
    void end_frame(vector<CommandBuffer> commandBuffers);

    template<class Elem_T>
    ring<Buffer> createElemBuffers(Elem_T* vertices, u32 count, u32 buffer_usage = 0);
    template<class Elem_T>
    ring<Buffer> createElemBuffers(vector<Elem_T> vertices, u32 buffer_usage = 0);
    template<class Elem_T>
    Buffer createElemBuffer(Elem_T* elements, u32 count, u32 buffer_usage);

    void createAllocator();
    void createWindow();
    void createInstance();
    void setupDebugMessenger();
    void debugValidate(); // simple runtime validation if settings.debug
    void debugSetName();
    void createSurface();
    SwapChainSupportDetails querySwapchainSupport(VkPhysicalDevice);
    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice);
    bool checkFormatSupport(VkPhysicalDevice device, VkFormat format, VkFormatFeatureFlags features);
    bool isPhysicalDeviceSuitable(VkPhysicalDevice);
    bool checkPhysicalDeviceExtensionSupport(VkPhysicalDevice);
    void pickPhysicalDevice();
    void createLogicalDevice();
    void createSwapchain();
    VkSurfaceFormatKHR chooseSwapSurfaceFormat(vector<VkSurfaceFormatKHR> availableFormats);
    VkPresentModeKHR chooseSwapPresentMode(vector<VkPresentModeKHR> availablePresentModes);
    VkExtent2D chooseSwapExtent(VkSurfaceCapabilitiesKHR capabilities);
    void createSwapchainImageViews();
    VkAttachmentLoadOp getOpLoad(LoadStoreOp op);
    VkAttachmentStoreOp getOpStore(LoadStoreOp op);

    // used on windows resize for example
    // should be provided
    //  using sd_fun = VkResult (*)(void);
    //  sd_fun cleanupSwapchainDependent = NULL;
    //  sd_fun createSwapchainDependent = NULL;
    std::function<VkResult()> cleanupSwapchainDependent = NULL;
    std::function<VkResult()> createSwapchainDependent = NULL;
    void recreateSwapchain();

    void deviceWaitIdle();
    void createRenderPass(vector<AttachmentDescription> attachments, vector<SubpassAttachments> subpasses, RenderPass* rpass);
    void destroyRenderPass(RenderPass* rpass);

    VkFormat findSupportedFormat(vector<VkFormat> candidates, VkImageType type, VkImageTiling tiling, VkImageUsageFlags usage);

    void createRasterPipeline(
        RasterPipe* pipe,
        VkDescriptorSetLayout extra_dynamic_layout,
        vector<ShaderStage> shader_stages,
        vector<AttrFormOffs> attr_desc,
        u32 stride,
        VkVertexInputRate input_rate,
        VkPrimitiveTopology topology,
        VkExtent2D extent,
        vector<BlendAttachment> blends,
        u32 push_size,
        DepthTesting depthTest,
        VkCompareOp depthCompareOp,
        VkCullModeFlags culling,
        Discard discard,
        VkStencilOpState stencil
    );
    void destroyRasterPipeline(RasterPipe* pipe);

    void createComputePipeline(
        ComputePipe* pipe,
        VkDescriptorSetLayout extra_dynamic_layout,
        const char* src,
        u32 push_size,
        VkPipelineCreateFlags create_flags
    );
    void destroyComputePipeline(ComputePipe* pipe);

    void createDescriptorPool();
    void resetDescriptorSetup();
    void deferDescriptorSetup(
        VkDescriptorSetLayout* dsetLayout,
        ring<VkDescriptorSet>* descriptors,
        vector<DescriptorInfo> description,
        VkShaderStageFlags stages,
        VkDescriptorSetLayoutCreateFlags createFlags = 0
    );
    void setupDescriptor(
        VkDescriptorSetLayout* dsetLayout,
        ring<VkDescriptorSet>* descriptors,
        vector<DescriptorInfo> description,
        VkShaderStageFlags stages
    );
    void flushDescriptorSetup();
    vector<DelayedDescriptorSetup> delayed_descriptor_setups;
    void createSampler(VkSampler* sampler, VkFilter mag, VkFilter min, VkSamplerAddressMode U, VkSamplerAddressMode V, VkSamplerAddressMode W);
    void destroySampler(VkSampler sampler);

    class DescriptorSetupBuilder {
      private:
        VkDescriptorSetLayout* dsetLayout = {};
        ring<VkDescriptorSet>* descriptors = {};
        vector<DescriptorInfo> descriptions = {};
        VkDescriptorSetLayoutCreateFlags createFlags = 0;
        Renderer& renderer;

      public:
        DescriptorSetupBuilder(Renderer& renderer) : renderer(renderer) {}

        DescriptorSetupBuilder& setLayout(VkDescriptorSetLayout* layout) {
            dsetLayout = layout;
            return *this;
        }

        DescriptorSetupBuilder& setDescriptorSets(ring<VkDescriptorSet>* desc) {
            descriptors = desc;
            return *this;
        }

        DescriptorSetupBuilder& setCreateFlags(VkDescriptorSetLayoutCreateFlags flags) {
            createFlags = flags;
            return *this;
        }

        DescriptorSetupBuilder& setDescriptions(vector<DescriptorInfo> const& descInfos) {
            descriptions = descInfos;
            return *this;
        }

        // descriptions are clear()ed, other state reamains
        void defer() {
            renderer.deferDescriptorSetup(dsetLayout, descriptors, descriptions, createFlags);
            descriptions.clear();
        }
    };

    class PipeBuilder {
      private:
        Renderer& renderer;
        VkDescriptorSetLayout extra_dynamic_layout = 0;
        vector<ShaderStage> shader_stages;
        vector<AttrFormOffs> attr_desc;
        u32 stride = 0;
        VkVertexInputRate input_rate = VK_VERTEX_INPUT_RATE_VERTEX;
        VkPrimitiveTopology topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        VkExtent2D extent = {0, 0};
        vector<BlendAttachment> blends;
        u32 push_size = 0;
        DepthTesting depthTest = DEPTH_TEST_NONE_BIT;
        VkCompareOp depthCompareOp = VK_COMPARE_OP_NEVER;
        VkCullModeFlags culling = VK_CULL_MODE_NONE;
        Discard discard = NO_DISCARD;
        VkStencilOpState stencil = NO_STENCIL;
        VkPipelineCreateFlags create_flags = 0;

      public:
        PipeBuilder(Renderer& renderer) : renderer(renderer) {}

        PipeBuilder& setExtraDynamicLayout(VkDescriptorSetLayout layout) {
            extra_dynamic_layout = layout;
            return *this;
        }

        PipeBuilder& setStride(u32 s) {
            stride = s;
            return *this;
        }

        PipeBuilder& setInputRate(VkVertexInputRate rate) {
            input_rate = rate;
            return *this;
        }

        PipeBuilder& setTopology(VkPrimitiveTopology topo) {
            topology = topo;
            return *this;
        }

        PipeBuilder& setExtent(VkExtent2D e) {
            extent = e;
            return *this;
        }

        PipeBuilder& setPushConstantSize(u32 size) {
            push_size = size;
            return *this;
        }

        PipeBuilder& setDepthTesting(DepthTesting depth) {
            depthTest = depth;
            return *this;
        }

        PipeBuilder& setDepthCompareOp(VkCompareOp op) {
            depthCompareOp = op;
            return *this;
        }

        PipeBuilder& setCulling(VkCullModeFlags cull) {
            culling = cull;
            return *this;
        }

        PipeBuilder& setDiscard(Discard disc) {
            discard = disc;
            return *this;
        }

        PipeBuilder& setStencil(VkStencilOpState st) {
            stencil = st;
            return *this;
        }

        PipeBuilder& setCreateFlags(VkPipelineCreateFlags flags) {
            create_flags = flags;
            return *this;
        }

        PipeBuilder& setAttributes(vector<AttrFormOffs> const& attributes) {
            attr_desc = attributes;
            return *this;
        }

        PipeBuilder& setBlends(vector<BlendAttachment> const& b) {
            blends = b;
            return *this;
        }

        PipeBuilder& setStages(vector<ShaderStage> const& stages) {
            shader_stages = stages;
            return *this;
        }

        void buildRaster(RasterPipe* pipe) {
            renderer.createRasterPipeline(
                pipe,
                extra_dynamic_layout,
                shader_stages,
                attr_desc,
                stride,
                input_rate,
                topology,
                extent,
                blends,
                push_size,
                depthTest,
                depthCompareOp,
                culling,
                discard,
                stencil
            );
            extra_dynamic_layout = 0;
        }

        void buildCompute(ComputePipe* pipe) {
            renderer.createComputePipeline(pipe, extra_dynamic_layout, shader_stages[0].src, push_size, create_flags);
            extra_dynamic_layout = 0;
        }
    };

    class RenderPassBuilder {
      private:
        Renderer& renderer; // Reference to the renderer
        vector<AttachmentDescription> attachments_; // Attachments for the render pass
        vector<SubpassAttachments> subpasses_; // Subpasses for the render pass
        // vector<VkClearValue> clear_colors;

      public:
        RenderPassBuilder(Renderer& renderer) : renderer(renderer) {}

        RenderPassBuilder& setAttachments(vector<AttachmentDescription> const& attachments) {
            attachments_ = attachments;
            return *this;
        }

        RenderPassBuilder& setSubpasses(vector<SubpassAttachments> const& subpasses) {
            subpasses_ = subpasses;
            return *this;
        }

        // RenderPassBuilder& setClearColors(vector<VkClearValue> const& clears) {clear_colors =
        // clears;      return *this;}

        void build(RenderPass* rpass) const {
            renderer.createRenderPass(attachments_, subpasses_, rpass);
        }
    };

    DescriptorSetupBuilder descriptorBuilder;
    PipeBuilder pipeBuilder;
    RenderPassBuilder renderPassBuilder;

    void destroyImages(ring<Image>* images);
    void destroyImages(Image* images);
    void destroyBuffers(ring<Buffer>* buffers);
    void destroyBuffers(Buffer* buffers);
    void createImageStorages(
        ring<Image>* images,
        VkImageType type,
        VkFormat format,
        VkImageUsageFlags usage,
        VmaMemoryUsage vma_usage,
        VmaAllocationCreateFlags vma_flags,
        VkImageAspectFlags aspect,
        VkExtent3D size,
        int mipmaps = 1,
        VkSampleCountFlagBits sample_count = VK_SAMPLE_COUNT_1_BIT
    );
    void createImageStorages(
        Image* image,
        VkImageType type,
        VkFormat format,
        VkImageUsageFlags usage,
        VmaMemoryUsage vma_usage,
        VmaAllocationCreateFlags vma_flags,
        VkImageAspectFlags aspect,
        VkExtent3D size,
        int mipmaps = 1,
        VkSampleCountFlagBits sample_count = VK_SAMPLE_COUNT_1_BIT
    );
    void generateMipmaps(
        CommandBuffer commandBuffer,
        VkImage image,
        int32_t texWidth,
        int32_t texHeight,
        uint32_t mipLevels,
        VkImageAspectFlags aspect
    );
    void createBufferStorages(ring<Buffer>* buffers, VkBufferUsageFlags usage, u32 size, bool host = false);
    void createBufferStorages(Buffer* buffer, VkBufferUsageFlags usage, u32 size, bool host = false);
    void mapBufferStorages(ring<Buffer>* buffers);
    void mapBufferStorages(Buffer* buffer);
    VkShaderModule createShaderModule(vector<char>* code);
    void createFramebuffers(ring<VkFramebuffer>* framebuffers, vector<ring<Image>*> imgs4views, VkRenderPass renderPass, u32 Width, u32 Height);
    void createFramebuffers(ring<VkFramebuffer>* framebuffers, vector<ring<Image>*> imgs4views, VkRenderPass renderPass, VkExtent2D extent);
    void createCommandPool();
    void createCommandBuffers(ring<CommandBuffer>* commandBuffers, u32 size);
    void createSyncObjects();

    void createImageFromMemorySingleTime(Image* image, const void* source, u32 size);
    // void createBuffer (VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags
    // properties, VkBuffer* buffer, VkDeviceMemory* bufferMemory);
    void copyBufferSingleTime(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
    void copyBufferSingleTime(VkBuffer srcBuffer, Image* image, uvec3 size);
    CommandBuffer beginSingleTimeCommands();
    void endSingleTimeCommands(CommandBuffer commandBuffer);
    void transitionImageLayoutSingletime(Image* image, VkImageLayout newLayout, int mipmaps = 1);
    void copyImage(VkCommandBuffer cmdbuf, VkExtent3D extent, Image src, Image dst);
    void copyWholeImage(VkCommandBuffer cmdbuf, Image src, Image dst);
    void blitWholeImage(VkCommandBuffer cmdbuf, Image src, Image dst, VkFilter filter);
    void processDeletionQueues();
    void getInstanceLayers();
    void getInstanceExtensions();

    DescriptorCounter descriptorCounter = {};
    i32 descriptor_sets_count = 0;
    void countDescriptor(const VkDescriptorType type);
    void createDescriptorSetLayout(
        vector<ShortDescriptorInfo> shortDescriptorInfos,
        VkDescriptorSetLayout* layout,
        VkDescriptorSetLayoutCreateFlags flags = 0
    );

    Window window = {};
    VkInstance instance = {};
    VkPhysicalDevice physicalDevice = {};
    VkDevice device = {};
    VkSurfaceKHR surface = {};
    QueueFamilyIndices familyIndices = {};
    VkQueue graphicsQueue = {};
    VkQueue presentQueue = {};
    VkCommandPool commandPool = {};
    VkSwapchainKHR swapchain = {};

    VkPhysicalDeviceProperties physicalDeviceProperties = {};
    VkPhysicalDeviceFeatures physicalDeviceFeatures = {};

    VkDescriptorPool descriptorPool = {};
    VmaAllocator VMAllocator = {};

    VkFormat swapChainImageFormat = {};
    VkExtent2D swapChainExtent = {};
    ring<Image> swapchainImages = {};
    u32 imageIndex = 0;

    // this are still managed by user, but are required to exist
    ring<CommandBuffer>* extraCommandBuffers = {}; // runtime copies. Also does first frame
    // resources
    ring<CommandBuffer>* mainCommandBuffers = {};

    ring<VkSemaphore> imageAvailableSemaphores = {}; // to sync presenting with renering
    ring<VkSemaphore> renderFinishedSemaphores = {}; // to sync renering with presenting
    ring<VkFence> frameInFlightFences = {};

    // holds all the cpu-side VkSamplers
    std::unordered_map<VkSamplerCreateInfo, VkSampler, VkSamplerCreateInfoHash, VkSamplerCreateInfoEqual> samplerMap;

    vector<ImageDeletion> imageDeletionQueue = {}; // cpu side  image abstractions deletion queue. Exists for delayed copies
    vector<BufferDeletion> bufferDeletionQueue = {}; // cpu side buffer abstractions deletion queue. Exists for delayed copies

    u32 currentFrame = 0;
    int iFrame = 0;

    VkDebugUtilsMessengerEXT debugMessenger; // just keep it there, who cares about
    ring<VkQueryPool> queryPoolTimestamps;
    int currentTimestamp = 0;
    int timestampCount = 0;
    vector<uint64_t> timestamps = {0};
    vector<double> ftimestamps = {0};
    vector<double> average_ftimestamps = {0};
    vector<const char*> timestampNames = {0};

    void* pNext = 0; // to allow easy expansion

    // private:
    // VkPipeline CACHED_BOUND_PIPELINE = VK_NULL_HANDLE;
    // VkBuffer CACHED_BOUND_VERTEX_BUFFER = VK_NULL_HANDLE;
    // VkBuffer CACHED_BOUND_INDEX_BUFFER = VK_NULL_HANDLE;
};

template<class Elem_T>
ring<Buffer> Renderer::createElemBuffers(Elem_T* elements, u32 count, u32 buffer_usage) {
    VkDeviceSize bufferSize = sizeof(Elem_T) * count;
    ring<Buffer> elems(settings.fif);
    VkBufferCreateInfo stagingBufferInfo = {VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
    stagingBufferInfo.size = bufferSize;
    stagingBufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    VmaAllocationCreateInfo stagingAllocInfo = {};
    stagingAllocInfo.usage = VMA_MEMORY_USAGE_AUTO;
    stagingAllocInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
    stagingAllocInfo.requiredFlags = VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    VkBuffer stagingBuffer = {};
    VmaAllocation stagingAllocation = {};
    vmaCreateBuffer(VMAllocator, &stagingBufferInfo, &stagingAllocInfo, &stagingBuffer, &stagingAllocation, NULL);
    void* data;
    assert(VMAllocator);
    assert(stagingAllocation);
    assert(&data);
    vmaMapMemory(VMAllocator, stagingAllocation, &data);
    memcpy(data, elements, bufferSize);
    vmaUnmapMemory(VMAllocator, stagingAllocation);
    for (i32 i = 0; i < settings.fif; i++) {
        VkBufferCreateInfo bufferInfo = {VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
        bufferInfo.size = bufferSize;
        bufferInfo.usage = buffer_usage;
        VmaAllocationCreateInfo allocInfo = {};
        allocInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
        vmaCreateBuffer(VMAllocator, &bufferInfo, &allocInfo, &elems[i].buffer, &elems[i].alloc, NULL);
        copyBufferSingleTime(stagingBuffer, elems[i].buffer, bufferSize);
    }
    vmaDestroyBuffer(VMAllocator, stagingBuffer, stagingAllocation);
    return elems;
}

template<class Elem_T>
Buffer Renderer::createElemBuffer(Elem_T* elements, u32 count, u32 buffer_usage) {
    VkDeviceSize bufferSize = sizeof(Elem_T) * count;
    Buffer elems = {};
    VkBufferCreateInfo stagingBufferInfo = {VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
    stagingBufferInfo.size = bufferSize;
    stagingBufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    VmaAllocationCreateInfo stagingAllocInfo = {};
    stagingAllocInfo.usage = VMA_MEMORY_USAGE_AUTO;
    stagingAllocInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
    stagingAllocInfo.requiredFlags = VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    VkBuffer stagingBuffer = {};
    VmaAllocation stagingAllocation = {};
    vmaCreateBuffer(VMAllocator, &stagingBufferInfo, &stagingAllocInfo, &stagingBuffer, &stagingAllocation, NULL);
    void* data;
    assert(VMAllocator);
    assert(stagingAllocation);
    assert(&data);
    vmaMapMemory(VMAllocator, stagingAllocation, &data);
    memcpy(data, elements, bufferSize);
    vmaUnmapMemory(VMAllocator, stagingAllocation);
    // for (i32 i = 0; i < settings.fif; i++) {
    VkBufferCreateInfo bufferInfo = {VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
    bufferInfo.size = bufferSize;
    bufferInfo.usage = buffer_usage;
    VmaAllocationCreateInfo allocInfo = {};
    allocInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
    vmaCreateBuffer(VMAllocator, &bufferInfo, &allocInfo, &elems.buffer, &elems.alloc, NULL);
    copyBufferSingleTime(stagingBuffer, elems.buffer, bufferSize);
    // }
    vmaDestroyBuffer(VMAllocator, stagingBuffer, stagingAllocation);
    return elems;
}
} // namespace Lumal