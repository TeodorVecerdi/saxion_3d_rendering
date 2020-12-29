#pragma once

#include <limits>
#include <glm/glm.hpp>
#include <glm/trigonometric.hpp>
#include <glm/gtx/integer.hpp>

namespace utils::math {
	inline auto Max(float a, float b) -> float {
		return a > b ? a : b;
	}

	inline auto Min(float a, float b) -> float {
		return a < b ? a : b;
	}

	inline auto Clamp(float value, float min, float max) -> float {
		if (value < min) return min;
		if (value > max) return max;
		return value;
	}

	inline auto Clamp01(float value) -> float {
		return Clamp(value, 0.0f, 1.0f);
	}

	/// @brief Interpolates between /from/ and /to/ with smoothing at the limits
	/// Adapted from Unity's implementation in Mathf 
	inline auto SmoothStep(float from, float to, float t) -> float {
		t = Clamp01(t);
		t = -2.0f * t * t * t + 3.0f * t * t;
		return to * t + from * (1.0f - t);
	}

	/// @brief Interpolates between /from/ and /to/ with smoothing at the limits
	inline auto SmoothStep(::glm::vec2 from, ::glm::vec2 to, float t) -> ::glm::vec2 {
		t = Clamp01(t);
		t = -2.0f * t * t * t + 3.0f * t * t;
		return to * t + from * (1.0f - t);
	}

	/// @brief Interpolates between /from/ and /to/ with smoothing at the limits
	inline auto SmoothStep(::glm::vec3 from, ::glm::vec3 to, float t) -> ::glm::vec3 {
		t = Clamp01(t);
		t = -2.0f * t * t * t + 3.0f * t * t;
		return to * t + from * (1.0f - t);
	}

	/// @brief Gradually changes a value towards a desired goal over time.
	/// Adapted from Unity's implementation in Mathf 
	inline float SmoothDamp(float current, float target, float& currentVelocity, float smoothTime, float deltaTime) {
		const float maxSpeed = std::numeric_limits<float>::infinity();

		// Based on Game Programming Gems 4 Chapter 1.10
		smoothTime = Max(0.0001f, smoothTime);
		const float omega = 2.0f / smoothTime;

		const float x = omega * deltaTime;
		const float exp = 1.0f / (1.0f + x + 0.48f * x * x + 0.235f * x * x * x);
		float change = current - target;
		const float originalTo = target;

		// Clamp maximum speed
		const float maxChange = maxSpeed * smoothTime;
		change = Clamp(change, -maxChange, maxChange);
		target = current - change;

		const float temp = (currentVelocity + omega * change) * deltaTime;
		currentVelocity = (currentVelocity - omega * temp) * exp;
		float output = target + (change + temp) * exp;

		// Prevent overshooting
		if (originalTo - current > 0.0f == output > originalTo) {
			output = originalTo;
			currentVelocity = (output - originalTo) / deltaTime;
		}

		return output;
	}

	/// @brief Gradually changes a vector towards a desired goal over time.
    /// Adapted from Unity's implementation in Vector3 
	inline ::glm::vec3 SmoothDamp(::glm::vec3 current, ::glm::vec3 target, ::glm::vec3& currentVelocity, float smoothTime, float deltaTime) {
		const float maxSpeed = std::numeric_limits<float>::infinity();

		float output_x = 0.0f;
		float output_y = 0.0f;
		float output_z = 0.0f;

		// Based on Game Programming Gems 4 Chapter 1.10
		smoothTime = Max(0.0001f, smoothTime);
		const float omega = 2.0f / smoothTime;

		const float x = omega * deltaTime;
		const float exp = 1.0f / (1.0f + x + 0.48f * x * x + 0.235f * x * x * x);

		float change_x = current.x - target.x;
		float change_y = current.y - target.y;
		float change_z = current.z - target.z;
		const ::glm::vec3 originalTo = target;

		// Clamp maximum speed
		const float maxChange = maxSpeed * smoothTime;

		const float maxChangeSq = maxChange * maxChange;
		const float sqrmag = change_x * change_x + change_y * change_y + change_z * change_z;
		if (sqrmag > maxChangeSq) {
			auto mag = ::glm::sqrt(sqrmag);
			change_x = change_x / mag * maxChange;
			change_y = change_y / mag * maxChange;
			change_z = change_z / mag * maxChange;
		}

		target.x = current.x - change_x;
		target.y = current.y - change_y;
		target.z = current.z - change_z;

		const float temp_x = (currentVelocity.x + omega * change_x) * deltaTime;
		const float temp_y = (currentVelocity.y + omega * change_y) * deltaTime;
		const float temp_z = (currentVelocity.z + omega * change_z) * deltaTime;

		currentVelocity.x = (currentVelocity.x - omega * temp_x) * exp;
		currentVelocity.y = (currentVelocity.y - omega * temp_y) * exp;
		currentVelocity.z = (currentVelocity.z - omega * temp_z) * exp;

		output_x = target.x + (change_x + temp_x) * exp;
		output_y = target.y + (change_y + temp_y) * exp;
		output_z = target.z + (change_z + temp_z) * exp;

		// Prevent overshooting
		const float origMinusCurrent_x = originalTo.x - current.x;
		const float origMinusCurrent_y = originalTo.y - current.y;
		const float origMinusCurrent_z = originalTo.z - current.z;
		const float outMinusOrig_x = output_x - originalTo.x;
		const float outMinusOrig_y = output_y - originalTo.y;
		const float outMinusOrig_z = output_z - originalTo.z;

		if (origMinusCurrent_x * outMinusOrig_x + origMinusCurrent_y * outMinusOrig_y + origMinusCurrent_z * outMinusOrig_z > 0) {
			output_x = originalTo.x;
			output_y = originalTo.y;
			output_z = originalTo.z;

			currentVelocity.x = (output_x - originalTo.x) / deltaTime;
			currentVelocity.y = (output_y - originalTo.y) / deltaTime;
			currentVelocity.z = (output_z - originalTo.z) / deltaTime;
		}

		return ::glm::vec3(output_x, output_y, output_z);
	}

	/// @brief Clamps a quaternion to be in /bounds/ in degrees
	/// Adapted from 'https://forum.unity.com/threads/how-do-i-clamp-a-quaternion.370041/#post-5494723'
	inline ::glm::quat ClampRotation(::glm::quat q, ::glm::vec3 bounds)
	{
		q.x /= q.w;
		q.y /= q.w;
		q.z /= q.w;
		q.w = 1.0f;
 
		float angleX = 2.0f * ::glm::degrees(::glm::atan(q.x));
		angleX = Clamp(angleX, -bounds.x, bounds.x);
		q.x = ::glm::tan(0.5f * ::glm::radians(angleX));
 
		float angleY = 2.0f * ::glm::degrees(::glm::atan(q.y));
		angleY = Clamp(angleY, -bounds.y, bounds.y);
		q.y = ::glm::tan(0.5f * ::glm::radians(angleY));
 
		float angleZ = 2.0f * ::glm::degrees(::glm::atan(q.z));
		angleZ = Clamp(angleZ, -bounds.z, bounds.z);
		q.z = ::glm::tan(0.5f * ::glm::radians(angleZ));

		return q;
	}
}
