#pragma once

#include "Uneye/Renderer/Camera.h"
#include "Uneye/Core/Timestep.h"

#include "Uneye/Events/Event.h"
#include "Uneye/Events/MouseEvent.h"
#include "Uneye/Events/ApplicationEvent.h"



namespace Uneye
{
	struct OrthographicCameraBounds
	{
		float Left, Right;
		float Bottom, Top;

		float GetWidth() { return Right - Left; }
		float GetHeight() { return Top - Bottom; }
	};

	class OrthographicCameraController
	{
		public:
			OrthographicCameraController() {}
			OrthographicCameraController(float aspectratio, bool rotation = false);

			void OnUpdate(Timestep ts);
			void OnEvent(Event& e);

			const glm::vec3& GetPosition() const		 { return m_CameraPosition; }
			const glm::vec3& GetDirection() const		 { return m_Direction; }
			const glm::vec3& GetSpeed() const			 { return m_Speed; }
			float GetZoomLevel()						 { return m_ZoomLevel; }
			bool  GetHaveRotation()						 { return m_Rotation; }
			float GetRotation()							 { return m_CameraRotation; }
			float GetRotationSpeed()					 { return m_CameraRotationSpeed; }

			OrthographicCamera& GetCamera()				 { return m_Camera; }
			const OrthographicCamera& GetCamera() const  { return m_Camera; }

			void SetZoomLevel(float level)				 { m_ZoomLevel = level; }
			void SetCameraSpeed(const glm::vec3& speed)	 { m_Speed = speed; }
			void SetCameraPosition(const glm::vec3& pos) { m_CameraPosition = pos; }
			void SetHaveRotation(bool rotation)			 { m_Rotation = rotation; }
			void SetRotation(float rotation)			 { m_CameraRotation = rotation; }
			void SetRotationSpeed(float speed)			 { m_CameraRotationSpeed = speed; }

			const OrthographicCameraBounds& GetBounds() const { return m_Bounds; }
		private:
			bool OnMouseScrolled(MouseScrolledEvent& e);
			bool OWindowResize(WindowResizeEvent& e);

		private:
			float m_AspectRatio{ 0.0f };
			float m_ZoomLevel = 1.0f;
			bool m_Rotation = false;

			OrthographicCameraBounds m_Bounds;
			OrthographicCamera m_Camera;

			glm::vec3 m_CameraPosition{ 0.0f };
			glm::vec3 m_Direction{ 0.0f };
			glm::vec3 m_Speed{ 1.0f };	
			float m_CameraRotation{ 0.0f };
			float m_CameraRotationSpeed{ 180.0f };
	};




};