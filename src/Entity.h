#pragma once
#include "Model.h"
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>

struct Transform
{
	glm::vec3 pos = { 0.0f, 0.0f, 0.0f };
	glm::vec3 eulerRot = { 0.0f, 0.0f, 0.0f };
	glm::vec3 scale = { 1.0f, 1.0f, 1.0f };

	glm::mat4 modelMatrix = glm::mat4(1.0f);

};

class Entity : public Model
{
public:
	Transform transform;
	Entity();
	Entity(Shader& shader);
	Entity(Shader& shader, vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures, unsigned int instancing = 1, vector<glm::mat4> instanceMatrix = {});
	Entity(Shader& shader, const char* path, unsigned int instancing = 1, vector<glm::mat4> instanceMatrix = {});
	list<Entity*> children;
	Entity* parent = nullptr;

	void updateSelfAndChild();
	void updateSelfAndChildInstanceMatrix(glm::vec3 transform, glm::vec3 rotation, glm::vec3 scale, int index, Shader& shader, bool saveNew);
	glm::mat4 getLocalModelMatrix();
	void addChild(Entity* child);
	void drawSelfAndChild();
	void drawSelf();
	void ReassignShader(Shader& shader);
	glm::vec3 GetWorldPosition();
	void SetModelRenderBool(bool render);
private:
	Shader m_Shader = Shader();
	bool RenderModel = true;
};