#include "ParticleSystem.h"
#include "../imgui/imgui.h"

ParticleSystem::ParticleSystem(d3dApp& gfx, const wchar_t* textureFile)
{
	LoadTexture(gfx, textureFile);
	InitParticleSystem();
	InitBuffers(gfx);
}

ParticleSystem::~ParticleSystem()
{
}

void ParticleSystem::Update(float dt, d3dApp& gfx)
{
	KillParticles();
	Emit(dt);
	UpdateParticles(dt);
	UpdateBuffers(gfx);
}

void ParticleSystem::Render(d3dApp& gfx)
{
	RenderBuffers(gfx);
}

int ParticleSystem::GetIndexCount()
{
	return indexCount;
}

ID3D11ShaderResourceView* ParticleSystem::GetTexture()
{
	return texture->GetTexture();
}

void ParticleSystem::LoadTexture(d3dApp& gfx, const wchar_t* fileName)
{
	texture = std::make_unique<Texture>(gfx, fileName);
}

void ParticleSystem::ReleaseTexture()
{
}

void ParticleSystem::InitParticleSystem()
{
	deviationX = 2.0f;
	deviationY = 0.1f;
	deviationZ = 3.5f;

	speed = 0.2f;
	speedVariation = 0.1f;

	size = 0.06f;

	particlesPerSecond = 600;
	maxParticles = 5000;
	particlePool.resize(maxParticles);
	
	currentParticleCount = 0;
	accumulatedTime = 0.0f;
}

void ParticleSystem::ShutDownParticleSystem()
{
	particlePool.clear();
}

void ParticleSystem::InitBuffers(d3dApp& gfx)
{
	std::unique_ptr<unsigned long[]> indices;

	vertexCount = maxParticles * 6;
	indexCount = vertexCount;
	vertices = std::make_unique<Vertex[]>(vertexCount);
	indices = std::make_unique<unsigned long[]>(indexCount);

	memset(vertices.get(), 0, (sizeof(Vertex) * vertexCount));
	for (int i = 0; i < indexCount; i++)
	{
		indices[i] = i;
	}

	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(Vertex) * vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = vertices.get();
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;
	gfx.GetDevice()->CreateBuffer(&vertexBufferDesc, &vertexData, &pVertexBuffer);

	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA indexData;
	indexData.pSysMem = indices.get();
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;
	gfx.GetDevice()->CreateBuffer(&indexBufferDesc, &indexData, &pIndexBuffer);
}

bool CompareByPosZ(const ParticleSystem::Particle& p1, const ParticleSystem::Particle& p2)
{
	return p1.posZ < p2.posZ;
}

void ParticleSystem::Emit(float dt)
{
	bool emitParticle, found;
	float posX, posY, posZ;
	float speed;
	int index;

	ImVec4 particleColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

	accumulatedTime += dt;
	emitParticle = false;

	if (accumulatedTime > (1000.0f / particlesPerSecond))
	{
		accumulatedTime = 0.0f;
		emitParticle = true;
	}

	if ((emitParticle == true) && (currentParticleCount < (maxParticles - 1)))
	{
		currentParticleCount++;

		posX = ((Random::GenFloat() - Random::GenFloat())) * deviationX;
		posY = ((Random::GenFloat() - Random::GenFloat()))* deviationY;
		posZ = ((Random::GenFloat() - Random::GenFloat())) * deviationZ;

		speed = this->speed + (((float)rand() - (float)rand()) / RAND_MAX) * speedVariation;

		index = 0;
		found = false;

		particlePool[currentParticleCount].posX = posX;
		particlePool[currentParticleCount].posY = posY;
		particlePool[currentParticleCount].posZ = posZ;
		particlePool[currentParticleCount].red = red;
		particlePool[currentParticleCount].green = green;
		particlePool[currentParticleCount].blue = blue;
		particlePool[currentParticleCount].speed = speed;
		particlePool[currentParticleCount].active = true;

		std::sort(particlePool.begin(), particlePool.begin() + currentParticleCount, CompareByPosZ);
	}
}

