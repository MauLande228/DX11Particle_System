#pragma once

#include "d3dApp.h"
#include <DDSTextureLoader.h>
#include <iostream>
#include <fstream>
#include <memory>

class Texture
{
public:
	Texture(d3dApp& gfx, const wchar_t* fileName);
	Texture(const Texture& texture);
	~Texture();

	ID3D11ShaderResourceView* GetTexture();

private:
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pTextureView;
};

