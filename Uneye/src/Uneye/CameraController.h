#pragma once

#include "Uneye/Renderer/Camera.h"
#include "Uneye/Core/Timestep.h"

#include "Uneye/Events/Event.h"
#include "Uneye/Events/MouseEvent.h"
#include "Uneye/Events/ApplicationEvent.h"



namespace Uneye
{
	class OrthographicCameraController
	{
		public:
			OrthographicCameraController() {}
			OrthographicCameraController(float aspectratio, bool rotation = false);

			void OnUpdate(Timestep ts);
			void OnEvent(Event& e);

			const glm::vec3& GetPosition() const	{ return m_CameraPosition; }
			const glm::vec3& GetDirection() const	{ return m_Direction; }
			const glm::vec3& GetSpeed() const		{ return m_Speed; }

			OrthographicCamera& GetCamera() { return m_Camera; }
			const OrthographicCamera& GetCamera() const { return m_Camera; }

		private:
			bool OnMouseScrolled(MouseScrolledEvent& e);
			bool OWindowResize(WindowResizeEvent& e);

		private:
			float m_AspectRatio{ 0.0f };
			float m_ZoomLevel = 1.0f;
			bool m_Rotation = false;

			OrthographicCamera m_Camera;

			glm::vec3 m_CameraPosition{ 0.0f };
			glm::vec3 m_Direction{ 0.0f };
			glm::vec3 m_Speed{ 1.0f };	
			float m_CameraRotation{ 0.0f };
			float m_CameraRotationSpeed{ 180.0f };
	};




};