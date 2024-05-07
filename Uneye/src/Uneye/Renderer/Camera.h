#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> 


namespace Uneye
{
	class Camera
	{
		public:
			Camera() = default;
			Camera(const glm::mat4& projection)
				: m_Projection(projection) {}
			virtual ~Camera() = default;

			const glm::mat4& GetProjection() const { return m_Projection; }

			// TODO:
			// SetPerspective(...)

		protected:
			glm::mat4 m_Projection = glm::mat4(1.0f);
	};


	class OrthographicCamera
	{
		public:
			OrthographicCamera() {}
			OrthographicCamera(float left, float right, float bottom, float top);

			void SetProjection(float left, float right, float bottom, float top);

			void SetPosition(const glm::vec3& position) { m_Position = position; RecalculateMatrix(); }
			const glm::vec3& GetPosition() const { return m_Position;  }

			void SetRotation(float rotation) { m_Rotation = rotation; RecalculateMatrix(); }
			float GetRotation() const { return m_Rotation; }

			const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix;  }
			const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
			const glm::mat4& GetViewProjectionMatrix() const { return m_ViewProjectionMatrix; }

		private:
			void RecalculateMatrix();

		private:
			glm::mat4 m_ProjectionMatrix{ 1.0f };
			glm::mat4 m_ViewMatrix{ 1.0f };
			glm::mat4 m_ViewProjectionMatrix{ 1.0f };

			glm::vec3 m_Position{ 0.0f };
			float m_Rotation = 0.0f;
	};

}