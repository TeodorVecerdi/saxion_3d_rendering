#include "DayNightCycleBehaviour.h"

#include <iostream>

#include "game/utils/constants.hpp"
#include "game/utils/math_utils.hpp"
#include "mge/core/Light.hpp"

DayNightCycleBehaviour::DayNightCycleBehaviour(Light& day, Light& night, float speed, glm::vec3 axis, glm::vec3 offset): day(&day), night(&night), speed(speed),
	axis(glm::normalize(axis)), offset(offset) {
	currentRotation = 0;

	glm::quat yQuat = glm::angleAxis(offset.y * utils::constants::degToRad, utils::constants::up);
	glm::quat xQuat = glm::angleAxis(offset.x * utils::constants::degToRad, utils::constants::right);
	glm::quat zQuat = glm::angleAxis(offset.z * utils::constants::degToRad, utils::constants::forward);
	baseRotation = yQuat * xQuat * zQuat;
}

void DayNightCycleBehaviour::Start() {
	dayAmbientIntensity = day->GetAmbientIntensity();
	dayDiffuseIntensity = day->GetDiffuseIntensity();
	daySpecularIntensity = day->GetSpecularIntensity();
	
	nightAmbientIntensity = night->GetAmbientIntensity();
	nightDiffuseIntensity = night->GetDiffuseIntensity();
	nightSpecularIntensity = night->GetSpecularIntensity();
}

void DayNightCycleBehaviour::Update(const float ts) {
	currentRotation += speed * ts;

	/// SET DIRECTION
	const glm::quat currentRotationQuat = glm::angleAxis(currentRotation, axis);
	const glm::quat rotation = currentRotationQuat * baseRotation;

	const glm::vec3 directionDay = glm::normalize(rotation * utils::constants::forward);
	const glm::vec3 directionNight = -directionDay;
	day->SetDirection(directionDay);
	night->SetDirection(directionNight);

	/// SET INTENSITY
	float intensityRotation = utils::math::WrapPi(currentRotation);
	float intensityRotationOffset = 0.0f;
	float intensity;
	// LIGHT RISE
	if (intensityRotation >= -30 * utils::constants::oneDeg + intensityRotationOffset && intensityRotation < utils::constants::deg45 + intensityRotationOffset)
		intensity = glm::lerp(0.0f, 1.0f, (intensityRotation + 30 * utils::constants::oneDeg) / (utils::constants::deg45 + 30 * utils::constants::oneDeg));
		// MAINTAIN LIT
	else if (intensityRotation >= utils::constants::deg45 + intensityRotationOffset && intensityRotation < utils::constants::deg180 - utils::constants::deg45 +
		intensityRotationOffset)
		intensity = 1.0f;
		// LIGHT SET
	else if (intensityRotation >= utils::constants::deg180 - utils::constants::deg45 + intensityRotationOffset &&
		intensityRotation < utils::constants::deg180 + intensityRotationOffset ||
		intensityRotation >= -utils::constants::deg180 &&
		intensityRotation < -utils::constants::deg180 + 30 * utils::constants::oneDeg
	) {
		float intensityAngle;
		if (intensityRotation >= 0)
			intensityAngle = intensityRotation - utils::constants::deg180 + utils::constants::deg45; // 0->45
		else
			intensityAngle = intensityRotation + utils::constants::deg45 + utils::constants::deg180; //45->75
		intensity = glm::lerp(1.0f, 0.0f, (intensityAngle) / (utils::constants::deg45 + 30 * utils::constants::oneDeg));
	}
		// MAINTAIN UNLIT
	else intensity = 0.0f;

	day->SetAmbientIntensity(dayAmbientIntensity * intensity);
	day->SetDiffuseIntensity(dayDiffuseIntensity * intensity);
	day->SetSpecularIntensity(daySpecularIntensity * intensity);

	night->SetAmbientIntensity(nightAmbientIntensity * (1 - intensity));
	night->SetDiffuseIntensity(nightDiffuseIntensity * (1 - intensity));
	night->SetSpecularIntensity(nightSpecularIntensity * (1 - intensity));

}