#include "LightRotatingBehaviour.h"

#include "game/utils/constants.hpp"
#include "game/utils/glm_utils.hpp"
#include "game/utils/math_utils.hpp"
#include "mge/core/Light.hpp"

LightRotatingBehaviour::LightRotatingBehaviour(const float speed, const glm::vec3 angleOffset) : speed(speed), angleOffset(angleOffset) {
	currentRotation = 0;
}

void LightRotatingBehaviour::Update(const float pStep) {
	currentRotation += speed * pStep;

	Light* ownerLight = dynamic_cast<Light*>(_owner);

	glm::quat yQuat = glm::angleAxis(angleOffset.y * utils::constants::degToRad + currentRotation, utils::constants::up);
	glm::quat xQuat = glm::angleAxis(angleOffset.x * utils::constants::degToRad, utils::constants::right);
	glm::quat zQuat = glm::angleAxis(angleOffset.z * utils::constants::degToRad, utils::constants::forward);
	glm::quat rotation = yQuat * xQuat * zQuat;
	glm::vec3 direction = glm::normalize(rotation * utils::constants::forward);

	glm::mat4 matrix = _owner->GetTransform();
	glm::vec3 trans;
	glm::vec3 scale;
	glm::quat rot;

	glm::decompose(matrix, scale, rot, trans);

	glm::mat4 newMatrix = utils::constants::identity;
	newMatrix = glm::translate(trans) * glm::toMat4(rotation) * glm::scale(scale);
	_owner->SetTransform(newMatrix);
	
	ownerLight->SetDirection(direction);
}
