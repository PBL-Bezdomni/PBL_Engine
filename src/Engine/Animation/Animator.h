#pragma once

#include <vector>
#include <string>
#include "Animation.h"
#include <map>
#include "../Components/Component.h"
#include <glm/glm.hpp>

class Animator : public Component
{
private:
	void CalculateBoneTransform(const AssimpNodeData* node, const glm::mat4& parentTransform);

	std::vector<glm::mat4> m_FinalBoneMatrices;
	Animation* m_CurrentAnimation;
	float m_CurrentTime;
	float m_DeltaTime;

	glm::mat4 m_GlobalInverseTransform;
	std::map<std::string, Animation*> m_Animations;

public:
	Animator();
	Animator(Animation* animation);

	void UpdateAnimation(float dt);
	void PlayAnimation(const std::string& name);
	void AddAnimation(const std::string& name, Animation* animation);

	const std::vector<glm::mat4>& GetFinalBoneMatrices() const { return m_FinalBoneMatrices; }
};