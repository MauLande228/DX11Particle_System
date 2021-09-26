#pragma once
#include "Window.h"
#include "Timer.h"
#include "Camera.h"
#include "ParticleSystem.h"
#include "PSysShader.h"
#include "ImguiManager.h"

class Application
{
public:
	Application();
	~Application();
	int Go();

private:
	void DoFrame();

	ImguiManager imgui;
	Window wnd;
	Timer timer;
	std::unique_ptr<Camera> m_Camera;
	std::unique_ptr<ParticleSystem> m_ParticleSystem;
	std::unique_ptr<PSysShader> m_ParticleShader;
	ImVec4 particlesColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
};

