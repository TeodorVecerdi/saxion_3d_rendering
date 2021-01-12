#include "glm.hpp"

#include "LitColorMaterial.hpp"


#include "game/config.hpp"
#include "game/utils/constants.hpp"
#include "mge/core/Camera.hpp"
#include "mge/core/Light.hpp"
#include "mge/core/Mesh.hpp"
#include "mge/core/ShaderProgram.hpp"
#include "mge/core/World.hpp"

ShaderProgram* LitColorMaterial::_shader = NULL;

LitColorMaterial::LitColorMaterial(glm::vec3 baseColor): baseColor(baseColor), ambientColor(1,1,1,0.1f), specularColor(1,1,1,0.5f), shininess(256), eye(0) {
	//every time we create an instance of colormaterial we check if the corresponding shader has already been loaded
	_lazyInitializeShader();
}

void LitColorMaterial::_lazyInitializeShader() {
	//this shader contains everything the material can do (it can render something in 3d using a single color)
	if (!_shader) {
		_shader = new ShaderProgram();
		_shader->addShader(GL_VERTEX_SHADER, game::config::Shader("colorlit.vert"));
		_shader->addShader(GL_FRAGMENT_SHADER, game::config::Shader("colorlit.frag"));
		_shader->finalize();
	}
}

LitColorMaterial::~LitColorMaterial() {
	//dtor
}

void LitColorMaterial::SetBaseColor(glm::vec3 baseColor) {
	this->baseColor = baseColor;
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

	LightData lightData = {};
	if (world->getLightCount() > 0) {
		auto* light = world->getLightAt(0);
		lightData = light->GetLightData();
	}

	eye = world->getMainCamera()->getWorldPosition();

	// Base
	glUniform3fv(4, 1, value_ptr(baseColor));
	glUniform4fv(5, 1, value_ptr(ambientColor));
	// Specular
	glUniform4fv(6, 1, value_ptr(specularColor));
	glUniform1f(7, shininess);
	glUniform3fv(8, 1, value_ptr(eye));
	// Light Data
	glUniform1ui(9, static_cast<uint32_t>(lightData.lightType));
	glUniform3fv(10, 1, value_ptr(lightData.lightPosition));
	glUniform3fv(11, 1, value_ptr(lightData.lightDirection));
	glUniform3fv(12, 1, value_ptr(lightData.lightColor));
	glUniform3fv(13, 1, value_ptr(lightData.attenuation));
	glUniform1f(14, lightData.outerAngle);
	glUniform1f(15, lightData.innerAngle);

	// fix non-uniform scaling (normal matrix)
	auto tiModelMatrix = glm::mat3(glm::transpose(glm::inverse(modelMatrix)));

	//pass in all MVP matrices separately
	glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	glUniformMatrix4fv(1, 1, GL_FALSE, glm::value_ptr(viewMatrix));
	glUniformMatrix4fv(2, 1, GL_FALSE, glm::value_ptr(modelMatrix));
	glUniformMatrix3fv(3, 1, GL_FALSE, glm::value_ptr(tiModelMatrix));

	//now inform mesh of where to stream its data
	mesh->streamToOpenGL(
		0, 1, 2
	);

}
