#include "d3dApp.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "D3DCompiler.lib")

d3dApp::d3dApp(HWND hWnd)
{
	//Create swap chain
	DXGI_SWAP_CHAIN_DESC scd = {};
	scd.BufferCount = 2;
	scd.BufferDesc.Width = 0;
	scd.BufferDesc.Height = 0;
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scd.BufferDesc.RefreshRate.Numerator = 60;
	scd.BufferDesc.RefreshRate.Denominator = 1;
	scd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	scd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	scd.SampleDesc.Count = 1;
	scd.SampleDesc.Quality = 0;
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scd.OutputWindow = hWnd;
	scd.Windowed = TRUE;
	scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	scd.Flags = 0;

	D3D11CreateDeviceAndSwapChain(
		nullptr, 
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		0u, 
		nullptr,
		0u,
		D3D11_SDK_VERSION,
		&scd,
		&pSwapChain, &pDevice, nullptr, &pContext
	);

	//Create back buffer
	Microsoft::WRL::ComPtr<ID3D11Texture2D> pBackBuffer;
	pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), &pBackBuffer);
	pDevice->CreateRenderTargetView(pBackBuffer.Get(), nullptr, &pTargetView);

	//Create depth buffer
	D3D11_TEXTURE2D_DESC depthBufferd;
	depthBufferd.Width = 800u;
	depthBufferd.Height = 600u;
	depthBufferd.MipLevels = 1u;
	depthBufferd.ArraySize = 1u;
	depthBufferd.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferd.SampleDesc.Count = 1u;
	depthBufferd.SampleDesc.Quality = 0u;
	depthBufferd.Usage = D3D11_USAGE_DEFAULT;
	depthBufferd.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferd.CPUAccessFlags = 0u;
	depthBufferd.MiscFlags = 0u;
	pDevice->CreateTexture2D(&depthBufferd, nullptr, &pDepthStencilBuffer);

	//Depth stencil state
	D3D11_DEPTH_STENCIL_DESC dsd;
	dsd.DepthEnable = TRUE;
	dsd.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsd.DepthFunc = D3D11_COMPARISON_LESS;
	dsd.StencilEnable = TRUE;
	dsd.StencilReadMask = 0xFF;
	dsd.StencilWriteMask = 0xFF;
	//if pixel front facing
	dsd.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsd.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	dsd.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsd.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	//if pixel back facing
	dsd.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsd.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	dsd.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsd.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	pDevice->CreateDepthStencilState(&dsd, &pDepthStencilState);
	//Bind dss
	pContext->OMSetDepthStencilState(pDepthStencilState.Get(), 1u);

	//Create depth stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvd;
	dsvd.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvd.Texture2D.MipSlice = 0u;
	pDevice->CreateDepthStencilView(pDepthStencilBuffer.Get(), &dsvd, &pDepthStencilView);

	pContext->OMSetRenderTargets(1u, pTargetView.GetAddressOf(), pDepthStencilView.Get());
	
	//Rasterizer description
	D3D11_RASTERIZER_DESC rasterDesc;
	rasterDesc.AntialiasedLineEnable = FALSE;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = TRUE;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = FALSE;
	rasterDesc.MultisampleEnable = FALSE;
	rasterDesc.ScissorEnable = FALSE;
	rasterDesc.SlopeScaledDepthBias = 0.0f;
	pDevice->CreateRasterizerState(&rasterDesc, &pRasterizerState);

	pContext->RSSetState(pRasterizerState.Get());
	
	//Set up viewPort
	D3D11_VIEWPORT vp;
	vp.Width = 800.0f;
	vp.Height = 600.0f;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0.0f;
	vp.TopLeftY = 0.0f;
	pContext->RSSetViewports(1u, &vp);

	//Set up and create projection matrix for 3d rendering
	float fieldOfView, screenAspect;
	fieldOfView = DirectX::XM_PI / 4.0f;
	screenAspect = 800.0f / 600.0f; // width/height
	projectionMatrix = DirectX::XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, 0.1f, 1000.0f);

	//Init world matrix to identity matrix
	worldMatrix = DirectX::XMMatrixIdentity();

	//Create orthographic projection for 2d rendering
	orthoMatrix = DirectX::XMMatrixOrthographicLH(800.0f, 600.0f, 0.1f, 1000.0f);

	//Create alpha enabled blend state
	D3D11_BLEND_DESC bd;
	ZeroMemory(&bd, sizeof(D3D11_BLEND_DESC));
	bd.RenderTarget[0].BlendEnable = TRUE;
	bd.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	bd.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	bd.RenderTarget[0].RenderTargetWriteMask = 0x0f;
	pDevice->CreateBlendState(&bd, &pBlendingStateEnable);

	//Modify desc to create disabled blend state
	bd.RenderTarget[0].BlendEnable = FALSE;
	pDevice->CreateBlendState(&bd, &pBlendingStateDisable);

	ImGui_ImplDX11_Init(pDevice.Get(), pContext.Get());
}

d3dApp::~d3dApp()
{
	ImGui_ImplDX11_Shutdown();
}

void d3dApp::EndFrame()
{
	if (imguiEnabled)
	{
		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	}
	pSwapChain->Present(1u, 0u);
}

void d3dApp::BeginFrame(float red, float green, float blue)
{
	if (imguiEnabled)
	{
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
	}

	const float color[] = { red, green, blue, 1.0f };
	pContext->ClearRenderTargetView(pTargetView.Get(), color);
	pContext->ClearDepthStencilView(pDepthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0u);
}

void d3dApp::DrawIndexed(UINT count)
{
	pContext->DrawIndexed(count, 0u, 0u);
}

void d3dApp::SetProjectionMatrix(DirectX::FXMMATRIX proj) noexcept
{
	projectionMatrix = proj;
}

DirectX::XMMATRIX d3dApp::GetProjectionMatrix() const noexcept
{
	return projectionMatrix;
}

void d3dApp::SetWorldMatrix(DirectX::FXMMATRIX world) noexcept
{
	worldMatrix = world;
}

DirectX::XMMATRIX d3dApp::GetWorldMatrix() const noexcept
{
	return worldMatrix;
}

void d3dApp::SetOrthoMatrix(DirectX::FXMMATRIX ortho) noexcept
{
	orthoMatrix = ortho;
}

DirectX::XMMATRIX d3dApp::GetOrthoMatrix() const noexcept
{
	return orthoMatrix;
}

ID3D11Device* d3dApp::GetDevice()
{
	return pDevice.Get();
}

ID3D11DeviceContext* d3dApp::GetContext()
{
	return pContext.Get();
}

void d3dApp::EnableAlphaBlending()
{
	float blendFactor[] = {0.0f, 0.0f, 0.0f, 0.0f};
	pContext->OMSetBlendState(pBlendingStateEnable.Get(), blendFactor, 0xffffffff);
}

void d3dApp::DisableAlphaBlending()
{
	float blendFactor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	pContext->OMSetBlendState(pBlendingStateDisable.Get(), blendFactor, 0xffffffff);
}

void d3dApp::EnableImgui()
{
	imguiEnabled = true;
}

void d3dApp::DisableImgui()
{
	imguiEnabled = false;
}

bool d3dApp::IsImguiEnabled() const
{
	return imguiEnabled;
}
