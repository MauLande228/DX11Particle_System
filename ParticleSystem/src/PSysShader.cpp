#include "PSysShader.h"

PSysShader::PSysShader(d3dApp& gfx)
{
	InitShader(gfx, L"Particle.cso", L"ParticlePS.cso");
}

PSysShader::~PSysShader()
{
}

void PSysShader::Render(d3dApp& gfx, int indexCount, DirectX::XMMATRIX worldMatrix, 
	DirectX::XMMATRIX viewMatrix, DirectX::XMMATRIX projectionMatrix, ID3D11ShaderResourceView* srView)
{
	SetShaderParameters(gfx, worldMatrix, viewMatrix, projectionMatrix, srView);
	RenderShader(gfx, indexCount);
}

void PSysShader::InitShader(d3dApp& gfx, const wchar_t* vsFileName, const wchar_t* psFileName)
{
	Microsoft::WRL::ComPtr<ID3DBlob> vertexShaderBuffer;
	D3DReadFileToBlob(vsFileName, &vertexShaderBuffer);
	gfx.GetDevice()->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), nullptr, &pVertexShader);

	Microsoft::WRL::ComPtr<ID3DBlob> pixelShaderBuffer;
	D3DReadFileToBlob(psFileName, &pixelShaderBuffer);
	gfx.GetDevice()->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), nullptr, &pPixelShader);

	D3D11_INPUT_ELEMENT_DESC polygonLayout[] = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	unsigned int numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);
	gfx.GetDevice()->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &pInputLayout);

	D3D11_BUFFER_DESC matrixBufferDesc;
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBuffer);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;
	gfx.GetDevice()->CreateBuffer(&matrixBufferDesc, nullptr, &pMatrixBuffer);

	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	gfx.GetDevice()->CreateSamplerState(&samplerDesc, &pSampleState);
}

void PSysShader::SetShaderParameters(d3dApp& gfx, DirectX::XMMATRIX worldMatrix,
	DirectX::XMMATRIX viewMatrix, DirectX::XMMATRIX projectionMatrix, ID3D11ShaderResourceView* srView)
{
	MatrixBuffer* dataPtr;
	unsigned int bufferNumber;

	worldMatrix = DirectX::XMMatrixTranspose(worldMatrix);
	viewMatrix = DirectX::XMMatrixTranspose(viewMatrix);
	projectionMatrix = DirectX::XMMatrixTranspose(projectionMatrix);

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	gfx.GetContext()->Map(pMatrixBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	dataPtr = (MatrixBuffer*)mappedResource.pData;
	dataPtr->world = worldMatrix;
	dataPtr->view = viewMatrix;
	dataPtr->projection = projectionMatrix;

	gfx.GetContext()->Unmap(pMatrixBuffer.Get(), 0);

	bufferNumber = 0;
	gfx.GetContext()->VSSetConstantBuffers(bufferNumber, 1, pMatrixBuffer.GetAddressOf());
	gfx.GetContext()->PSSetShaderResources(0, 1, &srView);
}

void PSysShader::RenderShader(d3dApp& gfx, int indexCount)
{
	gfx.GetContext()->IASetInputLayout(pInputLayout.Get());
	gfx.GetContext()->VSSetShader(pVertexShader.Get(), nullptr, 0);
	gfx.GetContext()->PSSetShader(pPixelShader.Get(), nullptr, 0);
	gfx.GetContext()->PSSetSamplers(0, 1, pSampleState.GetAddressOf());
	gfx.GetContext()->DrawIndexed(indexCount, 0, 0);
}
