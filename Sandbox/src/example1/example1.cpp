#include "example1.h"

#include <imgui/imgui.h>


static const int s_MapWidth = 20;
static const int s_MapHeight = 20;
static const char* s_MapTile =
"00000000000000000000"
"00000001111110000000"
"00000111111111100000"
"00011111111111111100"
"00000111111111111100"
"00000101111010011000"
"00010001111100011100"
"001111111.1100000000"
"01111111111111100000"
"00101000011111111100"
"00000000000111110000"
"00000000001101100000"
"00000000001100000000"
"00000000111110000000"
"00000011111000000000"
"00000000000000000000"
"00000000000000000000"
"00000000111110000000"
"00000000000000000000"
"00000000000000000000"
"00000000000000000000";






void Example1::OnAttach()
{
	UNEYE_PROFILE_FUNCTION();

	Uneye::Application::Get().GetWindow().SetVSync(false);

	m_SpriteSheet = Uneye::Texture2D::Create("assets/game/RPGpack_sheet_2X.png");

	m_Map['0'] = Uneye::SubTexture2D::CreateFromTexture(m_SpriteSheet, { 11, 11 }, { 128, 128 });
	m_Map['1'] = Uneye::SubTexture2D::CreateFromTexture(m_SpriteSheet, { 6, 11 }, { 128, 128 });

}

void Example1::OnDetach()
{
	UNEYE_PROFILE_FUNCTION();
}

void Example1::OnUpdate(Uneye::Timestep ts)
{
	UNEYE_PROFILE_FUNCTION();


	m_EditorCamera.OnUpdate(ts);

	Uneye::Renderer2D::ResetStats();

	Uneye::RenderCommand::Clear({ 0.1f, 0.1f, 0.13f, 1.0f });

	{
		UNEYE_PROFILE_SCOPE("Renderer Draw");


		Uneye::Renderer2D::BeginScene(m_EditorCamera);

		for (int y = 0; y < s_MapHeight; y++)
		{
			
			for (int x = 0; x < s_MapWidth; x++)
			{
				Uneye::Ref<Uneye::SubTexture2D> texture;
				if (m_Map.find(s_MapTile[x + y * s_MapWidth]) != m_Map.end())
				{
					texture = m_Map[s_MapTile[x + y * s_MapWidth]];
				}
				else
				{
					texture = Uneye::SubTexture2D::CreateFromTexture(m_SpriteSheet, { 11, 11 }, { 128, 128 });
				}

				Uneye::Renderer2D::DrawQuad(
					glm::vec3(x - s_MapWidth / 2, s_MapHeight / 2 - y, 0.0f),
					{ 1.0f, 1.0f },
					texture
				);
			}
		}

		Uneye::Renderer2D::EndScene();

	}
}

void Example1::OnEvent(Uneye::Event& e)
{
	m_EditorCamera.OnEvent(e);
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
