#pragma once

#include <glm/glm.hpp>

namespace glm {

	struct vec5 : vec4 {
		explicit vec5(const float scalar) : vec5(vec4(scalar), scalar) {
		}
		vec5(const vec3& abc, const float d, const float e) : vec5(abc.x, abc.y, abc.z, d, e) {
		}

		vec5(const vec4& abcd, const float e) : vec5(abcd.x, abcd.y, abcd.z, abcd.w, e) {
		}

		vec5(const float a, const float b, const float c, const float d, const float e): a(a), b(b), c(c), d(d), e(e) {
		}

		float a, b, c, d, e;
	};
}
