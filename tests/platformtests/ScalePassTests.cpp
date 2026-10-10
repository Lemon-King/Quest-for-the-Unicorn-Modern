/*
 *******************************************************************
 *** This software is copyright 1985-2014 by Michael H Riley     ***
 *** You have permission to use, modify, copy, and distribute    ***
 *** this software so long as this copyright notice is retained. ***
 *** This software may not be used in commercial applications    ***
 *** without express written permission from the author.         ***
 *******************************************************************
*/

/* The Direct3D scaler draws exactly the window pixels the GDI build's CPU viewport drew, borders included */

#include <d3d11.h>
#include <wrl/client.h>

#include <cstdio>
#include <cstring>
#include <exception>
#include <string>
#include <vector>

#include "Direct3DError.h"
#include "ReferenceScaler.h"
#include "ScalePass.h"
#include "TestRunner.h"
#include "ViewportLayout.h"

using Microsoft::WRL::ComPtr;
using platform::Direct3DError;

namespace {

constexpr int32_t CANVAS_WIDTH = 640;
constexpr int32_t CANVAS_HEIGHT = 480;

/* Every channel changes from one pixel to the next, so a blend with the wrong neighbour or weight shows */
std::vector<uint32_t> testPattern() {
  std::vector<uint32_t> pixels(static_cast<size_t>(CANVAS_WIDTH) * CANVAS_HEIGHT);
  for (int32_t y = 0; y < CANVAS_HEIGHT; y++)
    for (int32_t x = 0; x < CANVAS_WIDTH; x++)
      pixels[static_cast<size_t>(y) * CANVAS_WIDTH + x] = (static_cast<uint32_t>(x * 37 + y * 101) * 0x010203u) & 0xffffffu;
  return pixels;
  }

/* The client area as ScalePass draws it over a black clear, read back as 0x00RRGGBB */
std::vector<uint32_t> drawClient(ID3D11Device* device, ID3D11DeviceContext* context, platform::ScalePass& pass,
                                 int32_t clientWidth, int32_t clientHeight) {
  static const float BLACK[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
  ComPtr<ID3D11Texture2D>        target;
  ComPtr<ID3D11RenderTargetView> view;
  ComPtr<ID3D11Texture2D>        staging;
  D3D11_TEXTURE2D_DESC           description = {};
  D3D11_MAPPED_SUBRESOURCE       mapped = {};

  description.Width = static_cast<UINT>(clientWidth);
  description.Height = static_cast<UINT>(clientHeight);
  description.MipLevels = 1;
  description.ArraySize = 1;
  description.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
  description.SampleDesc.Count = 1;
  description.Usage = D3D11_USAGE_DEFAULT;
  description.BindFlags = D3D11_BIND_RENDER_TARGET;
  Direct3DError::check(device->CreateTexture2D(&description, nullptr, &target), "CreateTexture2D");
  Direct3DError::check(device->CreateRenderTargetView(target.Get(), nullptr, &view), "CreateRenderTargetView");

  description.Usage = D3D11_USAGE_STAGING;
  description.BindFlags = 0;
  description.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
  Direct3DError::check(device->CreateTexture2D(&description, nullptr, &staging), "CreateTexture2D");

  context->ClearRenderTargetView(view.Get(), BLACK);
  pass.draw(context, view.Get(), platform::ViewportLayout(CANVAS_WIDTH, CANVAS_HEIGHT).place(clientWidth, clientHeight));
  context->CopyResource(staging.Get(), target.Get());

  std::vector<uint32_t> pixels(static_cast<size_t>(clientWidth) * static_cast<size_t>(clientHeight));
  Direct3DError::check(context->Map(staging.Get(), 0, D3D11_MAP_READ, 0, &mapped), "Map");
  for (int32_t y = 0; y < clientHeight; y++)
    std::memcpy(&pixels[static_cast<size_t>(y) * static_cast<size_t>(clientWidth)],
                static_cast<const uint8_t*>(mapped.pData) + static_cast<size_t>(y) * mapped.RowPitch,
                static_cast<size_t>(clientWidth) * sizeof(uint32_t));
  context->Unmap(staging.Get(), 0);

  for (uint32_t& pixel : pixels) pixel &= 0x00ffffff;
  return pixels;
  }

/* "" when they match, else where and how they first differ */
std::string difference(const std::vector<uint32_t>& gpu, const std::vector<uint32_t>& cpu, int32_t clientWidth) {
  for (size_t i = 0; i < gpu.size() && i < cpu.size(); i++)
    if (gpu[i] != cpu[i]) {
      char detail[96];
      std::snprintf(detail, sizeof(detail), "at %d,%d Direct3D #%06x, CPU #%06x", static_cast<int>(i % clientWidth),
                    static_cast<int>(i / clientWidth), static_cast<unsigned>(gpu[i]), static_cast<unsigned>(cpu[i]));
      return detail;
      }
  return gpu.size() == cpu.size() ? "" : "sizes differ";
  }

void compareClient(TestRunner& runner, ID3D11Device* device, ID3D11DeviceContext* context, platform::ScalePass& pass,
                   const std::vector<uint32_t>& canvas, int32_t clientWidth, int32_t clientHeight, const std::string& name) {
  std::vector<uint32_t> gpu = drawClient(device, context, pass, clientWidth, clientHeight);
  std::vector<uint32_t> cpu = ReferenceScaler(canvas.data(), CANVAS_WIDTH, CANVAS_HEIGHT).window(clientWidth, clientHeight);
  std::string           found = difference(gpu, cpu, clientWidth);
  runner.check(found.empty(), "ScalePassMatchesCpuScaler",
               name + " " + std::to_string(clientWidth) + "x" + std::to_string(clientHeight) + ": " + found);
  }

void scalePassMatchesCpuScaler(TestRunner& runner) {
  static const D3D_FEATURE_LEVEL LEVELS[] = {
    D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_1, D3D_FEATURE_LEVEL_10_0
    };
  ComPtr<ID3D11Device>        device;
  ComPtr<ID3D11DeviceContext> context;
  HRESULT result = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_WARP, nullptr, 0, LEVELS, ARRAYSIZE(LEVELS),
                                     D3D11_SDK_VERSION, &device, nullptr, &context);
  if (FAILED(result)) {
    runner.check(false, "ScalePassMatchesCpuScaler", Direct3DError(result, "D3D11CreateDevice (WARP)").what());
    return;
    }

  try {
    std::vector<uint32_t> canvas = testPattern();
    platform::ScalePass   pass(device.Get(), CANVAS_WIDTH, CANVAS_HEIGHT);
    pass.upload(context.Get(), canvas.data(), CANVAS_WIDTH, platform::Rect{0, 0, CANVAS_WIDTH, CANVAS_HEIGHT});

    const int32_t clients[][2] = { {640, 480}, {1280, 960}, {1500, 1000}, {2000, 1000} };
    for (const auto& client : clients)
      compareClient(runner, device.Get(), context.Get(), pass, canvas, client[0], client[1], "whole upload");

    /* Only a changed area goes up, as the game's frames do: rows a stride apart, starting inside the canvas */
    for (int32_t y = 50; y < 200; y++)
      for (int32_t x = 100; x < 300; x++) canvas[static_cast<size_t>(y) * CANVAS_WIDTH + x] ^= 0x00ffffff;
    pass.upload(context.Get(), canvas.data(), CANVAS_WIDTH, platform::Rect{100, 50, 300, 200});
    compareClient(runner, device.Get(), context.Get(), pass, canvas, 640, 480, "after a partial upload");
    compareClient(runner, device.Get(), context.Get(), pass, canvas, 1500, 1000, "after a partial upload");
    }
  catch (const std::exception& error) {
    runner.check(false, "ScalePassMatchesCpuScaler", error.what());
    }
  }

}

void runScalePassTests(TestRunner& runner) {
  scalePassMatchesCpuScaler(runner);
  }
