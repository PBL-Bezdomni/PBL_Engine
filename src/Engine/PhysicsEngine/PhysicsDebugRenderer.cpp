#include "PhysicsDebugRenderer.h"
#include "../../Shader.h"
#include <glm/gtc/type_ptr.hpp>

PhysicsDebugRenderer::PhysicsDebugRenderer()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6, NULL, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
}

PhysicsDebugRenderer::~PhysicsDebugRenderer()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

void PhysicsDebugRenderer::Setup(Shader& shader, const glm::mat4& view, const glm::mat4& projection)
{
    m_Shader = &shader;
    m_Shader->Use();
    m_Shader->SetMat4("view", view);
    m_Shader->SetMat4("projection", projection);
}

void PhysicsDebugRenderer::DrawLine(JPH::RVec3Arg inFrom, JPH::RVec3Arg inTo, JPH::ColorArg inColor)
{
    if (!m_Shader) return;

    glm::vec3 color(inColor.r / 255.0f, inColor.g / 255.0f, inColor.b / 255.0f);
    m_Shader->SetVec3("lineColor", color);

    float vertices[6] = {
        inFrom.GetX(), inFrom.GetY(), inFrom.GetZ(),
        inTo.GetX(),   inTo.GetY(),   inTo.GetZ()
    };

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

    glDrawArrays(GL_LINES, 0, 2);

    glBindVertexArray(0);
}