#include "uypch.h"
#include "Uneye/Renderer/EditorCamera.h"

#include "Uneye/Core/Input.h"
#include "Uneye/Core/KeyCodes.h"

#include "Uneye/Core/Application.h"

#include <GLFW/glfw3.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>




namespace Uneye {

	EditorCamera::EditorCamera(float fov, float aspectRatio, float nearClip, float farClip)
		: Camera(glm::perspective(glm::radians(fov), aspectRatio, nearClip, farClip)), m_FOV(fov),
		m_AspectRatio(aspectRatio), m_NearClip(nearClip), m_FarClip(farClip)
	{
		m_Position = CalculatePosition();
		UpdateView();
	}

	void EditorCamera::UpdateProjection()
	{
		m_AspectRatio = m_ViewportWidth / m_ViewportHeight;
		m_Projection = glm::perspective(glm::radians(m_FOV * m_Zoom), m_AspectRatio, m_NearClip, m_FarClip);
	}

	void EditorCamera::UpdateView()
	{
		if (m_IsFixed2D)
		{
			m_Position.z = CalculatePosition().z;
		}
		else if(m_NewZoom != m_Zoom)
		{	
			m_Zoom = m_NewZoom;
			UpdateProjection();
		}

		m_ViewMatrix = glm::lookAt(m_Position, m_Position + m_Front, m_Up);
	}

	void EditorCamera::SetFix2D(bool p_fix)
	{
		if (p_fix)
		{
			m_FocalPoint = { 0.0f, 0.0f, 0.0f };
			m_Front = { 0.0f, 0.0f, -1.0f };
			m_Up = { 0.0f, 1.0f, 0.0f };
			m_Right = { 1.0f, 0.0f, 0.0f };
			m_Distance = 10.0f;
			m_Position = CalculatePosition();
		}

		m_IsFixed2D = p_fix;

		UpdateView();
	}

