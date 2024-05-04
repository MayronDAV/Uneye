#include "example1.h"

#include <imgui/imgui.h>



void Example1::OnAttach()
{
	UNEYE_PROFILE_FUNCTION();

	Uneye::Application::Get().GetWindow().SetVSync(false);

	m_Particle.ColorBegin = { 254 / 255.0f, 212 / 255.0f, 123 / 255.0f, 1.0f };
	m_Particle.ColorEnd = { 254 / 255.0f, 109 / 255.0f, 41 / 255.0f, 1.0f };
	m_Particle.SizeBegin = 0.5f, m_Particle.SizeVariation = 0.3f, m_Particle.SizeEnd = 0.0f;
	m_Particle.LifeTime = 1.0f;
	m_Particle.Velocity = { 0.0f, 0.0f };
	m_Particle.VelocityVariation = { 3.0f, 1.0f };
	m_Particle.Position = { 0.0f, 0.0f };

	m_SpriteSheet = Uneye::Texture2D::Create("assets/game/RPGpack_sheet_2X.png");
}

void Example1::OnDetach()
{
	UNEYE_PROFILE_FUNCTION();
}

void Example1::OnUpdate(Uneye::Timestep ts)
{
	UNEYE_PROFILE_FUNCTION();


	m_CameraController.OnUpdate(ts);

	Uneye::Renderer2D::ResetStats();

	Uneye::RenderCommand::Clear({ 0.1f, 0.1f, 0.13f, 1.0f });

	{
		UNEYE_PROFILE_SCOPE("Renderer Draw");

		if (Uneye::Input::IsMouseButtonPressed(Uneye::Mouse::Button_Left))
		{
			auto [x, y] = Uneye::Input::GetMousePosition();
			auto width = Uneye::Application::Get().GetWindow().GetWidth();
			auto height = Uneye::Application::Get().GetWindow().GetHeight();

			auto bounds = m_CameraController.GetBounds();
			auto pos = m_CameraController.GetCamera().GetPosition();
			x = (x / width) * bounds.GetWidth() - bounds.GetWidth() * 0.5f;
			y = bounds.GetHeight() * 0.5f - (y / height) * bounds.GetHeight();
			m_Particle.Position = { x + pos.x, y + pos.y };
			for (int i = 0; i < 5; i++)
				m_ParticleSystem.Emit(m_Particle);

		}



		m_ParticleSystem.OnUpdate(ts);
		m_ParticleSystem.OnRender(m_CameraController.GetCamera());


		Uneye::Renderer2D::BeginScene(m_CameraController.GetCamera());

		Uneye::Renderer2D::DrawQuad(
			glm::vec3(0.0f, 0.0f, 0.5f),
			{ 1.0f, 1.0f },
			Uneye::SubTexture2D::CreateFromTexture(m_SpriteSheet, {7, 6}, {128, 128}),
			m_SquareColor
		);

		Uneye::Renderer2D::DrawQuad(
			glm::vec3(-1.0f, -1.0f, 0.5f),
			{ 1.0f, 1.0f },
			Uneye::SubTexture2D::CreateFromTexture(m_SpriteSheet, { 8, 2 }, { 128, 128 })
		);

		Uneye::Renderer2D::DrawQuad(
			glm::vec3(1.5f, 0.0f, 0.5f),
			{ 1.0f, 2.0f },
			Uneye::SubTexture2D::CreateFromTexture(m_SpriteSheet, { 2, 1 }, { 128, 128}, {1, 2})
		);

		Uneye::Renderer2D::EndScene();

	}

}

void Example1::OnEvent(Uneye::Event& e)
{
	m_CameraController.OnEvent(e);
}

void Example1::OnImGuiRender()
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
