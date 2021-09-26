#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include <vector>
#include <memory>

#include "../imgui/imgui_impl_dx11.h"
#include "../imgui/imgui_impl_win32.h"
#include "WinUtil.h"

class d3dApp
{
public:
	d3dApp(HWND hWnd);
	d3dApp(const d3dApp&) = delete;
	d3dApp& operator=(const d3dApp&) = delete;
	~d3dApp();

	void EndFrame();
	void BeginFrame(float red, float green, float blue);
	void DrawIndexed(UINT count);
	
	void SetProjectionMatrix(DirectX::FXMMATRIX proj) noexcept;
	DirectX::XMMATRIX GetProjectionMatrix() const noexcept;
	void SetWorldMatrix(DirectX::FXMMATRIX world) noexcept;
	DirectX::XMMATRIX GetWorldMatrix() const noexcept;
	void SetOrthoMatrix(DirectX::FXMMATRIX ortho) noexcept;
	DirectX::XMMATRIX GetOrthoMatrix() const noexcept;

	ID3D11Device* GetDevice();
	ID3D11DeviceContext* GetContext();

	void EnableAlphaBlending();
	void DisableAlphaBlending();
	void EnableImgui();
	void DisableImgui();

	bool IsImguiEnabled() const;

private:
	bool imguiEnabled = true;

	DirectX::XMMATRIX projectionMatrix;
	DirectX::XMMATRIX worldMatrix;
	DirectX::XMMATRIX orthoMatrix;

	Microsoft::WRL::ComPtr<IDXGISwapChain> pSwapChain;
	Microsoft::WRL::ComPtr<ID3D11Device> pDevice;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pContext;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pTargetView;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> pDepthStencilView;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> pDepthStencilState;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> pDepthStencilBuffer;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> pRasterizerState;
	Microsoft::WRL::ComPtr<ID3D11BlendState> pBlendingStateEnable;
	Microsoft::WRL::ComPtr<ID3D11BlendState> pBlendingStateDisable;
};