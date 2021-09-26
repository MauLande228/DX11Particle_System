#include "Texture.h"

Texture::Texture(d3dApp& gfx, const wchar_t* fileName)
{
	HRESULT res;
	res = DirectX::CreateDDSTextureFromFile(gfx.GetDevice(), fileName, nullptr, pTextureView.GetAddressOf());
	if (FAILED(res))
	{
		exit(1);
	}
}

Texture::Texture(const Texture& texture)
{
}

Texture::~Texture()
{
}

void Texture::ShutDown()
{
}

ID3D11ShaderResourceView* Texture::GetTexture()
{
	return pTextureView.Get();
}