#pragma once
#include <string>
#include <GL/glew.h>

#include "mge/core/ShaderProgram.hpp"


namespace utils::gl {
	inline void PassTexture(ShaderProgram* shader, const std::string& uniformString, int textureIndex, const GLuint textureId) {
		assert(textureIndex >= 0 && textureIndex <= 31);
	
		glActiveTexture(GL_TEXTURE0 + textureIndex);
		glBindTexture(GL_TEXTURE_2D, textureId);
		glUniform1i(shader->getUniformLocation(uniformString), textureIndex);
	}

	inline void PassTexture(const GLuint uniformLocation, int textureIndex, const GLuint textureId) {
		assert(textureIndex >= 0 && textureIndex <= 31);
	
		glActiveTexture(GL_TEXTURE0 + textureIndex);
		glBindTexture(GL_TEXTURE_2D, textureId);
		glUniform1i(uniformLocation, textureIndex);
	}
}
