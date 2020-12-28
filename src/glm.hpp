#pragma once

#include <iomanip>

#define GLM_FORCE_RADIANS
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/ext.hpp>

/* the functions:
 * std::ostream & operator << ( std::ostream & out, glm::vec3 v );
 * std::ostream & operator << ( std::ostream & out, glm::vec4 v );
 * std::ostream & operator << ( std::ostream & out, glm::quat q );
 * std::ostream & operator << ( std::ostream & out, glm::mat4 m );
 */

inline std::ostream& operator <<(std::ostream& out, const glm::ivec2& v) {
	out << "(" << v.x << "," << v.y << ")";
	return out;
}

inline std::ostream& operator <<(std::ostream& out, const glm::uvec3& v) {
	out << "(" << v.x << "," << v.y << v.z << ")";
	return out;
}

inline std::ostream& operator <<(std::ostream& out, const glm::vec2& v) {
	out << "(" << v.x << "," << v.y << ")";
	return out;
}

inline std::ostream& operator <<(std::ostream& out, const glm::vec3& v) {
	out << "(" << v.x << "," << v.y << "," << v.z << ")";
	return out;
}

inline std::ostream& operator <<(std::ostream& out, const glm::vec4& v) {
	out << "(" << v.x << "," << v.y << "," << v.z << "," << v.w << ")";
	return out;
}

inline std::ostream& operator <<(std::ostream& out, const glm::quat& q) {
	out << "(" << q.x << "," << q.y << "," << q.z << "," << q.w << ")";
	return out;
}

inline std::ostream& operator <<(std::ostream& out, const glm::mat4& m) {
	out.precision(1);
	for (int r = 0; r < 4; r++) {
		for (int c = 0; c < 4; c++) {
			out << std::showpos << std::fixed << m[c][r] << " ";
		}
		out << std::endl;
	}
	return out;
}

namespace glm {
	namespace detail {

		template <typename T, qualifier Q>
		GLM_FUNC_QUALIFIER vec<3, T, Q> _scale(vec<3, T, Q> const& v, T desiredLength) {
			return v * desiredLength / length(v);
		}
	}

	/// Adapted from glm/gtx/matrix_decompose.inl to remove Skew and Perspective
	template <typename T, qualifier Q>
	bool decompose(mat<4, 4, T, Q> const& matrix, vec<3, T, Q>& scale, qua<T, Q>& rotation, vec<3, T, Q>& translation) {
		mat<4, 4, T, Q> LocalMatrix(matrix);

		// Normalize the matrix.
		if (glm::epsilonEqual(LocalMatrix[3][3], static_cast<T>(0), glm::epsilon<T>()))
			return false;

		for (length_t i = 0; i < 4; ++i)
			for (length_t j = 0; j < 4; ++j)
				LocalMatrix[i][j] /= LocalMatrix[3][3];

		// perspectiveMatrix is used to solve for perspective, but it also provides
		// an easy way to test for singularity of the upper 3x3 component.
		mat<4, 4, T, Q> PerspectiveMatrix(LocalMatrix);

		for (length_t i = 0; i < 3; i++)
			PerspectiveMatrix[i][3] = static_cast<T>(0);
		PerspectiveMatrix[3][3] = static_cast<T>(1);

		/// TODO: Fixme!
		if (glm::epsilonEqual(determinant(PerspectiveMatrix), static_cast<T>(0), glm::epsilon<T>()))
			return false;

		// Next take care of translation (easy).
		translation = vec<3, T, Q>(LocalMatrix[3]);
		LocalMatrix[3] = vec<4, T, Q>(0, 0, 0, LocalMatrix[3].w);

		vec<3, T, Q> Row[3], Pdum3;

		// Now get scale and shear.
		for (length_t i = 0; i < 3; ++i)
			for (length_t j = 0; j < 3; ++j)
				Row[i][j] = LocalMatrix[i][j];

		// Compute X scale factor and normalize first row.
		scale.x = glm::length(Row[0]); // v3Length(Row[0]);

		Row[0] = detail::_scale(Row[0], static_cast<T>(1));

		// Now, compute Y scale and normalize 2nd row.
		scale.y = glm::length(Row[1]);
		Row[1] = detail::_scale(Row[1], static_cast<T>(1));

		// Next, get Z scale and normalize 3rd row.
		scale.z = glm::length(Row[2]);

		// At this point, the matrix (in rows[]) is orthonormal.
		// Check for a coordinate system flip.  If the determinant
		// is -1, then negate the matrix and the scaling factors.
		Pdum3 = glm::cross(Row[1], Row[2]); // v3Cross(row[1], row[2], Pdum3);
		if (glm::dot(Row[0], Pdum3) < 0) {
			for (length_t i = 0; i < 3; i++) {
				scale[i] *= static_cast<T>(-1);
				Row[i] *= static_cast<T>(-1);
			}
		}

		T root, trace = Row[0].x + Row[1].y + Row[2].z;
		if (trace > static_cast<T>(0)) {
			root = glm::sqrt(trace + static_cast<T>(1.0));
			rotation.w = static_cast<T>(0.5) * root;
			root = static_cast<T>(0.5) / root;
			rotation.x = root * (Row[1].z - Row[2].y);
			rotation.y = root * (Row[2].x - Row[0].z);
			rotation.z = root * (Row[0].y - Row[1].x);
		} // End if > 0
		else {
			static int Next[3] = {1, 2, 0};
			int i = 0;
			if (Row[1].y > Row[0].x) i = 1;
			if (Row[2].z > Row[i][i]) i = 2;
			int j = Next[i];
			int k = Next[j];

			root = glm::sqrt(Row[i][i] - Row[j][j] - Row[k][k] + static_cast<T>(1.0));

			rotation[i] = static_cast<T>(0.5) * root;
			root = static_cast<T>(0.5) / root;
			rotation[j] = root * (Row[i][j] + Row[j][i]);
			rotation[k] = root * (Row[i][k] + Row[k][i]);
			rotation.w = root * (Row[j][k] - Row[k][j]);
		} // End if <= 0

		return true;
	}
}
