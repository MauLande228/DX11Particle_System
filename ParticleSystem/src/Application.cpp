#include "Application.h"
#include "../imgui/imgui.h"

Application::Application()
	: wnd(800, 600, L"Particle System")
{
	m_Camera = std::make_unique<Camera>();
	m_Camera->SetPosition(0.0f, -2.5f, -10.0f);

	m_ParticleShader = std::make_unique<PSysShader>(wnd.Gfx());

	m_ParticleSystem = std::make_unique<ParticleSystem>(wnd.Gfx(), L"data/star.dds");
}

Application::~Application()
{
}

int Application::Go()
{
	while (true)
	{
		if (const auto ecode = Window::ProcessMessages())
		{
			return *ecode;
		}
		timer.Frame();
		DoFrame();
	}
}

void Application::DoFrame()
{
	DirectX::XMMATRIX worldMatrix;
	DirectX::XMMATRIX viewMatrix;
	DirectX::XMMATRIX projectionMatrix;
	
	m_ParticleSystem->Update(timer.GetTime(), wnd.Gfx());

	wnd.Gfx().BeginFrame(0.0f, 0.0f, 0.0f);

	m_Camera->Render();
	viewMatrix = m_Camera->GetViewMatrix();
	worldMatrix = wnd.Gfx().GetWorldMatrix();
	projectionMatrix = wnd.Gfx().GetProjectionMatrix();

	wnd.Gfx().EnableAlphaBlending();

	//
	
	ImGui::Begin("Particles Props");
	ImGui::Text("Set some properties of the particle system.");
	ImGui::ColorEdit3("P.S. Color", (float*)&particlesColor);
	ImGui::End();

	m_ParticleSystem->red = particlesColor.x;
	m_ParticleSystem->green = particlesColor.y;
	m_ParticleSystem->blue = particlesColor.z;
	//

	m_ParticleSystem->Render(wnd.Gfx());
	m_ParticleShader->Render(wnd.Gfx(), m_ParticleSystem->GetIndexCount(), worldMatrix,
		viewMatrix, projectionMatrix, m_ParticleSystem->GetTexture());

	wnd.Gfx().DisableAlphaBlending();

	wnd.Gfx().EndFrame();
}
