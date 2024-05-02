#include "Sandbox2D.h"

#include <imgui/imgui.h>



void Sandbox2D::OnAttach()
{
	UNEYE_PROFILE_FUNCTION();

	//Uneye::Application::Get().GetWindow().SetVSync(false);
}

void Sandbox2D::OnDetach()
{
	UNEYE_PROFILE_FUNCTION();
}

void Sandbox2D::OnUpdate(Uneye::Timestep ts)
{
	UNEYE_PROFILE_FUNCTION();


	m_CameraController.OnUpdate(ts);

	{
		UNEYE_PROFILE_SCOPE("Renderer Prep");
		Uneye::RenderCommand::Clear({ 0.1f, 0.1f, 0.13f, 1.0f });
	}

	{
		UNEYE_PROFILE_SCOPE("Renderer Draw");
		Uneye::Renderer2D::BeginScene(m_CameraController.GetCamera());
		{

			static float Mapsize = 5;
			for (float x = 0.0f; x <= Mapsize; x += 1.0f)
			{
				for (float y = 0.0f; y <= Mapsize; y += 1.0f)
				{
					Uneye::Renderer2D::DrawQuad({ x * 0.11f, y * 0.11f },
						{ 0.1f, 0.1f }, { 0.8f, 0.3f, 0.2f, 1.0f });
				}
			}

			Uneye::Renderer2D::DrawRotateQuad(
				glm::vec3(m_CameraController.GetPosition().x,
					m_CameraController.GetPosition().y, 0.01f),
				{ 0.1f, 0.1f }, glm::radians(45.0f),
				Uneye::Texture2D::Create("assets/textures/container.jpg"),
				m_SquareColor);

		}
		Uneye::Renderer2D::EndScene();
	}
}

void Sandbox2D::OnEvent(Uneye::Event& e)
{
	m_CameraController.OnEvent(e);
}

void Sandbox2D::OnImGuiRender()
{
	UNEYE_PROFILE_FUNCTION();

	ImGui::Begin("Quads Setup");
	ImGui::ColorEdit4("SquareColor", glm::value_ptr(m_SquareColor));
	ImGui::End();
}