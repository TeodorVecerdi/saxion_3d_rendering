#pragma once

namespace mge {
	class Time {
		friend class AbstractGame;

	public: //static
		/// Returns the total time in seconds since the last frame
		[[nodiscard]] static float DeltaTime() { return deltaTime; }
		/// Returns the total time in seconds since the program started
		[[nodiscard]] static float TotalTime() { return time; }

	private: //static
		inline static float deltaTime = 0.0f;
		inline static float time = 0.0f;
		static void Update(float deltaTime) {
			Time::deltaTime = deltaTime;
			time += deltaTime;
		}
	};
}
