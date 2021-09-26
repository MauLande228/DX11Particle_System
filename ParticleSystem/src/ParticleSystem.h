#pragma once
#include <algorithm>
#include "d3dApp.h"
#include "Texture.h"
#include "Random.h"

class ParticleSystem
{
public:
	struct Particle
	{
		float posX;
		float posY;
		float posZ;
		float red, green, blue;
		float speed;
		bool active = false;
	};

	struct Vertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT2 texture;
		DirectX::XMFLOAT4 color;
	};

	float red, green, blue;

public:
	ParticleSystem(d3dApp& gfx, const wchar_t* textureFile);
	~ParticleSystem();

	void Update(float dt, d3dApp& gfx);
	void Render(d3dApp& gfx);

	int GetIndexCount();

	ID3D11ShaderResourceView* GetTexture();

private:
	void LoadTexture(d3dApp& gfx, const wchar_t* fileName);
	void ReleaseTexture();

	void InitParticleSystem();
	void ShutDownParticleSystem();

	void InitBuffers(d3dApp& gfx);

	void Emit(float dt);
	void UpdateParticles(float);
	void KillParticles();

	void UpdateBuffers(d3dApp& gfx);

	void RenderBuffers(d3dApp& gfx);

	

private:
	float deviationX, deviationY, deviationZ;
	float speed, speedVariation;
	float size;
	float particlesPerSecond;
	uint32_t maxParticles;
	int currentParticleCount;
	float accumulatedTime;

	std::unique_ptr<Texture> texture;
	std::vector<Particle> particlePool;

	int vertexCount, indexCount;
	std::unique_ptr<Vertex[]> vertices;
	Microsoft::WRL::ComPtr<ID3D11Buffer> pVertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> pIndexBuffer;
};