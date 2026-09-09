// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#include "D3D11Context.h"

#include "WindowWin.h"

#include <d3d11.h>
#include <dxgi.h>

#include <cstdio>

using namespace doriax;

namespace {

    // The back buffer format, and what environment() reports to sokol
    constexpr DXGI_FORMAT COLOR_FORMAT = DXGI_FORMAT_B8G8R8A8_UNORM;
    // What sokol maps SG_PIXELFORMAT_DEPTH_STENCIL to
    constexpr DXGI_FORMAT DEPTH_STENCIL_FORMAT = DXGI_FORMAT_D24_UNORM_S8_UINT;

    D3D11ContextConfig gConfig;

    ID3D11Device* gDevice = nullptr;
    ID3D11DeviceContext* gDeviceContext = nullptr;
    IDXGISwapChain* gSwapChain = nullptr;
    UINT gBufferCount = 2;

    ID3D11Texture2D* gRenderTarget = nullptr;
    ID3D11RenderTargetView* gRenderTargetView = nullptr;
    ID3D11Texture2D* gDepthStencil = nullptr;
    ID3D11DepthStencilView* gDepthStencilView = nullptr;

    int gWidth = 0;
    int gHeight = 0;
    bool gValid = false;

    template <typename T>
    void release(T*& object) {
        if (object) {
            object->Release();
            object = nullptr;
        }
    }

    void destroyRenderTarget() {
        // Releasing the views is not enough: while one is still bound the output
        // merger holds a reference to the back buffer and ResizeBuffers fails
        if (gDeviceContext) gDeviceContext->OMSetRenderTargets(0, nullptr, nullptr);
        release(gDepthStencilView);
        release(gDepthStencil);
        release(gRenderTargetView);
        release(gRenderTarget);
        gValid = false;
    }

    bool createRenderTarget(int width, int height) {
        HRESULT result = gSwapChain->GetBuffer(
            0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&gRenderTarget));
        if (FAILED(result)) {
            std::fprintf(stderr, "Error: Could not get the D3D11 back buffer (0x%08lx).\n", result);
            return false;
        }
        result = gDevice->CreateRenderTargetView(gRenderTarget, nullptr, &gRenderTargetView);
        if (FAILED(result)) {
            std::fprintf(stderr, "Error: Could not create the D3D11 render target view (0x%08lx).\n", result);
            return false;
        }

        D3D11_TEXTURE2D_DESC depthDesc{};
        depthDesc.Width = static_cast<UINT>(width);
        depthDesc.Height = static_cast<UINT>(height);
        depthDesc.MipLevels = 1;
        depthDesc.ArraySize = 1;
        depthDesc.Format = DEPTH_STENCIL_FORMAT;
        depthDesc.SampleDesc.Count = 1;
        depthDesc.Usage = D3D11_USAGE_DEFAULT;
        depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
        result = gDevice->CreateTexture2D(&depthDesc, nullptr, &gDepthStencil);
        if (FAILED(result)) {
            std::fprintf(stderr, "Error: Could not create the D3D11 depth-stencil buffer (0x%08lx).\n", result);
            return false;
        }
        result = gDevice->CreateDepthStencilView(gDepthStencil, nullptr, &gDepthStencilView);
        if (FAILED(result)) {
            std::fprintf(stderr, "Error: Could not create the D3D11 depth-stencil view (0x%08lx).\n", result);
            return false;
        }

        gWidth = width;
        gHeight = height;
        gValid = true;
        return true;
    }

    // Alt-Enter would hand fullscreen to DXGI behind the engine's back, and one
    // queued frame keeps input latency down
    void tuneDxgi() {
        IDXGIDevice1* dxgiDevice = nullptr;
        if (FAILED(gDevice->QueryInterface(
                __uuidof(IDXGIDevice1), reinterpret_cast<void**>(&dxgiDevice))))
            return;
        dxgiDevice->SetMaximumFrameLatency(1);

        IDXGIAdapter* adapter = nullptr;
        if (SUCCEEDED(dxgiDevice->GetAdapter(&adapter)) && adapter) {
            IDXGIFactory* factory = nullptr;
            if (SUCCEEDED(adapter->GetParent(
                    __uuidof(IDXGIFactory), reinterpret_cast<void**>(&factory)))) {
                factory->MakeWindowAssociation(WindowWin::handle(), DXGI_MWA_NO_ALT_ENTER);
                release(factory);
            }
            release(adapter);
        }
        release(dxgiDevice);
    }

    HRESULT createDeviceAndSwapChain(int width, int height, UINT flags,
                                     DXGI_SWAP_EFFECT swapEffect, UINT bufferCount) {
        DXGI_SWAP_CHAIN_DESC description{};
        description.BufferDesc.Width = static_cast<UINT>(width);
        description.BufferDesc.Height = static_cast<UINT>(height);
        description.BufferDesc.Format = COLOR_FORMAT;
        description.BufferDesc.RefreshRate.Numerator = 60;
        description.BufferDesc.RefreshRate.Denominator = 1;
        description.SampleDesc.Count = 1;
        description.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        description.BufferCount = bufferCount;
        description.OutputWindow = WindowWin::handle();
        description.Windowed = TRUE;
        description.SwapEffect = swapEffect;

        const D3D_FEATURE_LEVEL featureLevels[] = {
            D3D_FEATURE_LEVEL_11_1,
            D3D_FEATURE_LEVEL_11_0
        };
        D3D_FEATURE_LEVEL obtainedLevel{};
        const HRESULT result = D3D11CreateDeviceAndSwapChain(
            nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, flags,
            featureLevels, 2, D3D11_SDK_VERSION, &description, &gSwapChain,
            &gDevice, &obtainedLevel, &gDeviceContext);
        if (SUCCEEDED(result)) gBufferCount = bufferCount;
        return result;
    }

    // The flip model is the one Windows composites without an extra copy, but it
    // needs two buffers and is missing on older Windows, so fall back to the
    // blit model rather than failing outright
    HRESULT createDeviceWithFallback(int width, int height, UINT flags) {
        HRESULT result = createDeviceAndSwapChain(
            width, height, flags, DXGI_SWAP_EFFECT_FLIP_DISCARD, 2);
        if (FAILED(result)) {
            release(gSwapChain);
            release(gDeviceContext);
            release(gDevice);
            result = createDeviceAndSwapChain(
                width, height, flags, DXGI_SWAP_EFFECT_DISCARD, 1);
        }
        return result;
    }

}

