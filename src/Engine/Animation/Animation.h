#pragma once

#include <vector>
#include <map>
#include <string>
#include "Bone.h"
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <glm/glm.hpp>
#include "Model.h"

using namespace std;

struct AssimpNodeData
{
	glm::mat4 transformation;
	string name;
	int childrenCount;
	vector<AssimpNodeData> children;
};

class Animation
{
private:
	void ReadMissingBones(const aiAnimation* animation, Model& model);
	void ReadHierarchyData(AssimpNodeData& dest, const aiNode* src);

	float m_Duration;
	int m_TicksPerSecond;
	vector<Bone> m_Bones;
	AssimpNodeData m_RootNode;
	map<string, BoneInfo> m_BoneInfoMap;

public:
	Animation() = default;
	Animation(const string& animationPath, Model* model);

	Bone* FindBone(const string& name);

	inline float GetTicksPerSecond() { return m_TicksPerSecond; }
	inline float GetDuration() { return m_Duration; }
	inline const AssimpNodeData& GetRootNode() { return m_RootNode; }
	inline const map<string, BoneInfo>& GetBoneIDMap() { return m_BoneInfoMap; }
};