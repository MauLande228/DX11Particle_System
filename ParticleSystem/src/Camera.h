#pragma once
#include <DirectXMath.h>

class Camera
{
public:
	Camera();
	Camera(const Camera&) = delete;
	Camera& operator=(const Camera&) = delete;
	~Camera();

	void SetPosition(float xPos, float yPos, float zPos);
	void SetRotation(float rotX, float rotY, float rotZ);

	DirectX::XMFLOAT3 GetPosition();
	DirectX::XMFLOAT3 GetRotation();

	void Render();
	DirectX::XMMATRIX GetViewMatrix();

private:
	DirectX::XMMATRIX viewMatrix;
	float xPos, yPos, zPos;
	float rotX, rotY, rotZ;
};

