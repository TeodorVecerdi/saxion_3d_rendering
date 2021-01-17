#pragma once

#include <glm/glm.hpp>

namespace glm {

	struct vec6 {
		explicit vec6(const float scalar) : vec6(scalar, scalar, scalar, scalar, scalar, scalar) {
		}
		vec6(const vec3& abc, const float d, const float e, const float f) : vec6(abc.x, abc.y, abc.z, d, e, f) {
		}

		vec6(const vec4& abcd, const float e, const float f) : vec6(abcd.x, abcd.y, abcd.z, abcd.w, e, f) {
		}

		vec6(const float a, const float b, const float c, const float d, const float e, const float f): a(a), b(b), c(c), d(d), e(e), f(f) {
		}

		float a, b, c, d, e, f;
	};
}
