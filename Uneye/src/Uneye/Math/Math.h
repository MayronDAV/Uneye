#pragma once

#include <glm/glm.hpp>


namespace Uneye::Math
{
	bool DecomposeTransform(const glm::mat4& transform, glm::vec3& translation,
		glm::vec3& rtation, glm::vec3& scale);

}
