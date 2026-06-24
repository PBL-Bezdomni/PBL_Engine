#pragma once

#include "UIManager.h"
#include "Shader.h"
#include <vector>
#include <string>
#include <glm/glm.hpp>

class LeaderBoard
{
public:
	LeaderBoard() = default;
	~LeaderBoard() = default;

	void Init(UIManager* uiManager, unsigned int panelTexture, const glm::vec2& position, const glm::vec2& size, float textScale = 1.0f);

	void SetEntries(const std::vector<std::string>& entries);
	void AddScore(const std::string& name, int score);
	bool SaveToFile(const std::string& path);
	bool LoadFromFile(const std::string& path);

	void Draw(Shader& uiShader, Shader& textShader);

	void Show(bool visible) { m_IsVisible = visible; }
	bool IsVisible() const { return m_IsVisible; }

private:
	UIManager* m_UIManager = nullptr;
	UIPanel m_BackPanel;
	std::vector<std::pair<std::string,int>> m_Scores;
	std::vector<std::wstring> m_Entries;
	float m_TextScale = 1.0f;
	glm::vec2 m_EntryPadding = glm::vec2(10.0f, 10.0f);
	float m_EntryHeight = 30.0f;
	bool m_IsVisible = false;
	std::string m_FilePath;
};
