#pragma once

#include <vector>
#include <string>
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <assimp/scene.h>

using namespace std;

struct KeyPosition
{
	glm::vec3 position;
	float timeStamp;
};

struct KeyScale
{
	glm::vec3 scale;
	float timeStamp;
};

struct KeyRotation
{
	glm::quat orientation;
	float timeStamp;
};

class Bone
{
private:
	vector<KeyPosition> m_Positions;
	vector<KeyRotation> m_Rotations;
	vector<KeyScale> m_Scales;
	int m_NumPositions;
	int m_NumRotations;
	int m_NumScales;

	glm::mat4 m_LocalTransform;
	string m_Name;
	int m_ID;

	glm::mat4 InterpolatePosition(float animationTime);
	glm::mat4 InterpolateRotation(float animationTime);
	glm::mat4 InterpolateScaling(float animationTime);
	float GetScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime);

public:
	Bone(const string& name, int id, const aiNodeAnim* channel);

	void Update(float animationTime);
	glm::mat4 GetLocalTransform() const { return m_LocalTransform; }
	string GetBoneName() const { return m_Name; }
	int GetBoneID() const { return m_ID; }

	int GetPositionIndex(float animationTime);
	int GetRotationIndex(float animationTime);
	int GetScaleIndex(float animationTime);

};