#pragma once
#include <iostream>
#include <glm/vec2.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

namespace mge {
	/// Provides a global/static Input getter
	/// Provides mouse position, delta and also allows mouse locking (and hiding)
	class Input {
		friend class AbstractGame;

	public: //static
		static glm::vec2 ClampedMousePosition() {
			if (instance == nullptr) {
				std::cerr << "Input instance not created. Make sure to create an Input instance.\n";
				return glm::vec2(-1);
			}
			return mouseLock ? viewportCenter : clampedMousePosition;
		}

		static glm::vec2 MousePosition() {
			if (instance == nullptr) {
				std::cerr << "Input instance not created. Make sure to create an Input instance.\n";
				return glm::vec2(-1);
			}
			return mouseLock ? viewportCenter : mousePosition;
		}

		static glm::vec2 MouseDelta() {
			if (instance == nullptr) {
				std::cerr << "Input instance not created. Make sure to create an Input instance.\n";
				return glm::vec2(0);
			}
			return mouseDelta;
		}

		static void SetMouseLock(const bool mouseLock) {
			if (instance == nullptr) {
				std::cerr << "Input instance not created. Make sure to create an Input instance.\n";
				return;
			}

			// Reset position if activating mouse lock
			if (mouseLock != Input::mouseLock) {
				if (mouseLock) {
					mouseDelta = glm::vec2(0);
					const auto sfViewportCenter = sf::Vector2i(static_cast<int>(viewportCenter.x), static_cast<int>(viewportCenter.y));
					sf::Mouse::setPosition(sfViewportCenter, *instance->windowReference);
				}
				instance->SetMouseVisible(!mouseLock);
			}

			Input::mouseLock = mouseLock;
		}

	private:
		explicit Input(sf::RenderWindow* window) {
			if (instance != nullptr) throw std::exception("Cannot create more than one `Input` instance");

			instance = this;
			windowReference = window;
		}

		~Input() {
		}

		void FixMouseToCenter(const glm::vec2 viewportCenter) {
			const auto sfViewportCenter = sf::Vector2i(static_cast<int>(viewportCenter.x), static_cast<int>(viewportCenter.y));
			sf::Mouse::setPosition(sfViewportCenter, *windowReference);
		}

		void SetMouseVisible(const bool visible) {
			windowReference->setMouseCursorVisible(visible);
		}

		sf::RenderWindow* windowReference;
	private: //static
		static void OnViewportResized(const uint32_t width, const uint32_t height) {
			if (instance == nullptr) {
				std::cerr << "Input instance not created. Make sure to create an Input instance.\n";
				return;
			}
			viewportSize.x = static_cast<float>(width);
			viewportSize.y = static_cast<float>(height);
			viewportCenter = viewportSize / 2;
		}

		static void Update(const int mouseX, const int mouseY) {
			if (instance == nullptr) {
				std::cerr << "Input instance not created. Make sure to create an Input instance.\n";
				return;
			}

			const auto newMousePos = glm::vec2(mouseX, mouseY);
			auto newClampedPosition = newMousePos;
			newClampedPosition.x = glm::clamp(newClampedPosition.x, 0.0f, viewportSize.x);
			newClampedPosition.y = glm::clamp(newClampedPosition.y, 0.0f, viewportSize.y);

			if (mouseLock) {
				mouseDelta = glm::vec2(mouseX, mouseY) - viewportCenter;
				instance->FixMouseToCenter(viewportCenter);
			} else {
				mouseDelta = newClampedPosition - clampedMousePosition;
			}

			mousePosition = newMousePos;
			clampedMousePosition = newClampedPosition;
		}

		/// @brief Resets `mousePosition` to `mouseX` and `mouseY`, and resets delta
		/// @param mouseX The x coordinate of the mouse position to reset to
		/// @param mouseY The y coordinate of the mouse position to reset to
		static void ResetAll(int mouseX = 0, int mouseY = 0) {
			mousePosition = glm::vec2(mouseX, mouseY);
			clampedMousePosition = glm::vec2(mouseX, mouseY);
			ResetDelta();
		}

		/// @brief Resets delta position
		static void ResetDelta() {
			if (instance == nullptr) {
				std::cerr << "Input instance not created. Make sure to create an Input instance.\n";
				return;
			}
			mouseDelta = glm::vec2(0);
		}

		// Use floating point vectors for easier math calculations, even though mouse position is always int
		inline static glm::vec2 mousePosition = glm::vec2(0);
		inline static glm::vec2 clampedMousePosition = glm::vec2(0);
		inline static glm::vec2 mouseDelta = glm::vec2(0);

		inline static glm::vec2 viewportSize = glm::vec2(0);
		inline static glm::vec2 viewportCenter = glm::vec2(0);

		inline static bool mouseLock = false;
		inline static Input* instance = nullptr;

	};
}
