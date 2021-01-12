#include "ObjectFollow.h"

#include "game/utils/glm_utils.hpp"
#include "mge/core/GameObject.hpp"

ObjectFollow::ObjectFollow(GameObject* target, const glm::vec3 offset, const glm::vec3 eulerAngles, const float orbitSpeed) : target(target), offset(offset),
	orbitEulerAngles(0), orbitSpeed(orbitSpeed) {
	SetRotation(eulerAngles);
}

ObjectFollow::~ObjectFollow() {
}

void ObjectFollow::update(float ts) {
	if (target == nullptr) return;

	if (rotationDirty) {
		UpdateRotation();
		rotationDirty = false;
	}

	// Todo interpolate
	const auto targetPosition = target->getWorldPosition() + offset;
	_owner->setLocalPosition(targetPosition);
}

void ObjectFollow::SetRotation(glm::vec3 eulerAngles) {
	this->eulerAngles = eulerAngles;
	rotationDirty = true;
}

void ObjectFollow::UpdateRotation() {
	// Get original transform & decompose into translation, rotation, scale
	const auto transform = _owner->getTransform();
	glm::vec3 translation;
	glm::quat rotation;
	glm::vec3 scale;
	decompose(transform, scale, rotation, translation);

	// Calculate new matrix using original translation & scale, but with new rotation
	auto matrix = glm::mat4(1.0f);
	matrix = translate(matrix, translation);
	matrix = utils::glm::RotateEulerXYZDegrees(matrix, eulerAngles);
	matrix = glm::scale(matrix, scale);

	_owner->setTransform(matrix);
}

/*void ObjectFollow::UpdateOrbit(float ts) {
	const bool isLeftPressed = sf::Mouse::isButtonPressed(sf::Mouse::Left);
	const bool isRightPressed = sf::Mouse::isButtonPressed(sf::Mouse::Right);
	const bool isShiftPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) || sf::Keyboard::isKeyPressed(sf::Keyboard::RShift);

	if (!isLeftPressed && !isRightPressed) return;

	if (isRightPressed) orbitEulerAngles.x += orbitSpeed * ts * (isShiftPressed ? -1 : 1);
	if (orbitEulerAngles.x >= 360.0f) orbitEulerAngles.x -= 360.0f;

	if (isLeftPressed) orbitEulerAngles.y += orbitSpeed * ts * (isShiftPressed ? -1 : 1);
	if (orbitEulerAngles.y >= 360.0f) orbitEulerAngles.y -= 360.0f;

	rotationDirty = true;
}*/

/*glm::vec3 ObjectFollow::CalculateOrbitPosition() {
	auto camPos = _owner->getWorldPosition();
	auto objPos = target->getWorldPosition();
	
	auto up = glm::vec3(0,1,0);
	auto objToCam = camPos - objPos;
	auto right = glm::cross(glm::normalize(objToCam), up);

	// Fix up / get actual up
	up = glm::cross(right, objToCam);

	// Finally, calculate new position of camera
	auto heading = glm::rotate(orbitEulerAngles.y, up);
	auto pitch = glm::rotate(orbitEulerAngles.x, right);
	auto relCamPos = heading * pitch * glm::vec4(objToCam.x, objToCam.y, objToCam.z, 1);
	auto newCamPos = glm::xyz(relCamPos) + objPos;

	return newCamPos;
}*/