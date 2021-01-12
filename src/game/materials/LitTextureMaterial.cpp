#include "glm.hpp"

#include "LitTextureMaterial.hpp"

#include "game/config.hpp"
#include "game/utils/constants.hpp"
#include "game/utils/math_utils.hpp"
#include "game/utils/string_utils.hpp"
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
		_shader->addShader(GL_FRAGMENT_SHADER, game::config::Shader("texturelit_multiple.frag"));
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

	eye = world->getMainCamera()->getWorldPosition();

	const size_t lightCount = std::min(world->getLightCount(), LitTextureMaterial::MAX_LIGHTS);
	for(size_t i = 0; i < lightCount; i++) {
		const LightData ld = world->getLightAt(i)->GetLightData();
		const std::string lStr = utils::str::string_format("lights[%u]", i);
		glUniform1ui(_shader->getUniformLocation(lStr + ".type"), static_cast<uint32_t>(ld.type));
		glUniform3fv(_shader->getUniformLocation(lStr + ".direction"), 1, glm::value_ptr(ld.direction));
		glUniform3fv(_shader->getUniformLocation(lStr + ".position"), 1, glm::value_ptr(ld.position));
		glUniform4fv(_shader->getUniformLocation(lStr + ".ambient"), 1, glm::value_ptr(ld.ambient));
		glUniform3fv(_shader->getUniformLocation(lStr + ".diffuse"), 1, glm::value_ptr(ld.diffuse));
		glUniform3fv(_shader->getUniformLocation(lStr + ".specular"), 1, glm::value_ptr(ld.specular));
		glUniform3fv(_shader->getUniformLocation(lStr + ".attenuation"), 1, glm::value_ptr(ld.attenuation));
		glUniform1f(_shader->getUniformLocation(lStr + ".innerCutOff"), ld.innerCutOff);
		glUniform1f(_shader->getUniformLocation(lStr + ".outerCutOff"), ld.outerCutOff);
	}
	glUniform1ui(_shader->getUniformLocation("lightCount"), lightCount);

	glUniform4fv(_shader->getUniformLocation("material.specular"), 1, glm::value_ptr(specularColor));
	glUniform1f(_shader->getUniformLocation("material.shininess"), shininess);


	// Base
	//setup texture slot 0
	glActiveTexture(GL_TEXTURE0);
	//bind the texture to the current active slot
	glBindTexture(GL_TEXTURE_2D, texture->getId());
	//tell the shader the texture slot for the diffuse texture is slot 0
	glUniform1i(_shader->getUniformLocation("material.diffuse"), 0);
	
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
