#include "Bone.h"

static inline glm::vec3 GetGLMVec(const aiVector3D& vec) 
{
	return glm::vec3(vec.x, vec.y, vec.z);
}

static inline glm::quat GetGLMQuat(const aiQuaternion& quat) 
{
	return glm::quat(quat.w, quat.x, quat.y, quat.z);
}

Bone::Bone(const string& name, int id, const aiNodeAnim* channel)
	: m_Name(name), m_ID(id), m_LocalTransform(1.0f)
{
	m_NumPositions = channel->mNumPositionKeys;
	for (int positionIndex = 0; positionIndex < m_NumPositions; ++positionIndex) 
	{
		KeyPosition data;
		data.position = GetGLMVec(channel->mPositionKeys[positionIndex].mValue);
		data.timeStamp = channel->mPositionKeys[positionIndex].mTime;
		m_Positions.push_back(data);
	}

	m_NumRotations = channel->mNumRotationKeys;
	for (int rotationIndex = 0; rotationIndex < m_NumRotations; ++rotationIndex) 
	{
		KeyRotation data;
		data.orientation = GetGLMQuat(channel->mRotationKeys[rotationIndex].mValue);
		data.timeStamp = channel->mRotationKeys[rotationIndex].mTime;
		m_Rotations.push_back(data);
	}

	m_NumScales = channel->mNumScalingKeys;
	for (int scaleIndex = 0; scaleIndex < m_NumScales; ++scaleIndex) 
	{
		KeyScale data;
		data.scale = GetGLMVec(channel->mScalingKeys[scaleIndex].mValue);
		data.timeStamp = channel->mScalingKeys[scaleIndex].mTime;
		m_Scales.push_back(data);
	}
}

int Bone::GetPositionIndex(float animationTime)
{
	for (int index = 0; index < m_NumPositions - 1; ++index)
	{
		if (animationTime < m_Positions[index + 1].timeStamp)
		{
			return index;
		}
	}
	assert(0);
	return 0;
}

int Bone::GetRotationIndex(float animationTime)
{
	for (int index = 0; index < m_NumRotations - 1; ++index)
	{
		if (animationTime < m_Rotations[index + 1].timeStamp)
		{
			return index;
		}
	}
	assert(0);
	return 0;
}

int Bone::GetScaleIndex(float animationTime)
{
	for (int index = 0; index < m_NumScales - 1; ++index)
	{
		if (animationTime < m_Scales[index + 1].timeStamp)
		{
			return index;
		}
	}
	assert(0);
	return 0;
}

float Bone::GetScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime)
{
	float scaleFactor = 0.0f;
	float midWayLength = animationTime - lastTimeStamp;
	float framesDiff = nextTimeStamp - lastTimeStamp;
	scaleFactor = midWayLength / framesDiff;
	return scaleFactor;
}

glm::mat4 Bone::InterpolatePosition(float animationTime)
{
	if (m_NumPositions == 1)
	{
		return glm::translate(glm::mat4(1.0f), m_Positions[0].position);
	}

	int p0Index = GetPositionIndex(animationTime);
	int p1Index = p0Index + 1;
	float scaleFactor = GetScaleFactor(m_Positions[p0Index].timeStamp, m_Positions[p1Index].timeStamp, animationTime);
	glm::vec3 finalPosition = glm::mix(m_Positions[p0Index].position, m_Positions[p1Index].position, scaleFactor);
	return glm::translate(glm::mat4(1.0f), finalPosition);
}

glm::mat4 Bone::InterpolateRotation(float animationTime)
{
	if (m_NumRotations == 1)
	{
		auto rotation = glm::normalize(m_Rotations[0].orientation);
		return glm::toMat4(rotation);
	}

	int p0Index = GetRotationIndex(animationTime);
	int p1Index = p0Index + 1;
	float scaleFactor = GetScaleFactor(m_Rotations[p0Index].timeStamp, m_Rotations[p1Index].timeStamp, animationTime);
	glm::quat finalRotation = glm::slerp(m_Rotations[p0Index].orientation, m_Rotations[p1Index].orientation, scaleFactor);
	finalRotation = glm::normalize(finalRotation);
	return glm::toMat4(finalRotation);
}

glm::mat4 Bone::InterpolateScaling(float animationTime)
{
	if (m_NumScales == 1)
	{
		return glm::scale(glm::mat4(1.0f), m_Scales[0].scale);
	}

	int p0Index = GetScaleIndex(animationTime);
	int p1Index = p0Index + 1;
	float scaleFactor = GetScaleFactor(m_Scales[p0Index].timeStamp, m_Scales[p1Index].timeStamp, animationTime);
	glm::vec3 finalScale = glm::mix(m_Scales[p0Index].scale, m_Scales[p1Index].scale, scaleFactor);
	return glm::scale(glm::mat4(1.0f), finalScale);
}

void Bone::Update(float animationTime)
{
	glm::mat4 translation = InterpolatePosition(animationTime);
	glm::mat4 rotation = InterpolateRotation(animationTime);
	glm::mat4 scale = InterpolateScaling(animationTime);
	m_LocalTransform = translation * rotation * scale;
}