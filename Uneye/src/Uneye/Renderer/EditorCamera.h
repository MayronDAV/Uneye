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

			inline float GetZoom() const { return m_Zoom; }
			inline void SetZoom(float p_zoom) { m_Zoom = std::min(std::max(p_zoom, 0.1f), 1.0f); }

			inline void SetViewportSize(float p_width, float p_height) 
			{ 
				if ((p_width > 0 && p_height > 0) && (m_ViewportWidth != p_width && m_ViewportHeight != p_height))
					m_ViewportWidth = p_width; m_ViewportHeight = p_height; UpdateProjection();
			}

			const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
			glm::mat4 GetViewProjection() const { return m_Projection * m_ViewMatrix; }

			const glm::vec3& GetPosition() const { return m_Position; }

			float GetPitch() const { return m_Pitch; }
			float GetYaw() const { return m_Yaw; }

			void SetFocus(bool p_focus) { m_IsFocused = p_focus; }

			void SetFix2D(bool p_fix);
			bool IsFixed2D() const { return m_IsFixed2D; }

			void SetLimit(bool p_limit) { m_IsLimited = p_limit; }
			void SetLimit(const glm::vec3& p_min, const glm::vec3& p_max) { m_LimitMin = p_min; m_LimitMax = p_max; }
			bool IsLimited() const { return m_IsLimited; }

		private:
			void UpdateProjection();
			void UpdateView();
			void UpdateCameraVectors();

			glm::vec3 CalculatePosition() const;

			void ProcessKeyboard2D(Timestep p_ts);
			void ProcessKeyboard3D(Timestep p_ts);

			void ProcessMouseMovement(const glm::vec2& p_delta, bool p_constrainPitch = true);

			bool OnMouseScroll(MouseScrolledEvent& p_e);
			void MouseZoom(float p_delta);
			float ZoomSpeed() const;

		private:
			bool m_IsFocused = true;
			bool m_IsFixed2D = false;
			bool m_IsLimited = false;

			glm::vec3 m_LimitMin = { 0.0f, 0.0f, 0.0f };
			glm::vec3 m_LimitMax = { 1.0f, 1.0f, 1.0f };

			glm::vec3 m_Position = { 0.0f, 0.0f, 0.0f };
			glm::vec3 m_Front = { 0.0f, 0.0f, -1.0f };
			glm::vec3 m_Right = { 1.0f, 0.0f, 0.0f };
			glm::vec3 m_Up = { 0.0f, 1.0f, 0.0f };
			glm::vec3 m_WorldUp = { 0.0f, 1.0f, 0.0f };
			float m_MovementSpeed = 5.0f;
			float m_MouseSensitivity = 0.1f;

			float m_FOV = 45.0f; 
			float m_AspectRatio = 1.778f;
			float m_NearClip = 0.1f;
			float m_FarClip = 1000.0f;
			float m_Pitch = 0.0f;
			float m_Yaw = 0.0f;

			glm::vec2 m_InitialMousePosition = { 0.0f, 0.0f };
			glm::mat4 m_ViewMatrix{ 1.0f };
			glm::vec3 m_FocalPoint = { 0.0f, 0.0f, 0.0f };

			float m_ViewportWidth = 1280;
			float m_ViewportHeight = 720;

			// 2D
			float m_Distance = 10.0f;

			// 3D
			float m_Zoom = 1.0f;
			float m_NewZoom = m_Zoom;

	};

}

