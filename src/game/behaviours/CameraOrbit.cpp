#include "CameraOrbit.h"

#include <glm/trigonometric.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>

#include "game/utils/constants.hpp"
#include "game/utils/math_utils.hpp"
#include "mge/core/GameObject.hpp"
#include "mge/util/Input.hpp"

CameraOrbit::CameraOrbit(GameObject* target, glm::vec3 offset, glm::vec3 eulerAngles, float orbitSpeed, float mouseSensitivity, float scrollSensitivity) :
target(target), currentPosition(0), offset(offset), orbitSpeed(orbitSpeed), mouseSensitivity(mouseSensitivity), scrollSensitivity(scrollSensitivity), zoomRange(-2, -20) {

	targetAngleY = utils::constants::degToRad * eulerAngles.y;
	targetAngleX = utils::constants::degToRad * eulerAngles.x;
	targetZoom = offset.z;

	currentAngleY = targetAngleY;
	currentAngleX = targetAngleX;
	currentZoom = targetZoom;

	// Lock mouse to middle of screen and hide
	mge::Input::SetMouseLock(true);
}

CameraOrbit::~CameraOrbit() {
	// Unlock mouse and show
	mge::Input::SetMouseLock(false);
}

void CameraOrbit::lateUpdate(const float ts) {
	const auto targetPosition = target->getWorldPosition();
	const auto selfPosition = _owner->getWorldPosition();
	UpdateInput(ts);

	///----------------------------------------------------------
	///				     CALCULATE SMOOTH ZOOM
	///----------------------------------------------------------
	if (!utils::math::Approx(currentZoom, targetZoom, 0.01f)) {
		currentZoom = glm::lerp(currentZoom, targetZoom, 0.1f);
	}
	auto zoomedOffset = offset;
	zoomedOffset.z = currentZoom;

	///----------------------------------------------------------
	///				    CALCULATE SMOOTH POSITION
	///----------------------------------------------------------
	if (!utils::math::Approx(currentPosition, targetPosition, 0.01f)) {
		// currentPosition = glm::lerp(currentPosition, targetPosition, 0.1f);
		// currentPosition = utils::math::SmoothStep(currentPosition, targetPosition,0.2f);
		currentPosition = utils::math::SmoothDamp(currentPosition, targetPosition, moveVelocity, 0.15f, ts);
	}

	///----------------------------------------------------------
	///					CALCULATE SMOOTH ROTATION
	///----------------------------------------------------------
	if (!utils::math::Approx(currentAngleX, targetAngleX, utils::constants::halfDeg))
		currentAngleX = glm::lerp(currentAngleX, targetAngleX, 0.1f);
	const auto xQuat = glm::angleAxis(currentAngleX, utils::constants::right);

	if (!utils::math::Approx(currentAngleY, targetAngleY, utils::constants::halfDeg))
		currentAngleY = glm::lerp(currentAngleY, targetAngleY, 0.1f);
	const auto yQuat = glm::angleAxis(currentAngleY, utils::constants::up);

	///----------------------------------------------------------
	///						 SHENANIGANS
	///----------------------------------------------------------
	auto shenaniganQuatY = glm::quat_identity<float, glm::defaultp>();
	auto shenaniganQuatX = glm::quat_identity<float, glm::defaultp>();

	if constexpr (false) {
		const auto shenanigan1 = currentPosition - targetPosition;
		const auto shenanigan2 = -10.0f * shenanigan1.x * utils::constants::degToRad; // heading
		const auto shenanigan3 = shenanigan1.z * utils::constants::degToRad;          // pitch

		shenaniganQuatY = glm::angleAxis(shenanigan2, utils::constants::up);
		shenaniganQuatX = glm::angleAxis(shenanigan3, utils::constants::right);
		if (utils::math::Approx(shenanigan1.x, 0.0, 0.01))
			shenaniganQuatY = glm::quat_identity<float, glm::defaultp>();
		if (utils::math::Approx(shenanigan1.z, 0.0, 0.01))
			shenaniganQuatX = glm::quat_identity<float, glm::defaultp>();
	}

	///----------------------------------------------------------
	///				     APPLY TRANSFORMATIONS
	///----------------------------------------------------------
	auto translation = glm::translate(currentPosition);
	auto rotation = glm::toMat4((yQuat) * (xQuat)) * translate(-zoomedOffset);
	auto matrix = translation * rotation;

	_owner->setTransform(matrix);
}

void CameraOrbit::UpdateInput(const float ts) {
	///----------------------------------------------------------
	///						  UPDATE ORBIT
	///----------------------------------------------------------
	// Update speed
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q)) orbitSpeed.y -= 60.0f * ts;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::E)) orbitSpeed.y += 60.0f * ts;
	if (orbitSpeed.y < 0.0f) orbitSpeed.y = 0.0f;

	if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
		//											   v- Use yx to flip input as `x delta` should control y rotation and vice-versa 
		const auto mouseDelta = yx(mge::Input::MouseDelta());
		const auto mouseSensitivityDelta = mouseDelta * mouseSensitivity * ts;
		const auto deltaRotation = utils::constants::degToRad * glm::vec3(mouseSensitivityDelta * orbitSpeed, 0);

		targetAngleY += deltaRotation.y;
		targetAngleX += deltaRotation.x;
		// Would it be better to clamp the interpolated rotation instead? 
		if (targetAngleX < -utils::constants::deg45) targetAngleX = -utils::constants::deg45;
		if (targetAngleX > utils::constants::deg45) targetAngleX = utils::constants::deg45;

		// Wrap
		// if(targetRotation.y > utils::constants::pi - utils::constants::epsilon * 8.0f) targetRotation.y -= utils::constants::twoPi + utils::constants::epsilon * 8.0f;
		// if(targetRotation.y < -utils::constants::pi + utils::constants::epsilon * 8.0f) targetRotation.y += utils::constants::twoPi - utils::constants::epsilon * 8.0f;
	}

	///----------------------------------------------------------
	///						  UPDATE ZOOM
	///----------------------------------------------------------
	const auto scrollDelta = mge::Input::ScrollDelta();
	const auto scrollSensitivityDelta = scrollDelta * scrollSensitivity * ts;
	targetZoom += scrollSensitivityDelta;
	if (targetZoom > zoomRange.x) targetZoom = zoomRange.x;
	if (targetZoom < zoomRange.y) targetZoom = zoomRange.y;
}