bool D3D11Context::create(const D3D11ContextConfig& config) {
    gConfig = config;

    int width = 0;
    int height = 0;
    WindowWin::getClientSize(width, height);
    if (width <= 0) width = 1;
    if (height <= 0) height = 1;

    UINT flags = D3D11_CREATE_DEVICE_SINGLETHREADED | D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#if defined(_DEBUG) || defined(SOKOL_DEBUG)
    flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    HRESULT result = createDeviceWithFallback(width, height, flags);
#if defined(_DEBUG) || defined(SOKOL_DEBUG)
    if (FAILED(result)) {
        // The debug layer ships with the Windows SDK, not with Windows, so on a
        // machine without it that flag alone fails device creation
        release(gSwapChain);
        release(gDeviceContext);
        release(gDevice);
        flags &= ~static_cast<UINT>(D3D11_CREATE_DEVICE_DEBUG);
        result = createDeviceWithFallback(width, height, flags);
    }
#endif
    if (FAILED(result)) {
        std::fprintf(stderr, "Error: Could not create the D3D11 device (0x%08lx).\n", result);
        return false;
    }

    tuneDxgi();
    return createRenderTarget(width, height);
}

void D3D11Context::destroy() {
    destroyRenderTarget();
    release(gSwapChain);
    release(gDeviceContext);
    release(gDevice);
    gWidth = 0;
    gHeight = 0;
}

void D3D11Context::beginFrame() {
    if (!gSwapChain) return;

    int width = 0;
    int height = 0;
    WindowWin::getClientSize(width, height);
    // Minimized: keep the buffers as they are until the window comes back
    if (width <= 0 || height <= 0) {
        gValid = false;
        return;
    }

    if (gValid && width == gWidth && height == gHeight) return;

    destroyRenderTarget();
    const HRESULT result = gSwapChain->ResizeBuffers(
        gBufferCount, static_cast<UINT>(width), static_cast<UINT>(height), COLOR_FORMAT, 0);
    if (FAILED(result)) {
        std::fprintf(stderr, "Error: Could not resize the D3D11 swapchain (0x%08lx).\n", result);
        return;
    }
    createRenderTarget(width, height);
}

void D3D11Context::endFrame() {
    if (!gValid) return;
    const HRESULT result = gSwapChain->Present(gConfig.vsync ? 1 : 0, 0);
    if (FAILED(result))
        std::fprintf(stderr, "Error: Could not present the D3D11 swapchain (0x%08lx).\n", result);
}

sg_environment D3D11Context::environment() {
    sg_environment environment{};
    environment.defaults.sample_count = 1;
    // Pipelines are built from these defaults, so the color format has to be the
    // swapchain's or none of them would match the swapchain pass
    environment.defaults.color_format = SG_PIXELFORMAT_BGRA8;
    environment.defaults.depth_format = SG_PIXELFORMAT_DEPTH_STENCIL;
    environment.d3d11.device = gDevice;
    environment.d3d11.device_context = gDeviceContext;
    return environment;
}

sg_swapchain D3D11Context::swapchain() {
    sg_swapchain swapchain{};
    swapchain.invalid = !gValid;
    swapchain.width = gWidth;
    swapchain.height = gHeight;
    swapchain.sample_count = 1;
    swapchain.color_format = SG_PIXELFORMAT_BGRA8;
    swapchain.depth_format = SG_PIXELFORMAT_DEPTH_STENCIL;
    if (swapchain.invalid) return swapchain;

    swapchain.d3d11.render_view = gRenderTargetView;
    swapchain.d3d11.depth_stencil_view = gDepthStencilView;
    return swapchain;
}
