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

}
