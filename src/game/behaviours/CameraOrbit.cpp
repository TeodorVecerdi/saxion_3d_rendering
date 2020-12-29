#include "CameraOrbit.h"

#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>

#include "game/utils/glm_utils.hpp"
#include "mge/core/GameObject.hpp"
#include "mge/util/Input.hpp"

CameraOrbit::CameraOrbit(GameObject* target, glm::vec3 offset, float orbitSpeed) : target(target), offset(offset), orbitSpeed(orbitSpeed), eulerAngles(0) {
	mge::Input::SetMouseLock(true);
}

CameraOrbit::~CameraOrbit() {
	mge::Input::SetMouseLock(false);
}

void CameraOrbit::lateUpdate(const float ts) {

	UpdateOrbit(ts);
	auto selfWorld = _owner->getWorldPosition();
	auto selfLocal = _owner->getLocalPosition();
	auto diff = selfWorld - selfLocal;
	auto targetPosition = target->getWorldPosition();
	auto translation = translate(diff + targetPosition);
	auto rotation = utils::glm::RotateEulerXYZ(glm::mat4(1.0f), eulerAngles) * translate(-offset);
	auto matrix = translation * rotation;

	_owner->setTransform(matrix);
}

void CameraOrbit::UpdateOrbit(const float ts) {
	const auto mousePos = mge::Input::MousePosition();
	const auto clampedMousePos = mge::Input::ClampedMousePosition();
	const auto mouseDelta = mge::Input::MouseDelta();
	printf("Mouse position: [%f,%f]; Clamped mouse position: [%f,%f]; Mouse delta: [%f,%f]\n", mousePos.x, mousePos.y, clampedMousePos.x, clampedMousePos.y, mouseDelta.x, mouseDelta.y);

	// Update speed
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q)) orbitSpeed.y -= 60.0f * ts;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::E)) orbitSpeed.y += 60.0f * ts;
	if (orbitSpeed.y < 0.0f) orbitSpeed.y = 0.0f;

	const bool isLeftPressed = sf::Mouse::isButtonPressed(sf::Mouse::Left);
	const bool isRightPressed = sf::Mouse::isButtonPressed(sf::Mouse::Right);
	const bool isShiftPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) || sf::Keyboard::isKeyPressed(sf::Keyboard::RShift);

	if (!isLeftPressed && !isRightPressed) return;

	if (isRightPressed) eulerAngles.x += orbitSpeed.x * ts * (isShiftPressed ? -1 : 1);
	if (eulerAngles.x >= 360.0f) eulerAngles.x -= 360.0f;

	if (isLeftPressed) eulerAngles.y += orbitSpeed.y * ts * (isShiftPressed ? -1 : 1);
	if (eulerAngles.y >= 360.0f) eulerAngles.y -= 360.0f;
}