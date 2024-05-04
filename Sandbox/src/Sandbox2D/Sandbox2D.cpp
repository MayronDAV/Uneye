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

	Uneye::Renderer2D::ResetStats();

	Uneye::RenderCommand::Clear({ 0.1f, 0.1f, 0.13f, 1.0f });

	{
		UNEYE_PROFILE_SCOPE("Renderer Draw");
		Uneye::Renderer2D::BeginScene(m_CameraController.GetCamera());


		float Mapsize = 10;
		for (float x = 0.0f; x <= Mapsize; x += 1.0f)
		{
			for (float y = 0.0f; y <= Mapsize; y += 1.0f)
			{
				Uneye::Renderer2D::DrawQuad({ x * 0.11f, y * 0.11f, 0.01f },
					{ 0.1f, 0.1f }, { 0.8f, 0.3f, 0.2f, 1.0f });
			}
		}

		Uneye::Renderer2D::DrawQuad(
			glm::vec3(-1.0f, 0.0f, 0.1f),
			{ 0.1f, 0.1f },
			m_SquareColor,
			Uneye::Texture2D::Create("assets/textures/container.jpg")
		);

		Uneye::Renderer2D::EndScene();

		Uneye::Renderer2D::BeginScene(m_CameraController.GetCamera());

		for (float x = 0.0f; x <= Mapsize; x += 1.0f)
		{
			for (float y = 0.0f; y <= Mapsize; y += 1.0f)
			{
				glm::vec4 color = { (x + 5.0f) / 10.0f, 0.4f, (y + 5.0f) / 10.0f, 0.5f };
				Uneye::Renderer2D::DrawQuad({ x * 0.11f, y * 0.11f, 0.1f },
					{ 0.1f, 0.1f }, color);
			}
		}

		Uneye::Renderer2D::DrawQuad(
			glm::vec3(-3.0f, 0.0f, 0.1f),
			{ 0.1f, 0.1f },
			m_SquareColor,
			Uneye::Texture2D::Create("assets/textures/container.jpg")
		);

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

	auto stats = Uneye::Renderer2D::GetStats();
	ImGui::Text("Renderer2D Stats: ");
	ImGui::Text("Draw Calls:	%d", stats.DrawCalls);
	ImGui::Text("Quad Count:	%d", stats.QuadCount);
	ImGui::Text("Vertices:		%d", stats.GetTotalVertexCount());
	ImGui::Text("Indices:		%d", stats.GetTotalIndexCount());

	ImGui::ColorEdit4("SquareColor", glm::value_ptr(m_SquareColor));
	ImGui::End();
}
