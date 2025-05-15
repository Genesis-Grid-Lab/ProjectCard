#pragma once

#include <UrbanEngine.h>
#include "Panels/SceneHierarchyPanel.h"
#include "Panels/ContentBrowserPanel.h"
#include "ImGui/Console.h"

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

    void NewScene();
    void OpenScene();
    void OpenScene(const std::filesystem::path& path);
    void SaveScene();
    void SaveSceneAs();

    void SerializeScene(Ref<Scene> scene, const std::filesystem::path& path);
private:
    glm::vec2 m_Size;
    bool m_ViewportFocused = false, m_ViewportHovered = false;
    glm::vec2 m_ViewportBounds[2];
    Ref<Scene> m_ActiveScene;
    Ref<Scene> m_EditorScene;
    glm::vec2 m_ViewportSize = { 0.0f, 0.0f };
    Entity m_SelectionContext;
    EditorCamera m_EditorCamera;
    int m_GizmoType = -1;

    std::filesystem::path m_EditorScenePath;
    AppConsole console;

    // Panels
    SceneHierarchyPanel m_SceneHierarchyPanel;
    ContentBrowserPanel m_ContentBrowserPanel;
};