/*
 *******************************************************************
 *** This software is copyright 1985-2014 by Michael H Riley     ***
 *** You have permission to use, modify, copy, and distribute    ***
 *** this software so long as this copyright notice is retained. ***
 *** This software may not be used in commercial applications    ***
 *** without express written permission from the author.         ***
 *******************************************************************
*/

#include "Presenter.h"

#include <stdexcept>

#include "Direct3DError.h"

namespace platform {

/* The swap chain takes the window's client size */
Presenter::Presenter(HWND window, int32_t canvasWidth, int32_t canvasHeight, UINT syncInterval)
  : window_(window), canvasWidth_(canvasWidth), canvasHeight_(canvasHeight), syncInterval_(syncInterval) {
  if (window == nullptr) throw std::invalid_argument("Presenter needs a window");
  create();
  }

/* Follows the client area; a minimized window (zero size) keeps its buffers, and a lost device rebuilds at present() */
void Presenter::resize(int32_t clientWidth, int32_t clientHeight) {
  if (clientWidth <= 0 || clientHeight <= 0 || lost_) return;

  /* The buffers may only be resized once nothing refers to them */
  context_->OMSetRenderTargets(0, nullptr, nullptr);
  target_.Reset();
  context_->Flush();

  HRESULT result = swapChain_->ResizeBuffers(0, static_cast<UINT>(clientWidth), static_cast<UINT>(clientHeight),
                                             DXGI_FORMAT_UNKNOWN, 0);
  if (result == DXGI_ERROR_DEVICE_REMOVED || result == DXGI_ERROR_DEVICE_RESET) {
    lost_ = true;
    return;
    }
  Direct3DError::check(result, "ResizeBuffers");
  createTarget();
  }

/* Uploads what changed, draws the scaled canvas on black and shows it */
void Presenter::present(FrameMailbox& frames, const Placement& placement) {
  static const float BLACK[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
  HRESULT result;

  if (lost_) recover(frames);

  frames.collect([this](const uint32_t* pixels, int32_t stride, const Rect& area) {
    scale_->upload(context_.Get(), pixels, stride, area);
    });

  context_->ClearRenderTargetView(target_.Get(), BLACK);
  scale_->draw(context_.Get(), target_.Get(), placement);

  result = swapChain_->Present(syncInterval_, 0);
  if (result == DXGI_ERROR_DEVICE_REMOVED || result == DXGI_ERROR_DEVICE_RESET) lost_ = true;
  else Direct3DError::check(result, "Present");
  }

void Presenter::create() {
  createDevice();
  createSwapChain();
  scale_ = std::make_unique<ScalePass>(device_.Get(), canvasWidth_, canvasHeight_);
  createTarget();
  }

void Presenter::createDevice() {
  static const D3D_FEATURE_LEVEL LEVELS[] = {
    D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_1, D3D_FEATURE_LEVEL_10_0
    };
  UINT    flags = 0;
  HRESULT result;

#ifdef _DEBUG
  /* The debug layer only exists where the Graphics Tools are installed */
  if (SUCCEEDED(D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_NULL, nullptr, D3D11_CREATE_DEVICE_DEBUG, nullptr, 0,
                                  D3D11_SDK_VERSION, nullptr, nullptr, nullptr)))
    flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

  /* No usable GPU (a VM, a remote session): Windows' software rasterizer */
  result = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, flags, LEVELS, ARRAYSIZE(LEVELS),
                             D3D11_SDK_VERSION, &device_, nullptr, &context_);
  if (FAILED(result))
    result = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_WARP, nullptr, flags, LEVELS, ARRAYSIZE(LEVELS),
                               D3D11_SDK_VERSION, &device_, nullptr, &context_);
  Direct3DError::check(result, "D3D11CreateDevice");
  }

void Presenter::createSwapChain() {
  Microsoft::WRL::ComPtr<IDXGIDevice>   dxgiDevice;
  Microsoft::WRL::ComPtr<IDXGIAdapter>  adapter;
  Microsoft::WRL::ComPtr<IDXGIFactory2> factory;
  DXGI_SWAP_CHAIN_DESC1                 description = {};

  Direct3DError::check(device_.As(&dxgiDevice), "IDXGIDevice");
  Direct3DError::check(dxgiDevice->GetAdapter(&adapter), "GetAdapter");
  Direct3DError::check(adapter->GetParent(IID_PPV_ARGS(&factory)), "IDXGIFactory2");

  description.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
  description.SampleDesc.Count = 1;
  description.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
  description.BufferCount = 2;
  description.Scaling = DXGI_SCALING_NONE;
  description.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
  description.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
  Direct3DError::check(factory->CreateSwapChainForHwnd(device_.Get(), window_, &description, nullptr, nullptr,
                                                       &swapChain_), "CreateSwapChainForHwnd");

  /* DXGI's own Alt+Enter would switch to exclusive fullscreen; the window handles Alt+Enter itself */
  Direct3DError::check(factory->MakeWindowAssociation(window_, DXGI_MWA_NO_ALT_ENTER), "MakeWindowAssociation");
  }

void Presenter::createTarget() {
  Microsoft::WRL::ComPtr<ID3D11Texture2D> buffer;
  Direct3DError::check(swapChain_->GetBuffer(0, IID_PPV_ARGS(&buffer)), "GetBuffer");
  Direct3DError::check(device_->CreateRenderTargetView(buffer.Get(), nullptr, &target_), "CreateRenderTargetView");
  }

/* Everything made from the lost device goes, then all of it is made again and the whole frame uploads anew */
void Presenter::recover(FrameMailbox& frames) {
  context_->ClearState();
  context_->Flush();
  target_.Reset();
  scale_.reset();
  swapChain_.Reset();
  context_.Reset();
  device_.Reset();
  create();
  frames.invalidate();
  lost_ = false;
  }

}
