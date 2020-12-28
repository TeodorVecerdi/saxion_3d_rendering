#pragma once

#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <glm/gtx/transform.hpp>

namespace utils::glm {
	inline auto RotateEulerXYZ(::glm::mat4 source, ::glm::vec3 eulerAngles) -> ::glm::mat4 {
		source = ::glm::rotate(source, ::glm::radians(eulerAngles.y), ::glm::vec3(0, 1, 0));
		source = ::glm::rotate(source, ::glm::radians(eulerAngles.x), ::glm::vec3(1, 0, 0));
		source = ::glm::rotate(source, ::glm::radians(eulerAngles.z), ::glm::vec3(0, 0, 1));
		return source;
	}

}
