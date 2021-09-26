#pragma once

#include "d3dApp.h"
#include "Texture.h"
#include <fstream>

class PSysShader
{
private:
	struct MatrixBuffer
	{
		DirectX::XMMATRIX world;
		DirectX::XMMATRIX view;
		DirectX::XMMATRIX projection;
	};

public:
	PSysShader(d3dApp& gfx);
	PSysShader(const PSysShader&) = delete;
	~PSysShader();

	void Render(d3dApp& gfx, int indexCount, DirectX::XMMATRIX worldMatrix, DirectX::XMMATRIX viewMatrix, 
		DirectX::XMMATRIX projectionMatrix, ID3D11ShaderResourceView* srView);

private:
	void InitShader(d3dApp& gfx, const wchar_t* vsFileName, const wchar_t* psFileName);

	void SetShaderParameters(d3dApp& gfx, DirectX::XMMATRIX worldMatrix, DirectX::XMMATRIX viewMatrix, 
		DirectX::XMMATRIX projectionMatrix, ID3D11ShaderResourceView* srView);
	void RenderShader(d3dApp& gfx, int indexCount);

private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader> pVertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pPixelShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> pInputLayout;
	Microsoft::WRL::ComPtr<ID3D11Buffer> pMatrixBuffer;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> pSampleState;
};

