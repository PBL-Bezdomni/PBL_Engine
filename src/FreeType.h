#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <map>
#include "Shader.h"
struct Character {
	unsigned int textureID;
	glm::ivec2 Size;
	glm::ivec2 Bearing;
	unsigned int Advance;
};

class FreeType {
public:
	std::map<unsigned int, Character> Characters;
	unsigned int VAO, VBO;

	FreeType();
	bool Init(const char* fontPath, unsigned int fontSize);
	void RenderText(Shader& shader, std::wstring text, float x, float y, float scale, glm::vec3 color);
};