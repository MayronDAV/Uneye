#include "uypch.h"
#include "Uneye/Renderer/CameraController.h"

#include "Uneye/Core/Input.h"
#include "Uneye/Core/KeyCodes.h"



namespace Uneye
{

	OrthographicCameraController::OrthographicCameraController(float aspectratio, bool rotation)
		:m_AspectRatio(aspectratio),
		m_Bounds({ -m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel }),
		m_Rotation(rotation),
		m_Camera(-aspectratio * m_ZoomLevel, aspectratio* m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel)
	{

	}

	void OrthographicCameraController::OnUpdate(Timestep ts)
	{
		UNEYE_PROFILE_FUNCTION();

		m_Direction = glm::vec3(0.0f);

		if (Input::IsKeyPressed(Key::W))
			m_Direction.y = 1.0f;
		if (Input::IsKeyPressed(Key::A))
			m_Direction.x = -1.0f;
		if (Input::IsKeyPressed(Key::S))
			m_Direction.y = -1.0f;
		if (Input::IsKeyPressed(Key::D))
			m_Direction.x = 1.0f;

		if (m_Rotation)
		{
			if (Input::IsKeyPressed(Key::Q))
				m_CameraRotation -= m_CameraRotationSpeed * ts;
			if (Input::IsKeyPressed(Key::E))
				m_CameraRotation -= m_CameraRotation * ts;

			m_Camera.SetRotation(m_CameraRotation);
		}

		m_Direction = m_Direction / ((glm::length(m_Direction) != 0) ?
			glm::length(m_Direction) : 1.0f);

		m_CameraPosition += m_Direction * m_Speed * ts.GetSeconds();
		m_Camera.SetPosition(m_CameraPosition);

		m_Speed = glm::vec3(m_ZoomLevel);
	}

	void OrthographicCameraController::OnEvent(Event& e)
	{
		UNEYE_PROFILE_FUNCTION();

		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<MouseScrolledEvent>(UNEYE_BIND_EVENT_FN(
			OrthographicCameraController::OnMouseScrolled));
		dispatcher.Dispatch<WindowResizeEvent>(UNEYE_BIND_EVENT_FN(
			OrthographicCameraController::OWindowResize));
	}

	bool OrthographicCameraController::OnMouseScrolled(MouseScrolledEvent& e)
	{
		UNEYE_PROFILE_FUNCTION();

		m_ZoomLevel-= e.GetYOffset() * 0.25f;
		m_ZoomLevel = std::max(m_ZoomLevel, 0.01f);
		//m_ZoomLevel = std::min(m_ZoomLevel, 4.0f);
		m_Bounds = { -m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel };
		m_Camera.SetProjection(m_Bounds.Left, m_Bounds.Right, m_Bounds.Bottom, m_Bounds.Top);

		return false;
	}

	bool OrthographicCameraController::OWindowResize(WindowResizeEvent& e)
	{
		UNEYE_PROFILE_FUNCTION();

		m_AspectRatio = (float)e.GetWidth() / (float)e.GetHeight();
		m_Bounds = { -m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel };
		m_Camera.SetProjection(m_Bounds.Left, m_Bounds.Right, m_Bounds.Bottom, m_Bounds.Top);

		return false;
	}

}