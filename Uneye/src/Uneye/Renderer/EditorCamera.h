#pragma once

#include "Uneye/Renderer/Camera.h"
#include "Uneye/Core/Timestep.h"
#include "Uneye/Events/Event.h"
#include "Uneye/Events/MouseEvent.h"

#include <glm/glm.hpp>



namespace Uneye {

	class EditorCamera : public Camera
	{
		public:
			EditorCamera() = default;
			EditorCamera(float p_fov, float p_aspectRatio, float p_nearClip, float p_farClip);

			void OnUpdate(Timestep p_ts);
			void OnEvent(Event& p_e);

			inline float GetDistance() const { return m_Distance; }
			inline void SetDistance(float p_distance) { m_Distance = p_distance; }

			inline void SetViewportSize(float p_width, float p_height) 
			{ 
				if ((p_width > 0 && p_height > 0) && (m_ViewportWidth != p_width && m_ViewportHeight != p_height))
					m_ViewportWidth = p_width; m_ViewportHeight = p_height; UpdateProjection();
			}

			const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
			glm::mat4 GetViewProjection() const { return m_Projection * m_ViewMatrix; }

			glm::vec3 GetUpDirection() const;
			glm::vec3 GetRightDirection() const;
			glm::vec3 GetForwardDirection() const;
			const glm::vec3& GetPosition() const { return m_Position; }
			glm::quat GetOrientation() const;

			float GetPitch() const { return m_Pitch; }
			float GetYaw() const { return m_Yaw; }

		private:
			void UpdateProjection();
			void UpdateView();

			bool OnMouseScroll(MouseScrolledEvent& e);

			void MousePan(const glm::vec2& delta);
			void MouseRotate(const glm::vec2& delta);
			void MouseZoom(float delta);

			glm::vec3 CalculatePosition() const;

			std::pair<float, float> PanSpeed() const;
			float RotationSpeed() const;
			float ZoomSpeed() const;

		private:
			float m_FOV = 45.0f, m_AspectRatio = 1.778f, m_NearClip = 0.1f, m_FarClip = 1000.0f;

			glm::mat4 m_ViewMatrix{ 1.0f };
			glm::vec3 m_Position = { 0.0f, 0.0f, 0.0f };
			glm::vec3 m_FocalPoint = { 0.0f, 0.0f, 0.0f };

			glm::vec2 m_InitialMousePosition = { 0.0f, 0.0f };

			float m_Distance = 10.0f;
			float m_Pitch = 0.0f, m_Yaw = 0.0f;

			float m_ViewportWidth = 1280, m_ViewportHeight = 720;
	};

}

