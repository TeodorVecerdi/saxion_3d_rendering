#include "glm.hpp"

#include "LitTextureMaterial.hpp"


#include "game/config.hpp"
#include "game/utils/constants.hpp"
#include "mge/core/Camera.hpp"
#include "mge/core/Light.hpp"
#include "mge/core/Mesh.hpp"
#include "mge/core/ShaderProgram.hpp"
#include "mge/core/Texture.hpp"
#include "mge/core/World.hpp"

ShaderProgram* LitTextureMaterial::_shader = NULL;

LitTextureMaterial::LitTextureMaterial(Texture* texture): texture(texture), ambientColor(1,1,1,0.1f), specularColor(1,1,1,0.5f), shininess(256), eye(0) {
	//every time we create an instance of colormaterial we check if the corresponding shader has already been loaded
	_lazyInitializeShader();
}

void LitTextureMaterial::_lazyInitializeShader() {
	//this shader contains everything the material can do (it can render something in 3d using a single color)
	if (!_shader) {
		_shader = new ShaderProgram();
		_shader->addShader(GL_VERTEX_SHADER, game::config::Shader("texturelit.vert"));
		_shader->addShader(GL_FRAGMENT_SHADER, game::config::Shader("texturelit.frag"));
		_shader->finalize();
	}
}

LitTextureMaterial::~LitTextureMaterial() {
	//dtor
}

void LitTextureMaterial::SetTexture(Texture* texture) {
	this->texture = texture;
}

void LitTextureMaterial::SetAmbientColor(glm::vec3 ambientColor) {
	this->ambientColor = glm::vec4(ambientColor, this->ambientColor.w);
}

void LitTextureMaterial::SetAmbientIntensity(float intensity) {
	ambientColor.w = intensity;
}

void LitTextureMaterial::SetSpecularColor(glm::vec3 specularColor) {
	this->specularColor = glm::vec4(specularColor, this->specularColor.w);
}

void LitTextureMaterial::SetSpecularIntensity(float intensity) {
	specularColor.w = intensity;
}

void LitTextureMaterial::SetShininess(float shininess) {
	this->shininess = shininess;
}

void LitTextureMaterial::render(World* world, Mesh* mesh, const glm::mat4& modelMatrix, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {
	_shader->use();

	LightData lightData = {};
	if (world->getLightCount() > 0) {
		auto* light = world->getLightAt(0);
		lightData = light->GetLightData();
	}

	eye = world->getMainCamera()->getWorldPosition();

	// Base
	//setup texture slot 0
	glActiveTexture(GL_TEXTURE0);
	//bind the texture to the current active slot
	glBindTexture(GL_TEXTURE_2D, texture->getId());
	//tell the shader the texture slot for the diffuse texture is slot 0
	glUniform1i(4, 0);
	
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
