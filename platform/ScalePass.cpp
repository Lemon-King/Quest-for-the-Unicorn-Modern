/*
 *******************************************************************
 *** This software is copyright 1985-2014 by Michael H Riley     ***
 *** You have permission to use, modify, copy, and distribute    ***
 *** this software so long as this copyright notice is retained. ***
 *** This software may not be used in commercial applications    ***
 *** without express written permission from the author.         ***
 *******************************************************************
*/

#include "ScalePass.h"

#include <stdexcept>

#include "Direct3DError.h"

/* Compiled from shaders\*.hlsl into $(IntDir) by the FxCompile items */
#include "ScalePixel.h"
#include "ScaleVertex.h"

namespace platform {

/* The texture holds 0x00RRGGBB as B8G8R8A8, so canvas pixels upload without conversion */
ScalePass::ScalePass(ID3D11Device* device, int32_t canvasWidth, int32_t canvasHeight)
  : canvasWidth_(canvasWidth), canvasHeight_(canvasHeight) {
  if (device == nullptr) throw std::invalid_argument("ScalePass needs a device");
  if (canvasWidth < 1 || canvasHeight < 1) throw std::invalid_argument("canvas size must be positive");

  D3D11_TEXTURE2D_DESC texture = {};
  texture.Width = static_cast<UINT>(canvasWidth);
  texture.Height = static_cast<UINT>(canvasHeight);
  texture.MipLevels = 1;
  texture.ArraySize = 1;
  texture.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
  texture.SampleDesc.Count = 1;
  texture.Usage = D3D11_USAGE_DEFAULT;
  texture.BindFlags = D3D11_BIND_SHADER_RESOURCE;
  Direct3DError::check(device->CreateTexture2D(&texture, nullptr, &texture_), "CreateTexture2D");
  Direct3DError::check(device->CreateShaderResourceView(texture_.Get(), nullptr, &view_), "CreateShaderResourceView");

  Direct3DError::check(device->CreateVertexShader(SCALE_VERTEX_SHADER, sizeof(SCALE_VERTEX_SHADER), nullptr,
                                                  &vertexShader_), "CreateVertexShader");
  Direct3DError::check(device->CreatePixelShader(SCALE_PIXEL_SHADER, sizeof(SCALE_PIXEL_SHADER), nullptr, &pixelShader_),
                       "CreatePixelShader");

  D3D11_BUFFER_DESC constants = {};
  constants.ByteWidth = 32;
  constants.Usage = D3D11_USAGE_DEFAULT;
  constants.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
  Direct3DError::check(device->CreateBuffer(&constants, nullptr, &constants_), "CreateBuffer");

  D3D11_RASTERIZER_DESC rasterizer = {};
  rasterizer.FillMode = D3D11_FILL_SOLID;
  rasterizer.CullMode = D3D11_CULL_NONE;
  rasterizer.DepthClipEnable = TRUE;
  Direct3DError::check(device->CreateRasterizerState(&rasterizer, &rasterizer_), "CreateRasterizerState");
  }

/* Copies one area of the canvas, whose rows are stride pixels apart, into the texture */
void ScalePass::upload(ID3D11DeviceContext* context, const uint32_t* pixels, int32_t stride, const Rect& area) {
  D3D11_BOX box = { static_cast<UINT>(area.left), static_cast<UINT>(area.top), 0,
                    static_cast<UINT>(area.right), static_cast<UINT>(area.bottom), 1 };
  context->UpdateSubresource(texture_.Get(), 0, &box, pixels + static_cast<size_t>(area.top) * stride + area.left,
                             static_cast<UINT>(stride * sizeof(uint32_t)), 0);
  }

/* The shader reads the placement to find each window pixel's place in the canvas */
void ScalePass::draw(ID3D11DeviceContext* context, ID3D11RenderTargetView* target, const Placement& placement) {
  struct Constants {
    int32_t size[2];
    int32_t scaled[2];
    int32_t origin[2];
    int32_t padding[2];
    } constants = { { canvasWidth_, canvasHeight_ }, { placement.width, placement.height },
                    { placement.left, placement.top }, { 0, 0 } };
  D3D11_VIEWPORT viewport = { static_cast<float>(placement.left), static_cast<float>(placement.top),
                              static_cast<float>(placement.width), static_cast<float>(placement.height), 0.0f, 1.0f };

  context->UpdateSubresource(constants_.Get(), 0, nullptr, &constants, 0, 0);
  context->OMSetRenderTargets(1, &target, nullptr);
  context->RSSetViewports(1, &viewport);
  context->RSSetState(rasterizer_.Get());
  context->IASetInputLayout(nullptr);
  context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
  context->VSSetShader(vertexShader_.Get(), nullptr, 0);
  context->PSSetShader(pixelShader_.Get(), nullptr, 0);
  context->PSSetShaderResources(0, 1, view_.GetAddressOf());
  context->PSSetConstantBuffers(0, 1, constants_.GetAddressOf());
  context->Draw(3, 0);
  }

}
