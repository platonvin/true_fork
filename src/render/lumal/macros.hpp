#pragma once
#ifndef TRACE_H
    #define TRACE_H

    // include after render|al.hpp only
    #include "vk_enum_string_helper.h" //idk why but it is neither shipped with Linux Vulkan SDK nor bundled in vcpkg vulkan-sdk-components

    #define KEND "\x1B[0m"
    #define KRED "\x1B[31m"
    #define KGRN "\x1B[32m"
    #define KYEL "\x1B[33m"
    #define KBLU "\x1B[34m"
    #define KMAG "\x1B[35m"
    #define KCYN "\x1B[36m"
    #define KWHT "\x1B[37m"

    #define ATRACE(__s) \
        printf(KGRN "%s:%d: Fun: %s\n" KEND, __FILE__, __LINE__, __FUNCTION__); \
        __s
    #define LOG(x) std::cout << #x " " << x << std::endl;
    #if defined(_PRINTLINE) || defined(_LANG_SERVER)
        #define TRACE(__s) \
            printf(KGRN "%s:%d: Fun: %s\n" KEND, __FILE__, __LINE__, __FUNCTION__); \
            __s
        #define DEBUG_LOG(x) std::cout << #x " " << x << std::endl;
    #else
        #define TRACE(__s) __s; // TODO: CPU profiler
        #define DEBUG_LOG(x) \
            do { \
            } while (0);
    #endif

    #define STRINGIZE(x) STRINGIZE2(x)
    #define STRINGIZE2(x) #x
    #define __LINE_STRING__ STRINGIZE(__LINE__)

    #define crash(string) \
        do { \
            std::cout << KRED << __FILE__ << ":" << __LINE__ << ": Fun: " << __FUNCTION__ << " -- " << #string << KEND << std::endl; \
            std::exit(69); \
        } while (0)

    // #define NDEBUG
    #ifdef NDEBUG
        // #define VMA_NAME(allocation)
        #define VKNDEBUG
        #define VK_CHECK(func) \
            do { \
                VkResult result = (func); \
                if (result != VK_SUCCESS) { \
                    std::cout << "LINE :" << __LINE__ << "Vulkan call failed: " << string_VkResult(result) << "\n"; \
                    exit(result); \
                } \
            } while (0)
    #else
        #define VK_CHECK(func) \
            do { \
                VkResult result = (func); \
                if (result != VK_SUCCESS) { \
                    std::cout << "LINE :" << __LINE__ << "Vulkan call failed: " << string_VkResult(result) << "\n"; \
                    exit(result); \
                } \
            } while (0)
    // #define malloc(x)   {void* res_ptr =(malloc)(x  ); assert(res_ptr!=NULL && __LINE_STRING__);
    // res_ptr;} #define calloc(x,y) {void* res_ptr =(calloc)(x,y); assert(res_ptr!=NULL &&
    // __LINE_STRING__); res_ptr;}
    #endif

    #define PLACE_TIMESTAMP(commandBuffer) \
        do { \
            if (settings.profile) { \
                timestampNames[currentTimestamp] = __func__; \
                vkCmdWriteTimestamp(commandBuffer, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, queryPoolTimestamps.current(), currentTimestamp++); \
            } \
        } while (0)

    #define PLACE_TIMESTAMP_ALWAYS(commandBuffer) \
        do { \
            timestampNames[currentTimestamp] = __func__; \
            vkCmdWriteTimestamp(commandBuffer, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, queryPoolTimestamps.current(), currentTimestamp++); \
        } while (0)

    // #define SET_ALLOC_NAMES
    #ifdef SET_ALLOC_NAMES
        #define vmaCreateImage(allocator, pImageCreateInfo, pAllocationCreateInfo, pImage, pAllocation, pAllocationInfo) \
            { \
                VkResult res = (vmaCreateImage)(allocator, pImageCreateInfo, pAllocationCreateInfo, pImage, pAllocation, pAllocationInfo); \
                vmaSetAllocationName(allocator, *pAllocation, #pAllocation __LINE_STRING__); \
                res; \
            }
        #define vmaCreateBuffer(allocator, pImageCreateInfo, pAllocationCreateInfo, pImage, pAllocation, pAllocationInfo) \
            { \
                VkResult res = (vmaCreateBuffer)(allocator, pImageCreateInfo, pAllocationCreateInfo, pImage, pAllocation, pAllocationInfo); \
                vmaSetAllocationName(allocator, *pAllocation, #pAllocation " " __LINE_STRING__); \
                res; \
            }
    #endif

// #define CACHE_BINDING()
#endif // TRACE_H
