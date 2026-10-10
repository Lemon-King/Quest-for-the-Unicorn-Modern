/*
 *******************************************************************
 *** This software is copyright 1985-2014 by Michael H Riley     ***
 *** You have permission to use, modify, copy, and distribute    ***
 *** this software so long as this copyright notice is retained. ***
 *** This software may not be used in commercial applications    ***
 *** without express written permission from the author.         ***
 *******************************************************************
*/

#pragma once

#include <d3d11.h>
#include <wrl/client.h>

#include <cstdint>

#include "Rect.h"
#include "ViewportLayout.h"

namespace platform {

/* Draws the canvas texture into a render target with the scaling shader; borders are the caller's clear */
class ScalePass {
  public:
    ScalePass(ID3D11Device* device, int32_t canvasWidth, int32_t canvasHeight);

    void upload(ID3D11DeviceContext* context, const uint32_t* pixels, int32_t stride, const Rect& area);
    void draw(ID3D11DeviceContext* context, ID3D11RenderTargetView* target, const Placement& placement);

  private:
    int32_t                                          canvasWidth_;
    int32_t                                          canvasHeight_;
    Microsoft::WRL::ComPtr<ID3D11Texture2D>          texture_;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> view_;
    Microsoft::WRL::ComPtr<ID3D11VertexShader>       vertexShader_;
    Microsoft::WRL::ComPtr<ID3D11PixelShader>        pixelShader_;
    Microsoft::WRL::ComPtr<ID3D11Buffer>             constants_;
    Microsoft::WRL::ComPtr<ID3D11RasterizerState>    rasterizer_;
  };

}
