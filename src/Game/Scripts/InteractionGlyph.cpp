#include "InteractionGlyph.h"

#include "Engine/Engine.h"
#include "Engine/Time.h"

void InteractionGlyph::Awake()
{
	Component::Awake();
}

void InteractionGlyph::Update()
{
	Component::Update();

	m_Timer += Time::GetDeltaTime();
	if (m_Timer >= DURATION)
	{
		m_Sign *= -1;
		if (m_Sign < 0)
		{
			m_Min = Y_POS - OFFSET;
			m_Max = Y_POS + OFFSET;
		}
		else
		{
			m_Min = Y_POS + OFFSET;
			m_Max = Y_POS - OFFSET;
		}
		m_Timer = 0.f;
	}

	float y = lerp(m_Min, m_Max, m_Timer / DURATION);

	m_Owner->transform->Position.y = y;
}

void InteractionGlyph::DrawUpdate()
{
	Component::DrawUpdate();
	m_InteractionMarkShader->Use();
	// m_InteractionMarkShader->SetFloat("u_width", 0.5f);
	// m_InteractionMarkShader->SetFloat("u_height", 0.5f);
	m_InteractionMarkShader->SetVec3("cameraRight", m_MainCamera->GetRight());
	m_InteractionMarkShader->SetVec3("cameraUp", m_MainCamera->GetUp());
}

void InteractionGlyph::Initialize(std::shared_ptr<Shader> shader, Texture tex)
{
	m_InteractionMarkShader = shader;
	m_MainCamera = Engine::GetInstance().GetGameManager().GetSceneManager().GetMainCamera().get();
	m_Owner->Name = "InteractionMark";
	m_Owner->transform->Position = glm::vec3(0.f, Y_POS, 0.f);
	m_Owner->transform->EulerAngles = glm::vec3(90.0f, 0.0f, 0.0f);
	m_Owner->transform->Scale = glm::vec3(1.0f, 1.0f, 1.0f);
	Model* interactionModel = m_Owner->GetComponent<Model>();
	if (interactionModel != nullptr)
	{
		// interactionModel->ReassignShader(*m_AssetMgr->WorldUIShader);
		interactionModel->AssignTexture(tex);
	}
	m_Owner->SetActive(false);
	m_InteractionMarkShader->Use();
	m_InteractionMarkShader->SetFloat("u_width", 1.f);
	m_InteractionMarkShader->SetFloat("u_height", 1.f);
}


