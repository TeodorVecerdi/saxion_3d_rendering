#include "glm.hpp"

#include "LitColorMaterial.hpp"

#include "game/config.hpp"
#include "game/utils/constants.hpp"
#include "game/utils/string_utils.hpp"
#include "mge/core/Camera.hpp"
#include "mge/core/Light.hpp"
#include "mge/core/Mesh.hpp"
#include "mge/core/ShaderProgram.hpp"
#include "mge/core/World.hpp"

ShaderProgram* LitColorMaterial::_shader = NULL;

LitColorMaterial::LitColorMaterial(glm::vec3 baseColor): diffuseColor(baseColor), ambientColor(1, 1, 1, 0.1f), specularColor(1, 1, 1, 0.5f), shininess(256), eye(0) {
	//every time we create an instance of colormaterial we check if the corresponding shader has already been loaded
	_lazyInitializeShader();
}

void LitColorMaterial::_lazyInitializeShader() {
	//this shader contains everything the material can do (it can render something in 3d using a single color)
	if (!_shader) {
		_shader = new ShaderProgram();
		_shader->addShader(GL_VERTEX_SHADER, game::config::Shader("colorlit.vert"));
		_shader->addShader(GL_FRAGMENT_SHADER, game::config::Shader("colorlit_multiple.frag"));
		_shader->finalize();
	}
}

LitColorMaterial::~LitColorMaterial() {
	//dtor
}

void LitColorMaterial::SetDiffuseColor(glm::vec3 diffuseColor) {
	this->diffuseColor = diffuseColor;
}

void LitColorMaterial::SetAmbientColor(glm::vec3 ambientColor) {
	this->ambientColor = glm::vec4(ambientColor, this->ambientColor.w);
}

void LitColorMaterial::SetAmbientIntensity(float intensity) {
	ambientColor.w = intensity;
}

void LitColorMaterial::SetSpecularColor(glm::vec3 specularColor) {
	this->specularColor = glm::vec4(specularColor, this->specularColor.w);
}

void LitColorMaterial::SetSpecularIntensity(float intensity) {
	specularColor.w = intensity;
}

void LitColorMaterial::SetShininess(float shininess) {
	this->shininess = shininess;
}

void LitColorMaterial::render(World* world, Mesh* mesh, const glm::mat4& modelMatrix, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {
	_shader->use();

	eye = world->getMainCamera()->GetWorldPosition();

	const size_t lightCount = std::min(world->getLightCount(), LitColorMaterial::MAX_LIGHTS);
	for (size_t i = 0; i < lightCount; i++) {
		const LightData& ld = world->getLightAt(i)->GetLightData();
		const std::string lStr = utils::str::string_format("lights[%u]", i);
		glUniform1ui(_shader->getUniformLocation(lStr + ".type"), static_cast<uint32_t>(ld.type));
		glUniform3fv(_shader->getUniformLocation(lStr + ".direction"), 1, glm::value_ptr(ld.direction));
		glUniform3fv(_shader->getUniformLocation(lStr + ".position"), 1, glm::value_ptr(ld.position));
		glUniform4fv(_shader->getUniformLocation(lStr + ".ambient"), 1, glm::value_ptr(ld.ambient));
		glUniform4fv(_shader->getUniformLocation(lStr + ".diffuse"), 1, glm::value_ptr(ld.diffuse));
		glUniform4fv(_shader->getUniformLocation(lStr + ".specular"), 1, glm::value_ptr(ld.specular));
		glUniform3fv(_shader->getUniformLocation(lStr + ".attenuation"), 1, glm::value_ptr(ld.attenuation));
		glUniform1f(_shader->getUniformLocation(lStr + ".innerCutOff"), ld.innerCutOff);
		glUniform1f(_shader->getUniformLocation(lStr + ".outerCutOff"), ld.outerCutOff);
	}
	glUniform1ui(_shader->getUniformLocation("lightCount"), lightCount);

	glUniform3fv(_shader->getUniformLocation("material.diffuse"), 1, glm::value_ptr(diffuseColor));
	glUniform4fv(_shader->getUniformLocation("material.specular"), 1, glm::value_ptr(specularColor));
	glUniform1f(_shader->getUniformLocation("material.shininess"), shininess);

	glUniform3fv(3, 1, glm::value_ptr(eye));

	//pass in all MVP matrices separately
	glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	glUniformMatrix4fv(1, 1, GL_FALSE, glm::value_ptr(viewMatrix));
	glUniformMatrix4fv(2, 1, GL_FALSE, glm::value_ptr(modelMatrix));

	//now inform mesh of where to stream its data
	mesh->streamToOpenGL(
		0, 1, 2
	);

}
