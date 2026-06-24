#include "LeaderBoard.h"
#include <locale>
#include <fstream>
#include <algorithm>

void LeaderBoard::Init(UIManager* uiManager, unsigned int panelTexture, const glm::vec2& position, const glm::vec2& size, float textScale)
{
	m_UIManager = uiManager;
	m_BackPanel.TextureID = panelTexture;
	m_BackPanel.Position = position;
	m_BackPanel.Size = size;
	m_BackPanel.HasTexture = true;
	m_BackPanel.Text = L"";
	m_TextScale = textScale;
	m_BackPanel.TextScale = textScale;
	m_BackPanel.TextColor = glm::vec3(0.9f, 0.9f, 0.9f);
	m_IsVisible = true;
}

void LeaderBoard::SetEntries(const std::vector<std::string>& entries)
{
	m_Entries.clear();
	m_Entries.reserve(entries.size());
	for (const auto& e : entries)
	{
		std::wstring w(e.begin(), e.end());
		m_Entries.push_back(w);
	}
}

void LeaderBoard::Draw(Shader& uiShader, Shader& textShader)
{
	if (!m_IsVisible || !m_UIManager)
		return;

	m_UIManager->DrawPanelWithText(uiShader, textShader, m_BackPanel);

	float startX = m_BackPanel.Position.x + m_EntryPadding.x;
	float startY = m_BackPanel.Position.y + m_EntryPadding.y;

	for (size_t i = 0; i < m_Entries.size(); ++i)
	{
		UIPanel entryPanel;
		entryPanel.HasTexture = false;
		entryPanel.Position = glm::vec2(startX, startY + i * (m_EntryHeight + m_EntryPadding.y));
		entryPanel.Size = glm::vec2(m_BackPanel.Size.x - 2.0f * m_EntryPadding.x, m_EntryHeight);
		entryPanel.Text = m_Entries[i];
		entryPanel.TextScale = m_TextScale;
		entryPanel.TextColor = glm::vec3(0.15f, 0.15f, 0.15f);

		m_UIManager->DrawPanelWithText(uiShader, textShader, entryPanel);
	}
}

void LeaderBoard::AddScore(const std::string& name, int score)
{
	m_Scores.emplace_back(name, score);
	std::sort(m_Scores.begin(), m_Scores.end(), [](const auto &a, const auto &b) { return a.second > b.second; });
	if (m_Scores.size() > 10) m_Scores.resize(10);
	m_Entries.clear();
	for (const auto &p : m_Scores)
	{
		std::string s = p.first + " - " + std::to_string(p.second);
		m_Entries.emplace_back(s.begin(), s.end());
	}
}

bool LeaderBoard::SaveToFile(const std::string& path)
{
	std::ofstream ofs(path);
	if (!ofs.is_open()) return false;
	for (auto &p : m_Scores)
	{
		ofs << p.first << "," << p.second << "\n";
	}
	ofs.close();
	m_FilePath = path;
	return true;
}

bool LeaderBoard::LoadFromFile(const std::string& path)
{
	m_Scores.clear();
	std::ifstream ifs(path);
	if (!ifs.is_open()) return false;

	std::string line;
	while (std::getline(ifs, line))
	{
		if (line.empty()) continue;

		auto pos = line.find(',');
		if (pos == std::string::npos) continue;

		std::string name = line.substr(0, pos);
		std::string scoreStr = line.substr(pos + 1);

		if (!scoreStr.empty() && scoreStr.back() == '\r') {
			scoreStr.pop_back();
		}

		try
		{
			int score = std::stoi(scoreStr);
			m_Scores.emplace_back(name, score);
		}
		catch (const std::exception& e)
		{
			std::cout << "Bezdomni_Engine WARNING: Skipping corrupted leaderboard line: " << line << "\n";
			continue;
		}
	}
	ifs.close();

	std::sort(m_Scores.begin(), m_Scores.end(), [](const auto& a, const auto& b) {
		return a.second > b.second;
		});

	if (m_Scores.size() > 10) {
		m_Scores.resize(10);
	}

	m_Entries.clear();
	m_Entries.reserve(m_Scores.size());
	for (const auto& p : m_Scores)
	{
		std::string s = p.first + " - " + std::to_string(p.second);

		std::wstring ws(s.begin(), s.end());
		m_Entries.push_back(ws);
	}

	m_FilePath = path;
	return true;
}

