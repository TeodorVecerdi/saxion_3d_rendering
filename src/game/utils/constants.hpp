#pragma once

#include <glm/glm.hpp>
#include <limits>

namespace utils::constants {
	// @formatter:off
	constexpr glm::vec3 right    = { 1.0,  0.0,  0.0};
	constexpr glm::vec3 left     = {-1.0,  0.0,  0.0};
	constexpr glm::vec3 up       = { 0.0,  1.0,  0.0};
	constexpr glm::vec3 down     = { 0.0, -1.0,  0.0};
	constexpr glm::vec3 forward  = { 0.0,  0.0,  1.0};
	constexpr glm::vec3 backward = { 0.0,  0.0, -1.0};
	constexpr glm::vec3 zero     = { 0.0,  0.0,  0.0};
	constexpr glm::vec3 one      = { 1.0,  1.0,  1.0};
	constexpr glm::vec3 positiveInfinity =   std::numeric_limits<float>::infinity() * one;
	constexpr glm::vec3 negativeInfinity =  -std::numeric_limits<float>::infinity() * one;

	constexpr glm::mat4 identity = {1, 0, 0, 0,
									0, 1, 0, 0,
									0, 0, 1, 0,
									0, 0, 0, 1 };

	constexpr float epsilon = std::numeric_limits<float>::epsilon();
	
	/// Returns PI * 2 (360 degrees)
	constexpr float twoPi        = 6.283185307f;
	/// 360 degrees, in radians
	constexpr float deg360       = twoPi;
	/// Returns PI (180 degrees)
	constexpr float pi           = 3.141592654f;
	/// 180 degrees, in radians
	constexpr float deg180       = pi;
	/// Returns PI / 2 (90 degrees)
	constexpr float halfPi       = 1.570796327f;
	/// 90 degrees, in radians
	constexpr float deg90        = halfPi;
	/// Returns PI / 4 (45 degrees)
	constexpr float quarterPi    = 0.7853981634f;
	/// 45 degrees, in radians
	constexpr float deg45        = quarterPi;
	/// Returns PI / 6, (30 degrees)
	constexpr float sixthPi      = 0.5235987756f;
	/// 30 degrees, in radians
	constexpr float deg30        = sixthPi;
	/// Converts radians to degrees
	constexpr float radToDeg     = 57.295779513f;
	/// Converts degrees to radians
	constexpr float degToRad     = 0.017453292f;
	/// 1 degree, in radians
	constexpr float oneDeg        = degToRad;
	/// 0.5 degrees, in radians
	constexpr float halfDeg      = 0.008726646f;

	// @formatter:on
}
