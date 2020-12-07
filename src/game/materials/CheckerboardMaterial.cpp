#include "CheckerboardMaterial.h"

#include "game/config.hpp"
#include "mge/config.hpp"
#include "mge/core/Mesh.hpp"
#include "mge/core/ShaderProgram.hpp"
ShaderProgram* CheckerboardMaterial::_shader = nullptr;

CheckerboardMaterial::CheckerboardMaterial(float patternSize) : patternSize(patternSize) {
	_lazyInitializeShader();
}

CheckerboardMaterial::~CheckerboardMaterial() {
}

void CheckerboardMaterial::render(World* pWorld, Mesh* pMesh, const glm::mat4& pModelMatrix,
	const glm::mat4& pViewMatrix, const glm::mat4& pProjectionMatrix) {

	_shader->use();
	glUniform1f(_shader->getUniformLocation("checkerboardScale"), patternSize);
	//pass in all MVP matrices separately
	glUniformMatrix4fv ( _shader->getUniformLocation("projectionMatrix"),   1, GL_FALSE, glm::value_ptr(pProjectionMatrix));
	glUniformMatrix4fv ( _shader->getUniformLocation("viewMatrix"),         1, GL_FALSE, glm::value_ptr(pViewMatrix));
	glUniformMatrix4fv ( _shader->getUniformLocation("modelMatrix"),        1, GL_FALSE, glm::value_ptr(pModelMatrix));

	//now inform mesh of where to stream its data
	pMesh->streamToOpenGL(
        _shader->getAttribLocation("vertex"),
        _shader->getAttribLocation("normal"),
        _shader->getAttribLocation("uv")
    );
}

void CheckerboardMaterial::setPatternSize(float patternSize) {
	this->patternSize = patternSize;
}

float CheckerboardMaterial::getPatternSize() const {
	return patternSize;
}


void CheckerboardMaterial::_lazyInitializeShader() {
	if (!_shader) {
		_shader = new ShaderProgram();
		_shader->addShader(GL_VERTEX_SHADER, config::game::SHADER_PATH+"checkerboard.vert");
		_shader->addShader(GL_FRAGMENT_SHADER, config::game::SHADER_PATH+"checkerboard.frag");
		_shader->finalize();
	}
}