	void EditorCamera::UpdateCameraVectors()
	{
		glm::vec3 front;
		front.x = cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
		front.y = sin(glm::radians(m_Pitch));
		front.z = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));

		m_Front = glm::normalize(front);
		m_Right = glm::normalize(glm::cross(m_Front, m_WorldUp));
		m_Up = glm::normalize(glm::cross(m_Right, m_Front));
	}

	float EditorCamera::ZoomSpeed() const
	{
		// TODO: maybe revisit this?

		if (!m_IsFixed2D)
		{
			float zoom = m_Zoom * 0.35f;
			zoom = std::max(zoom, 0.0f);
			float speed = zoom * zoom;
			speed = std::min(speed, 1.0f); // max speed = 1
			return speed;
		}

		float zoom = m_Distance * 0.2f;
		zoom = std::max(zoom, 0.2f);
		float speed = zoom * zoom;
		speed = std::min(speed, 100.0f); // max speed = 100
		return speed;
	}

	void EditorCamera::OnUpdate(Timestep ts)
	{
		if (!m_IsFocused)
			return;

		glm::vec2 mouse{ Input::GetMouseX(), Input::GetMouseY() };
		glm::vec2 delta = (mouse - m_InitialMousePosition);
		m_InitialMousePosition = mouse;

		if (m_IsFixed2D)
		{
			ProcessKeyboard2D(ts);
		}
		else
		{
			if (!Input::IsKeyPressed(Key::C) && m_Zoom != 1.0f)
			{
				m_Zoom = 1.0f;
				m_NewZoom = m_Zoom;
				UpdateProjection();
			}


			if (Input::IsMouseButtonPressed(1))
			{
				Application::Get().GetWindow().SetCursorMode(CursorMode::DISABLED);

				auto newDelta = delta * 100.0f * (float)ts;
				ProcessMouseMovement(newDelta);

				ProcessKeyboard3D(ts);
			}
			else
			{
				Application::Get().GetWindow().SetCursorMode(CursorMode::NORMAL);
			}
		}

		if (m_IsLimited)
		{
			m_Position.x = std::max(m_Position.x, m_LimitMin.x);
			m_Position.y = std::max(m_Position.y, m_LimitMin.y);
			m_Position.z = std::max(m_Position.z, m_LimitMin.z);

			m_Position.x = std::min(m_Position.x, m_LimitMax.x);
			m_Position.y = std::min(m_Position.y, m_LimitMax.y);
			m_Position.z = std::min(m_Position.z, m_LimitMax.z);
		}


		UpdateView();
	}

	void EditorCamera::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<MouseScrolledEvent>(UNEYE_BIND_EVENT_FN(EditorCamera::OnMouseScroll));
	}

	bool EditorCamera::OnMouseScroll(MouseScrolledEvent& e)
	{
		if (Input::IsKeyPressed(Key::C) || m_IsFixed2D)
		{
			float delta = e.GetYOffset() * 0.1f;
			MouseZoom(delta);
			UpdateView();
		}
		return false;
	}

	void EditorCamera::MouseZoom(float delta)
	{
		if (m_IsFixed2D)
		{
			m_Distance -= delta * ZoomSpeed();
			if (m_Distance < 1.0f)
			{
				m_FocalPoint += m_Front;
				m_Distance = 1.0f;
			}
		}
		else
		{
			m_NewZoom -= delta * ZoomSpeed();
			if (m_NewZoom < 0.1f)
			{
				m_FocalPoint += m_Front;
				m_NewZoom = 0.1f;
			}
			//m_NewZoom = std::max(m_NewZoom, 0.1f);
			m_NewZoom = std::min(m_NewZoom, 1.0f);
		}
	}

	glm::vec3 EditorCamera::CalculatePosition() const
	{
		return m_FocalPoint - m_Front * m_Distance;
	}

	void EditorCamera::ProcessKeyboard2D(Timestep p_ts)
	{
		float speed = m_MovementSpeed * p_ts;

		if (Input::IsKeyPressed(Key::W))
			m_Position += m_Up * speed;
		if (Input::IsKeyPressed(Key::S))
			m_Position -= m_Up * speed;

		if (Input::IsKeyPressed(Key::A))
			m_Position -= m_Right * speed;
		if (Input::IsKeyPressed(Key::D))
			m_Position += m_Right * speed;
	}

	void EditorCamera::ProcessKeyboard3D(Timestep p_ts)
	{
		float speed = m_MovementSpeed * p_ts;

		glm::vec3 frontXZ = glm::normalize(glm::vec3(m_Front.x, 0.0f, m_Front.z));

		if (Input::IsKeyPressed(Key::W))
			m_Position += frontXZ * speed;
		if (Input::IsKeyPressed(Key::S))
			m_Position -= frontXZ * speed;

		if (Input::IsKeyPressed(Key::A))
			m_Position -= m_Right * speed;
		if (Input::IsKeyPressed(Key::D))
			m_Position += m_Right * speed;

		if (Input::IsKeyPressed(Key::Space))
			m_Position += m_Up * speed;
		if (Input::IsKeyPressed(Key::LeftControl))
			m_Position -= m_Up * speed;
	}

	void EditorCamera::ProcessMouseMovement(const glm::vec2& p_delta, bool p_constrainPitch)
	{
		float yawSign = m_Up.y < 0 ? -1.0f : 1.0f;
		m_Yaw += yawSign * p_delta.x * m_MouseSensitivity;
		float pitchSign = m_Up.y < 0 ? 1.0f : -1.0f;
		m_Pitch += pitchSign * p_delta.y * m_MouseSensitivity;

		if (p_constrainPitch)
		{
			if (m_Pitch > 89.0f)
				m_Pitch = 89.0f;
			if (m_Pitch < -89.0f)
				m_Pitch = -89.0f;
		}

		UpdateCameraVectors();
	}

}
