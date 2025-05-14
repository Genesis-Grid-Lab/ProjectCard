#pragma once

#include <UrbanEngine.h>
#include "Panels/SceneHierarchyPanel.h"
#include "Panels/ContentBrowserPanel.h"

using namespace UE;

class EditorLayer : public Layer {
public:
    EditorLayer(const glm::vec2& size);

    virtual void OnAttach() override;
    virtual void OnUpdate(Timestep ts) override;
    virtual void OnEvent(Event& e) override;
    virtual void OnImGuiRender() override;
private:
    bool OnKeyPressed(KeyPressedEvent& e);
    bool OnMouseButtonPressed(MouseButtonPressedEvent& e);
private:
    glm::vec2 m_Size;
    bool m_ViewportFocused = false, m_ViewportHovered = false;
    glm::vec2 m_ViewportBounds[2];
    Ref<Scene> m_ActiveScene;
    glm::vec2 m_ViewportSize = { 0.0f, 0.0f };
    Entity m_SelectionContext;
    EditorCamera m_EditorCamera;
    int m_GizmoType = -1;

    // Panels
    SceneHierarchyPanel m_SceneHierarchyPanel;
    ContentBrowserPanel m_ContentBrowserPanel;
};