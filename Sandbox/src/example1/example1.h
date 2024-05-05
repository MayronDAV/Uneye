#pragma once

#include <Uneye.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "ParticleSystem/ParticleSystem.h"


class Example1 : public Uneye::Layer
{
public:
	Example1() :Layer("Sandbox2D"), m_CameraController(800.0f / 600.0f) {}
	virtual ~Example1() = default;

	virtual void OnAttach() override;
	virtual void OnDetach() override;

	virtual void OnUpdate(Uneye::Timestep ts) override;
	virtual void OnEvent(Uneye::Event& e) override;
	virtual void OnImGuiRender() override;

private:
	Uneye::OrthographicCameraController m_CameraController;

	// Temp
	Uneye::Ref<Uneye::Shader> m_Shader;
	Uneye::Ref<Uneye::VertexArray> m_VertexArray;
	Uneye::Ref<Uneye::Texture2D> m_Texture;

	Uneye::Ref<Uneye::Texture2D> m_SpriteSheet;


	glm::vec4 m_SquareColor{ 0.2f, 0.3f, 0.8f, 1.0f };

	ParticleSystem m_ParticleSystem;
	ParticleProps m_Particle;

	std::unordered_map<char, Uneye::Ref<Uneye::SubTexture2D>> m_Map;
};