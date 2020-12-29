#pragma once
#include <string>

namespace game::config {
	const std::string MODEL_PATH("assets/game/models/");
	const std::string TEXTURE_PATH("assets/game/textures/");
	const std::string SHADER_PATH("assets/game/shaders/");
	const std::string FONT_PATH("assets/game/fonts/");

	inline std::string Model(const std::string& modelPath) { return MODEL_PATH + modelPath; }
	inline std::string Texture(const std::string& texturePath) { return TEXTURE_PATH + texturePath; }
	inline std::string Shader(const std::string& shaderPath) { return SHADER_PATH + shaderPath; }
	inline std::string Font(const std::string& fontPath) { return FONT_PATH + fontPath; }
}
