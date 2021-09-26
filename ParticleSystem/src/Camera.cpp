#include "Camera.h"

Camera::Camera()
	:
	xPos(0.0f), yPos(0.0f), zPos(0.0f),
	rotX(0.0f), rotY(0.0f), rotZ(0.0f)
{
}

Camera::~Camera()
{
}

void Camera::SetPosition(float xPos, float yPos, float zPos)
{
	this->xPos = xPos;
	this->yPos = yPos;
	this->zPos = zPos;
}

void Camera::SetRotation(float rotX, float rotY, float rotZ)
{
	this->rotX = rotX;
	this->rotY = rotY;
	this->rotZ = rotZ;
}

DirectX::XMFLOAT3 Camera::GetPosition()
{
	return DirectX::XMFLOAT3(xPos, yPos, zPos);
}

DirectX::XMFLOAT3 Camera::GetRotation()
{
	return DirectX::XMFLOAT3(rotX, rotY, rotZ);
}

void Camera::Render()
{
	DirectX::XMFLOAT3 up, position, lookAt;
	DirectX::XMMATRIX rotationMatrix;
	float yaw, pitch, roll;

	//Vector that points upwards
	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;

	//Position of camera in the world
	position.x = xPos;
	position.y = yPos;
	position.z = zPos;

	//Where camera is looking by default
	lookAt.x = 0.0f;
	lookAt.y = 0.0f;
	lookAt.z = 1.0f;

	//Set pitch(x axis) yaw(y axis) roll(z axis). Rotation in rads
	pitch = rotX * 0.0174532925f;
	yaw = rotY * 0.0174532925f;
	roll = rotZ * 0.0174532925f;

	//Create rotation matrix
	rotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

	//Transform looAt and up vector by the rotation matrix so the view is correctly rotated at the origin
	DirectX::XMVECTOR lookAtVec = DirectX::XMLoadFloat3(&lookAt);
	DirectX::XMVECTOR upVec = DirectX::XMLoadFloat3(&up);
	lookAtVec = DirectX::XMVector3TransformCoord(lookAtVec, rotationMatrix);
	upVec = DirectX::XMVector3TransformCoord(upVec, rotationMatrix);

	//Translate the rotated camera position to the location of the viewer
	DirectX::XMVECTOR positionVec = DirectX::XMLoadFloat3(&position);
	lookAtVec = DirectX::XMVectorAdd(positionVec, lookAtVec);

	//Create the view matrix
	viewMatrix = DirectX::XMMatrixLookAtLH(positionVec, lookAtVec, upVec);
}

DirectX::XMMATRIX Camera::GetViewMatrix()
{
	return viewMatrix;
}
