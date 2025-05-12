#pragma once

#include <UrbanEngine.h>

using namespace UE;

class EditorLayer : public Layer {
public:
    EditorLayer(const glm::vec2& size);

    virtual void OnAttach() override;
    virtual void OnUpdate(Timestep ts) override;
    virtual void OnEvent(Event& e) override;
    virtual void OnImGuiRender() override;
private:
    void HierarachyPanel();
    void ContentBrowserPanel();
private:
    glm::vec2 m_Size;
};