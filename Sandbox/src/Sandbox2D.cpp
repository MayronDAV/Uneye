#include "Sandbox2D.h"

#include <imgui/imgui.h>



void Sandbox2D::OnAttach()
{

	Uneye::Application::Get().GetWindow().SetVSync(false);
}

void Sandbox2D::OnDetach()
{

}

void Sandbox2D::OnUpdate(Uneye::Timestep ts)
{
	m_CameraController.OnUpdate(ts);

	Uneye::RenderCommand::Clear({ 0.1f, 0.1f, 0.13f, 1.0f });

	Uneye::Renderer2D::BeginScene(m_CameraController.GetCamera());
	{

		for (float x = 0.0f; x <= 5.0f; x += 1.0f)
		{
			for (float y = 0.0f; y <= 5.0f; y += 1.0f)
			{
				Uneye::Renderer2D::DrawQuad({ x * 0.11f, y * 0.11f }, { 0.1f, 0.1f }, { 0.8f, 0.3f, 0.2f, 1.0f });
			}
		}

		Uneye::Renderer2D::DrawQuad(
			glm::vec3(m_CameraController.GetPosition().x, m_CameraController.GetPosition().y, 0.01f),
			{ 0.1f, 0.1f }, 
			Uneye::Texture2D::Create("assets/textures/container.jpg"),
			m_SquareColor);

	}
	Uneye::Renderer2D::EndScene();
}

void Sandbox2D::OnEvent(Uneye::Event& e)
{
	m_CameraController.OnEvent(e);
}

void Sandbox2D::OnImGuiRender()
{
	ImGui::Begin("Quads Setup");
	ImGui::ColorEdit4("SquareColor", glm::value_ptr(m_SquareColor));
	ImGui::End();
}
