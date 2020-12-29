#pragma once
#include <glm/vec2.hpp>

namespace mge {
	/// Provides a global/static Input getter
	/// Currently only for mouse position and delta as other functionality is already provided by SFML
	class Input {
	public:
		static glm::vec2 ClampedMousePosition() {
			return clampedMousePosition;
		}

		static glm::vec2 MousePosition() {
			return mousePosition;
		}

		static glm::vec2 MouseDelta() {
			return mouseDelta;
		}

	private:
		Input() {
		}

		~Input() {
		}

		static void OnViewportResized(const uint32_t width, const uint32_t height) {
			viewportSize.x = width;
			viewportSize.y = height;
		}

		static void Update(const int mouseX, const int mouseY) {
			const auto newMousePos = glm::vec2(mouseX, mouseY);
			mousePosition = newMousePos;

			auto newClampedPosition = newMousePos;
			newClampedPosition.x = glm::clamp(newClampedPosition.x, 0.0f, viewportSize.x);
			newClampedPosition.y = glm::clamp(newClampedPosition.y, 0.0f, viewportSize.y);
			mouseDelta = newClampedPosition - clampedMousePosition;

			clampedMousePosition = newClampedPosition;
		}

		static void ResetDelta() {
			mouseDelta = glm::vec2(0);
		}

		// Use floating point vectors for easier math calculations, even though mouse position is always int
		inline static glm::vec2 mousePosition = glm::vec2(0);
		inline static glm::vec2 clampedMousePosition = glm::vec2(0);
		inline static glm::vec2 mouseDelta = glm::vec2(0);
		inline static glm::vec2 viewportSize = glm::vec2(0);

		friend class AbstractGame;
	};
}
