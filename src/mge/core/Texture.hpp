#pragma once

#include <string>
#include <GL/glew.h>



class Texture
{
	public:
		enum UV_WRAP;
	
		static Texture* load(const std::string& pTexturePath, UV_WRAP wrap);
		static Texture* load(const std::string& pTexturePath);

		GLuint getId();

		enum UV_WRAP {
			REPEAT = GL_REPEAT,
			CLAMP = GL_CLAMP_TO_EDGE,
			CLAMP_TO_BORDER = GL_CLAMP_TO_BORDER,
			MIRROR_REPEAT = GL_MIRRORED_REPEAT
		};

	protected:
	    Texture();
		virtual ~Texture();

        static Texture* _loadFromFile(const std::string pTexturePath);

	    //OpenGL id for texture buffer
		GLuint _id;
};
