#pragma once

#include <Uneye.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>



class Example1 : public Uneye::Layer
{
public:
	Example1() :Layer("Sandbox2D"), m_EditorCamera(90.0f, 800.0f / 600.0f, 0.01f, 1000.0f) {}
	virtual ~Example1() = default;

	virtual void OnAttach() override;
	virtual void OnDetach() override;

	virtual void OnUpdate(Uneye::Timestep ts) override;
	virtual void OnEvent(Uneye::Event& e) override;
	virtual void OnImGuiRender() override;

private:
	Uneye::EditorCamera m_EditorCamera;

	// Temp
	Uneye::Ref<Uneye::Shader> m_Shader;
	Uneye::Ref<Uneye::VertexArray> m_VertexArray;
	Uneye::Ref<Uneye::Texture2D> m_Texture;

	Uneye::Ref<Uneye::Texture2D> m_SpriteSheet;


	glm::vec4 m_SquareColor{ 0.2f, 0.3f, 0.8f, 1.0f };

	std::unordered_map<char, Uneye::Ref<Uneye::SubTexture2D>> m_Map;
};
