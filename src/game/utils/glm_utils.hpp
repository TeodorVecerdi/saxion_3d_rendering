#pragma once

#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <glm/gtx/transform.hpp>

#include "constants.hpp"

namespace utils::glm {

	inline ::glm::mat4 RotateEulerXYZ(::glm::mat4 source, ::glm::vec3 eulerAngles) {
		source = rotate(source, eulerAngles.y, constants::up);
		source = rotate(source, eulerAngles.x, constants::right);
		source = rotate(source, eulerAngles.z, constants::forward);
		return source;
	}

	inline ::glm::mat4 RotateEulerXYZ(::glm::vec3 eulerAngles) {
		return RotateEulerXYZ(constants::identity, eulerAngles);
	}

	inline ::glm::mat4 RotateEulerXYZDegrees(::glm::mat4 source, ::glm::vec3 eulerAngles) {
		const auto radians = constants::radToDeg * eulerAngles;
		return RotateEulerXYZ(source, radians);
	}

	inline ::glm::mat4 RotateEulerXYZDegrees(::glm::vec3 eulerAngles) {
		return RotateEulerXYZDegrees(constants::identity, eulerAngles);
	}
}
