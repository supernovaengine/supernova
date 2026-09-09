// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#include "GraphicUtils.h"

#include "Out.h"
#include "System.h"
#include "stb_image_write.h"
#include "util/STBText.h"

#include <cstring>
#include <future>
#include <thread>
#include <utility>

#ifndef USE_GL_READPIXELS
#define USE_GL_READPIXELS 0
#endif

#if defined(SOKOL_METAL)
    #include <TargetConditionals.h>
    #import <Metal/Metal.h>
#endif

#if defined(SOKOL_D3D11)
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
    #include <d3d11.h>
    #include <wrl/client.h>
    using Microsoft::WRL::ComPtr;
#endif

#if defined(SOKOL_GLCORE) || defined(SOKOL_GLES3)
    #ifdef __APPLE__
        #include <OpenGL/gl.h>
    #elif defined(_WIN32)
        #include <windows.h>
        #include <GL/gl.h>
    #else
        #define GL_GLEXT_PROTOTYPES
        #include <GL/gl.h>
    #endif

    #if USE_GL_READPIXELS && defined(_WIN32)
        typedef void (APIENTRY *PFNGLGENFRAMEBUFFERSPROC)(GLsizei n, GLuint* framebuffers);
        typedef void (APIENTRY *PFNGLBINDFRAMEBUFFERPROC)(GLenum target, GLuint framebuffer);
        typedef void (APIENTRY *PFNGLFRAMEBUFFERTEXTURE2DPROC)(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
        typedef void (APIENTRY *PFNGLDELETEFRAMEBUFFERSPROC)(GLsizei n, const GLuint* framebuffers);
        static PFNGLGENFRAMEBUFFERSPROC glGenFramebuffersPtr = nullptr;
        static PFNGLBINDFRAMEBUFFERPROC glBindFramebufferPtr = nullptr;
        static PFNGLFRAMEBUFFERTEXTURE2DPROC glFramebufferTexture2DPtr = nullptr;
        static PFNGLDELETEFRAMEBUFFERSPROC glDeleteFramebuffersPtr = nullptr;
        #define glGenFramebuffers glGenFramebuffersPtr
        #define glBindFramebuffer glBindFramebufferPtr
        #define glFramebufferTexture2D glFramebufferTexture2DPtr
        #define glDeleteFramebuffers glDeleteFramebuffersPtr

        #ifndef GL_FRAMEBUFFER
        #define GL_FRAMEBUFFER 0x8D40
        #endif
        #ifndef GL_COLOR_ATTACHMENT0
        #define GL_COLOR_ATTACHMENT0 0x8CE0
        #endif
    #endif
#endif

#if defined(SOKOL_VULKAN)
    #include <vulkan/vulkan.h>
#endif

using namespace doriax;

Vector2 editor::GraphicUtils::getUILayoutCenter(Scene* scene, Entity entity, const UILayoutComponent& layout) {
    Vector2 center = Vector2(0, 0);
    Signature signature = scene->getSignature(entity);
    if (signature.test(scene->getComponentId<TextComponent>())) {
        TextComponent& text = scene->getComponent<TextComponent>(entity);
        if (text.pivotBaseline) {
            center.y = text.stbtext->getAscent();
        }
        if (text.pivotCentered) {
            center.x = layout.width / 2.0f;
        }
    }
    return center;
}

bool editor::GraphicUtils::saveFramebufferImage(Framebuffer* framebuffer, fs::path path, bool flipY, std::function<void()> onComplete) {
    uint8_t* pixels = nullptr;
    bool needDelete = false;

    unsigned int width = framebuffer->getWidth();
    unsigned int height = framebuffer->getHeight();

    #if defined(SOKOL_GLCORE) || defined(SOKOL_GLES3)
        // OpenGL
        pixels = new uint8_t[width * height * 4];
        needDelete = true;

        #if USE_GL_READPIXELS
            #if defined(_WIN32)
                if (!glGenFramebuffersPtr) {
                    glGenFramebuffersPtr = (PFNGLGENFRAMEBUFFERSPROC)wglGetProcAddress("glGenFramebuffers");
                    glBindFramebufferPtr = (PFNGLBINDFRAMEBUFFERPROC)wglGetProcAddress("glBindFramebuffer");
                    glFramebufferTexture2DPtr = (PFNGLFRAMEBUFFERTEXTURE2DPROC)wglGetProcAddress("glFramebufferTexture2D");
                    glDeleteFramebuffersPtr = (PFNGLDELETEFRAMEBUFFERSPROC)wglGetProcAddress("glDeleteFramebuffers");
                    if (!glGenFramebuffersPtr || !glBindFramebufferPtr || !glFramebufferTexture2DPtr || !glDeleteFramebuffersPtr) {
                        delete[] pixels;
                        Out::error("Engine failure: Failed to load GL framebuffer functions");
                        return false;
                    }
                }
            #endif
            // sokol no longer exposes its internal FBOs, read through a temporary
            // one attached to the framebuffer color texture
            GLuint readFbo = 0;
            glGenFramebuffers(1, &readFbo);
            glBindFramebuffer(GL_FRAMEBUFFER, readFbo);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, framebuffer->getRender().getColorTexture().getGLHandler(), 0);
            glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glDeleteFramebuffers(1, &readFbo);
        #else
            glBindTexture(GL_TEXTURE_2D, framebuffer->getRender().getColorTexture().getGLHandler());
            glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
            glBindTexture(GL_TEXTURE_2D, 0);
        #endif
    #endif

    #if defined(SOKOL_VULKAN)
        sg_environment environment = System::instance().getSokolEnvironment();
        VkPhysicalDevice physicalDevice = reinterpret_cast<VkPhysicalDevice>(
            const_cast<void*>(environment.vulkan.physical_device));
        VkDevice device = reinterpret_cast<VkDevice>(
            const_cast<void*>(environment.vulkan.device));
        VkQueue queue = reinterpret_cast<VkQueue>(
            const_cast<void*>(environment.vulkan.queue));
        VkImage image = reinterpret_cast<VkImage>(const_cast<void*>(
            framebuffer->getRender().getColorTexture().getVulkanImageHandler()));
        const VkDeviceSize dataSize = static_cast<VkDeviceSize>(width) * height * 4;
        VkBuffer stagingBuffer = VK_NULL_HANDLE;
        VkDeviceMemory stagingMemory = VK_NULL_HANDLE;
        VkCommandPool commandPool = VK_NULL_HANDLE;

        auto cleanupVulkan = [&]() {
            if (commandPool != VK_NULL_HANDLE)
                vkDestroyCommandPool(device, commandPool, nullptr);
            if (stagingBuffer != VK_NULL_HANDLE)
                vkDestroyBuffer(device, stagingBuffer, nullptr);
            if (stagingMemory != VK_NULL_HANDLE)
                vkFreeMemory(device, stagingMemory, nullptr);
        };

        if (physicalDevice == VK_NULL_HANDLE || device == VK_NULL_HANDLE ||
            queue == VK_NULL_HANDLE || image == VK_NULL_HANDLE ||
            vkQueueWaitIdle(queue) != VK_SUCCESS) {
            Out::error("Engine failure: Vulkan framebuffer readback is unavailable");
            return false;
        }

        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = dataSize;
        bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        if (vkCreateBuffer(device, &bufferInfo, nullptr, &stagingBuffer) != VK_SUCCESS) {
            Out::error("Engine failure: Could not create Vulkan readback buffer");
            return false;
        }

        VkMemoryRequirements requirements{};
        vkGetBufferMemoryRequirements(device, stagingBuffer, &requirements);
        VkPhysicalDeviceMemoryProperties memoryProperties{};
        vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memoryProperties);
        uint32_t memoryType = UINT32_MAX;
        const VkMemoryPropertyFlags wanted =
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
        for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; ++i) {
            if ((requirements.memoryTypeBits & (1u << i)) &&
                (memoryProperties.memoryTypes[i].propertyFlags & wanted) == wanted) {
                memoryType = i;
                break;
            }
        }
        if (memoryType == UINT32_MAX) {
            cleanupVulkan();
            Out::error("Engine failure: No host-visible Vulkan readback memory");
            return false;
        }

        VkMemoryAllocateInfo memoryInfo{};
        memoryInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        memoryInfo.allocationSize = requirements.size;
        memoryInfo.memoryTypeIndex = memoryType;
        if (vkAllocateMemory(device, &memoryInfo, nullptr, &stagingMemory) != VK_SUCCESS ||
            vkBindBufferMemory(device, stagingBuffer, stagingMemory, 0) != VK_SUCCESS) {
            cleanupVulkan();
            Out::error("Engine failure: Could not allocate Vulkan readback memory");
            return false;
        }

        VkCommandPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
        poolInfo.queueFamilyIndex = environment.vulkan.queue_family_index;
        if (vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool) != VK_SUCCESS) {
            cleanupVulkan();
            Out::error("Engine failure: Could not create Vulkan readback command pool");
            return false;
        }

        VkCommandBuffer commandBuffer = VK_NULL_HANDLE;
        VkCommandBufferAllocateInfo commandInfo{};
        commandInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        commandInfo.commandPool = commandPool;
        commandInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        commandInfo.commandBufferCount = 1;
        if (vkAllocateCommandBuffers(device, &commandInfo, &commandBuffer) != VK_SUCCESS) {
            cleanupVulkan();
            Out::error("Engine failure: Could not allocate Vulkan readback command buffer");
            return false;
        }

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
            cleanupVulkan();
            Out::error("Engine failure: Could not begin Vulkan readback command buffer");
            return false;
        }

        VkImageMemoryBarrier toTransfer{};
        toTransfer.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        toTransfer.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
        toTransfer.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
        toTransfer.oldLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        toTransfer.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        toTransfer.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        toTransfer.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        toTransfer.image = image;
        toTransfer.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        toTransfer.subresourceRange.levelCount = 1;
        toTransfer.subresourceRange.layerCount = 1;
        vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
            VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &toTransfer);

        VkBufferImageCopy copy{};
        copy.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        copy.imageSubresource.layerCount = 1;
        copy.imageExtent = {width, height, 1};
        vkCmdCopyImageToBuffer(commandBuffer, image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                               stagingBuffer, 1, &copy);

        VkImageMemoryBarrier toTexture = toTransfer;
        toTexture.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
        toTexture.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        toTexture.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        toTexture.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT,
            VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 0, nullptr, 0, nullptr, 1, &toTexture);
        if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
            cleanupVulkan();
            Out::error("Engine failure: Could not end Vulkan readback command buffer");
            return false;
        }

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;
        if (vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS ||
            vkQueueWaitIdle(queue) != VK_SUCCESS) {
            cleanupVulkan();
            Out::error("Engine failure: Vulkan framebuffer readback failed");
            return false;
        }

        void* mapped = nullptr;
        if (vkMapMemory(device, stagingMemory, 0, dataSize, 0, &mapped) != VK_SUCCESS) {
            cleanupVulkan();
            Out::error("Engine failure: Could not map Vulkan readback memory");
            return false;
        }
        pixels = new uint8_t[static_cast<size_t>(dataSize)];
        std::memcpy(pixels, mapped, static_cast<size_t>(dataSize));
        vkUnmapMemory(device, stagingMemory);
        cleanupVulkan();
        needDelete = true;
    #endif

    #if defined(SOKOL_METAL)
        // Metal
        auto metalTexPtr = framebuffer->getRender().getColorTexture().getMetalHandler();
        id<MTLTexture> tex = (__bridge id<MTLTexture>)metalTexPtr;
        id<MTLDevice> device = [tex device];
        NSUInteger bytesPerRow = width * 4;
        NSUInteger dataSize = bytesPerRow * height;
        id<MTLBuffer> buffer = [device newBufferWithLength:dataSize options:MTLResourceStorageModeShared];

        id<MTLCommandQueue> queue = [device newCommandQueue];
        id<MTLCommandBuffer> cmdBuf = [queue commandBuffer];
        id<MTLBlitCommandEncoder> blit = [cmdBuf blitCommandEncoder];

        [blit copyFromTexture:tex
                 sourceSlice:0
                 sourceLevel:0
                sourceOrigin:MTLOriginMake(0, 0, 0)
                  sourceSize:MTLSizeMake(width, height, 1)
                   toBuffer:buffer
           destinationOffset:0
      destinationBytesPerRow:bytesPerRow
    destinationBytesPerImage:dataSize];
        [blit endEncoding];
        [cmdBuf commit];
        [cmdBuf waitUntilCompleted];

        // The Metal buffer is released when this function returns, while PNG
        // encoding happens on a detached thread. Give that thread owned bytes.
        pixels = new uint8_t[dataSize];
        memcpy(pixels, [buffer contents], dataSize);
        needDelete = true;
    #endif

    #if defined(SOKOL_D3D11)
        // D3D11
        auto rtvPtr = framebuffer->getRender().getD3D11HandlerColorRTV();
        ID3D11RenderTargetView* rtv = reinterpret_cast<ID3D11RenderTargetView*>(const_cast<void*>(rtvPtr));

        ID3D11Device* device = nullptr;
        ID3D11DeviceContext* context = nullptr;
        ID3D11Resource* renderTargetResource = nullptr;
        D3D11_TEXTURE2D_DESC desc = {};
        ID3D11Texture2D* staging = nullptr;
        pixels = new uint8_t[width * height * 4];
        needDelete = true;

        rtv->GetDevice(&device);
        device->GetImmediateContext(&context);
        rtv->GetResource(&renderTargetResource);
        ID3D11Texture2D* srcTex = nullptr;
        renderTargetResource->QueryInterface(__uuidof(ID3D11Texture2D), (void**)&srcTex);
        srcTex->GetDesc(&desc);

        // Create staging texture
        D3D11_TEXTURE2D_DESC stagingDesc = desc;
        stagingDesc.Usage = D3D11_USAGE_STAGING;
        stagingDesc.BindFlags = 0;
        stagingDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
        stagingDesc.MiscFlags = 0;
        device->CreateTexture2D(&stagingDesc, nullptr, &staging);

        // Copy to staging
        context->CopyResource(staging, srcTex);

        // Map and read
        D3D11_MAPPED_SUBRESOURCE mapped = {};
        context->Map(staging, 0, D3D11_MAP_READ, 0, &mapped);

        for (int y = 0; y < height; ++y) {
            memcpy(&pixels[y * width * 4],
                (uint8_t*)mapped.pData + y * mapped.RowPitch,
                width * 4);
        }

        // Cleanup
        context->Unmap(staging, 0);
        staging->Release();
        srcTex->Release();
        renderTargetResource->Release();
        context->Release();
        device->Release();
    #endif

    try {
        std::thread([pixels, needDelete, width, height, path, flipY, onComplete]() {
            // Do not use stb's process-global flip flag from concurrent writers.
            // The readback buffer is owned here, so flip its rows directly.
            if (flipY && pixels) {
                const size_t rowBytes = static_cast<size_t>(width) * 4;
                for (unsigned int y = 0; y < height / 2; ++y) {
                    uint8_t* top = pixels + static_cast<size_t>(y) * rowBytes;
                    uint8_t* bottom = pixels + static_cast<size_t>(height - 1 - y) * rowBytes;
                    for (size_t x = 0; x < rowBytes; ++x) {
                        std::swap(top[x], bottom[x]);
                    }
                }
            }
            stbi_write_png(path.string().c_str(), width, height, 4, pixels, width * 4);

            if (needDelete && pixels) {
                delete[] pixels;
            }

            if (onComplete) {
                onComplete();
            }
        }).detach();
    } catch (...) {
        if (needDelete && pixels) {
            delete[] pixels;
        }
        return false;
    }
    return true;
}
