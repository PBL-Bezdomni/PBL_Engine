#include "FreeType.h"
#include <iostream>
#include <freetype/freetype.h>

FreeType::FreeType() {
	VAO = 0;
	VBO = 0;
}

bool FreeType::Init(const char* fontPath, unsigned int fontSize) {
	FT_Library ft;
	if (FT_Init_FreeType(&ft))
	{
		spdlog::error("FREETYPE: Nie uda³o siê zainicjowaæ bibliteki FreeType");
		return false;
	}

	FT_Face face;
	if (FT_New_Face(ft, fontPath, 0, &face))
	{
		spdlog::error("FREETYPE: Nie uda³o siê za³adowaæ czcionki.");
		return false;
	}
	
	FT_Set_Pixel_Sizes(face, 0, fontSize);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	for (unsigned int c = 0; c < 384; c++)
	{
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
		{
			spdlog::error("FREETYPE: Nie uda³o siê za³adowaæ glifu.");
			continue;
		}

		unsigned int texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width, face->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		Character character = {
			texture,
			glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
			glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
			static_cast<unsigned int>(face->glyph->advance.x)
		};
		Characters.insert(std::pair<unsigned int, Character>(c, character));
	}
	glBindTexture(GL_TEXTURE_2D, 0);

	FT_Done_Face(face);
	FT_Done_FreeType(ft);

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	return true;
}

void FreeType::RenderText(Shader& shader, std::wstring text, float x, float y, float scale, glm::vec3 color) {
	shader.Use();
	glUniform3f(glGetUniformLocation(shader.ID, "textColor"), color.x, color.y, color.z);
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(VAO);

	std::wstring::const_iterator c;
	for (c = text.begin(); c != text.end(); c++) 
	{
		Character ch = Characters[*c];

		float xpos = x + ch.Bearing.x * scale;
		float ypos = y - ch.Bearing.y * scale;

		float w = ch.Size.x * scale;
		float h = ch.Size.y * scale;

		float vertices[6][4] = {
			{ xpos,		ypos,	  0.0f, 0.0f },
			{ xpos,		ypos + h, 0.0f, 1.0f },
			{ xpos + w, ypos + h, 1.0f, 1.0f },

			{ xpos,		ypos,	  0.0f, 0.0f },
			{ xpos + w, ypos + h, 1.0f, 1.0f },
			{ xpos + w, ypos,	  1.0f, 0.0f }
		};

		glBindTexture(GL_TEXTURE_2D, ch.textureID);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glDrawArrays(GL_TRIANGLES, 0, 6);
		x += (ch.Advance >> 6) * scale;
	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}
float FreeType::GetTextWidth(std::wstring text, float scale)
{
	float width = 0.0f;
	std::wstring::const_iterator c;
	for (c = text.begin(); c != text.end(); c++)
	{
		Character ch = Characters[*c];
		width += (ch.Advance >> 6) * scale;
	}
	return width;
}