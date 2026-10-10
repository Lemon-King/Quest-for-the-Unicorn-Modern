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
#include <dxgi1_2.h>
#include <wrl/client.h>

#include <cstdint>
#include <memory>

#include "FrameMailbox.h"
#include "ScalePass.h"
#include "ViewportLayout.h"

namespace platform {

/* Shows frames in a window through a Direct3D 11 flip-model swap chain, rebuilding everything if the device is lost */
class Presenter {
  public:
    Presenter(HWND window, int32_t canvasWidth, int32_t canvasHeight, UINT syncInterval);

    void resize(int32_t clientWidth, int32_t clientHeight);
    void present(FrameMailbox& frames, const Placement& placement);

  private:
    void create();
    void createDevice();
    void createSwapChain();
    void createTarget();
    void recover(FrameMailbox& frames);

    HWND                                           window_;
    int32_t                                        canvasWidth_;
    int32_t                                        canvasHeight_;
    UINT                                           syncInterval_;
    Microsoft::WRL::ComPtr<ID3D11Device>           device_;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext>    context_;
    Microsoft::WRL::ComPtr<IDXGISwapChain1>        swapChain_;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> target_;
    std::unique_ptr<ScalePass>                     scale_;
    bool                                           lost_ = false;
  };

}