void ParticleSystem::UpdateParticles(float dt)
{
	DirectX::XMMATRIX particleTrayectory;
	for (int i = 0; i < currentParticleCount; i++)
	{
		particlePool[i].posY = particlePool[i].posY - (particlePool[i].speed * dt * 0.001f);
	}
}

void ParticleSystem::KillParticles()
{
	int currentParticlesBackup = currentParticleCount;
	for (int i = 0; i <= currentParticlesBackup; i++)
	{
		if ((particlePool[i].active == true) && (particlePool[i].posY < -4.5f))
		{
			particlePool[i].active = false;
			currentParticleCount--;
			particlePool.erase(particlePool.begin() + i);
			particlePool.push_back(Particle{});
		}
	}
}

void ParticleSystem::UpdateBuffers(d3dApp& gfx)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	std::unique_ptr<Vertex> verticesPtr;

	memset(vertices.get(), 0, (sizeof(Vertex) * vertexCount));

	int index = 0;
	for (int i = 0; i < currentParticleCount; i++)
	{
		//Bottom left
		vertices[index].position = DirectX::XMFLOAT3(particlePool[i].posX - size, particlePool[i].posY - size, particlePool[i].posZ);
		vertices[index].texture = DirectX::XMFLOAT2(0.0f, 1.0f);
		vertices[index].color = DirectX::XMFLOAT4(particlePool[i].red, particlePool[i].green, particlePool[i].blue, 1.0f);
		index++;

		//Top left
		vertices[index].position = DirectX::XMFLOAT3(particlePool[i].posX - size, particlePool[i].posY + size, particlePool[i].posZ);
		vertices[index].texture = DirectX::XMFLOAT2(0.0f, 0.0f);
		vertices[index].color = DirectX::XMFLOAT4(particlePool[i].red, particlePool[i].green, particlePool[i].blue, 1.0f);
		index++;

		//Bottom right
		vertices[index].position = DirectX::XMFLOAT3(particlePool[i].posX + size, particlePool[i].posY - size, particlePool[i].posZ);
		vertices[index].texture = DirectX::XMFLOAT2(1.0f, 1.0f);
		vertices[index].color = DirectX::XMFLOAT4(particlePool[i].red, particlePool[i].green, particlePool[i].blue, 1.0f);
		index++;

		//Bottom right
		vertices[index].position = DirectX::XMFLOAT3(particlePool[i].posX + size, particlePool[i].posY - size, particlePool[i].posZ);
		vertices[index].texture = DirectX::XMFLOAT2(1.0f, 1.0f);
		vertices[index].color = DirectX::XMFLOAT4(particlePool[i].red, particlePool[i].green, particlePool[i].blue, 1.0f);
		index++;

		//Top left
		vertices[index].position = DirectX::XMFLOAT3(particlePool[i].posX - size, particlePool[i].posY + size, particlePool[i].posZ);
		vertices[index].texture = DirectX::XMFLOAT2(0.0f, 0.0f);
		vertices[index].color = DirectX::XMFLOAT4(particlePool[i].red, particlePool[i].green, particlePool[i].blue, 1.0f);
		index++;

		//Top right
		vertices[index].position = DirectX::XMFLOAT3(particlePool[i].posX + size, particlePool[i].posY + size, particlePool[i].posZ);
		vertices[index].texture = DirectX::XMFLOAT2(1.0f, 0.0f);
		vertices[index].color = DirectX::XMFLOAT4(particlePool[i].red, particlePool[i].green, particlePool[i].blue, 1.0f);
		index++;
	}

	gfx.GetContext()->Map(pVertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	memcpy(mappedResource.pData, vertices.get(), (sizeof(Vertex) * vertexCount));
	gfx.GetContext()->Unmap(pVertexBuffer.Get(), 0);
}

void ParticleSystem::RenderBuffers(d3dApp& gfx)
{
	unsigned int stride;
	unsigned int offset;

	stride = sizeof(Vertex);
	offset = 0;

	gfx.GetContext()->IASetVertexBuffers(0, 1, pVertexBuffer.GetAddressOf(), &stride, &offset);
	gfx.GetContext()->IASetIndexBuffer(pIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	gfx.GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}
