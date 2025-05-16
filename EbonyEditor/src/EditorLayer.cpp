#include "EditorLayer.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

EditorLayer::EditorLayer(const glm::vec2& size): m_Size(size) {}

void EditorLayer::OnAttach(){
    console.AddLog("Starting");
    m_IconPlay = Texture2D::Create("Resources/Icons/PlayButton.png");
    m_IconStop = Texture2D::Create("Resources/Icons/StopButton.png");
    m_ActiveScene = CreateRef<Scene>(m_Size.x, m_Size.y);
    m_SceneHierarchyPanel = SceneHierarchyPanel(m_ActiveScene);
    m_EditorCamera = EditorCamera(30.0f, 1.778f, 0.1f, 2000.0f);

    auto commandLineArgs = Application::Get().GetCommandLineArgs();
    if(commandLineArgs.Count > 1){

        auto sceneFilePath = commandLineArgs[1];
        SceneSerializer serializer(m_ActiveScene);
        serializer.Deserialize(sceneFilePath);
    }

    console.AddLog("Loading Resources");
    // Ref<Model> castle = CreateRef<Model>("Resources/sponza/sponza.obj");   
    Ref<Model> sphere = CreateRef<Model>("Resources/sphere.fbx");

    console.AddLog("Creating entity");
    auto& camEntt = m_ActiveScene->CreateEntity("Cam");
    camEntt.AddComponent<CameraComponent>();
    auto& camTC = camEntt.GetComponent<TransformComponent>();
    camTC.Translation = {0.0f, 0.7f, 5.5f};

    glm::vec3 cubePos = {0.0f, -2.0f, 0.0f};
    glm::vec3 cubeSize = {50, 1, 50};
    auto& floorEntity = m_ActiveScene->CreateEntity("Floor");
    auto& FloorTC = floorEntity.GetComponent<TransformComponent>();
    auto& fbox = floorEntity.AddComponent<BoxShapeComponent>();
    FloorTC.Translation = cubePos;
    FloorTC.Scale = cubeSize; 

    floorEntity.AddComponent<CubeComponent>().Color = {0.5f, 0.0f, 0.5f};
    auto& frb = floorEntity.AddComponent<RigidbodyComponent>();
    frb.Shape = fbox.Shape;
    frb.Type = JPH::EMotionType::Static;
    frb.Layer = Layers::NON_MOVING;
    frb.Activate = false;

    auto& sphereEntt = m_ActiveScene->CreateEntity("Sphere");
    sphereEntt.AddComponent<ModelComponent>().ModelData = sphere;
    auto& stc = sphereEntt.GetComponent<TransformComponent>();
    auto& sbox = sphereEntt.AddComponent<SphereShapeComponent>();   

    stc.Translation = {0,0,0};
    auto& srb = sphereEntt.AddComponent<RigidbodyComponent>();
    srb.Shape = sbox.Shape;
    srb.Type = JPH::EMotionType::Dynamic;
    srb.Layer = Layers::MOVING;    
    srb.Activate = true;
    srb.Velocity = glm::vec3(1);

//     auto& castleEntt = m_ActiveScene->CreateEntity("Castle");
//     castleEntt.AddComponent<ModelComponent>().ModelData = castle;
//     auto& ctc = castleEntt.GetComponent<TransformComponent>();
//     ctc.Translation = {0,0,0};
//     ctc.Scale = glm::vec3(0.5f);
//     console.AddLog("Done Creating entity");
}

void EditorLayer::OnUpdate(Timestep ts){
    // Resize
    if (FramebufferSpecification spec = m_ActiveScene->m_Framebuffer->GetSpecification();
        m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f && // zero sized framebuffer is invalid
        (spec.Width != m_ViewportSize.x || spec.Height != m_ViewportSize.y))
    {        
        m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
        m_EditorCamera.SetViewportSize(m_ViewportSize.x, m_ViewportSize.y);
    }    
    
    auto[mx, my] = ImGui::GetMousePos();
    mx -= m_ViewportBounds[0].x;
    my -= m_ViewportBounds[0].y;
    glm::vec2 viewportSize = m_ViewportBounds[1] - m_ViewportBounds[0];
    my = viewportSize.y - my;
    int mouseX = (int)mx;
    int mouseY = (int)my;

    switch (m_SceneState)
    {
        case SceneState::Edit:
        {
            // m_ActiveScene->OnUpdateRuntime(ts, mouseX, mouseY, viewportSize);
            m_ActiveScene->OnUpdateEditor(ts, m_EditorCamera, mouseX, mouseY, viewportSize);
            m_EditorCamera.OnUpdate(ts);            
            if(m_ViewportFocused && m_ViewportHovered)
                m_ActiveScene->OnMouseInput(Input::GetMouseX(), Input::GetMouseY(), Input::IsMouseButtonPressed(0), ts);
            break;
        }
        case SceneState::Play:
        {
            m_ActiveScene->OnUpdateRuntime(ts, mouseX, mouseY, viewportSize);
            break;
        }
    }

        
}

bool EditorLayer::OnKeyPressed(KeyPressedEvent& e){
    // Shortcuts
    if (e.GetRepeatCount() > 0)
        return false;

    bool control = Input::IsKeyPressed(Key::LeftControl) || Input::IsKeyPressed(Key::RightControl);
    bool shift = Input::IsKeyPressed(Key::LeftShift) || Input::IsKeyPressed(Key::RightShift);

    switch (e.GetKeyCode())
    {
        case Key::N:
        {
            // if (control)
            //     NewScene();

            break;
        }
        case Key::O:
        {
            // if (control)
            //     OpenScene();

            break;
        }
        case Key::S:
        {
            if (control)
            {
                // if (shift)
                //     SaveSceneAs();
                // else
                //     SaveScene();
            }

            break;
        }

        // Scene Commands
        case Key::D:
        {
            // if (control)
            //     OnDuplicateEntity();

            break;
        }

        // Gizmos
        case Key::Q:
        {
            if (!ImGuizmo::IsUsing())
                m_GizmoType = -1;
            break;
        }
        case Key::W:
        {            
            if (!ImGuizmo::IsUsing())
                m_GizmoType = ImGuizmo::OPERATION::TRANSLATE;
            break;
        }
        case Key::E:
        {
            if (!ImGuizmo::IsUsing())
                m_GizmoType = ImGuizmo::OPERATION::ROTATE;
            break;
        }
        case Key::R:
        {
            if (!ImGuizmo::IsUsing())
                m_GizmoType = ImGuizmo::OPERATION::SCALE;
            break;
        }
    }
}

bool EditorLayer::OnMouseButtonPressed(MouseButtonPressedEvent& e)
{
    if (e.GetMouseButton() == Mouse::ButtonLeft)
    {
        if (m_ViewportHovered && !ImGuizmo::IsOver() && !Input::IsKeyPressed(Key::LeftAlt))
            m_SceneHierarchyPanel.SetSelectedEntity(m_ActiveScene->GetHoveredEntity());
    } 
    return false;
}
void EditorLayer::OnEvent(Event& e){
    m_EditorCamera.OnEvent(e);
    EventDispatcher dispatcher(e);
    dispatcher.Dispatch<KeyPressedEvent>(BIND_EVENT_FN(EditorLayer::OnKeyPressed));
    dispatcher.Dispatch<MouseButtonPressedEvent>(BIND_EVENT_FN(EditorLayer::OnMouseButtonPressed));
}

void EditorLayer::OnImGuiRender(){
    // Note: Switch this to true to enable dockspace
    static bool dockspaceOpen = true;
    static bool opt_fullscreen_persistant = true;
    bool opt_fullscreen = opt_fullscreen_persistant;
    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

    // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
    // because it would be confusing to have two docking targets within each others.
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    if (opt_fullscreen)
    {
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    }

    // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background and handle the pass-thru hole, so we ask Begin() to not render a background.
    if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
        window_flags |= ImGuiWindowFlags_NoBackground;

    // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
    // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive, 
    // all active windows docked into it will lose their parent and become undocked.
    // We cannot preserve the docking relationship between an active window and an inactive docking, otherwise 
    // any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("DockSpace Demo", &dockspaceOpen, window_flags);
    ImGui::PopStyleVar();

    if (opt_fullscreen)
        ImGui::PopStyleVar(2);

    // DockSpace
    ImGuiIO& io = ImGui::GetIO();
    ImGuiStyle& style = ImGui::GetStyle();
    // float minWinSizeX = style.WindowMinSize.x;
    // style.WindowMinSize.x = 370.0f;
    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
    {
        ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
    }

    // style.WindowMinSize.x = minWinSizeX;

    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            // Disabling fullscreen would allow the window to be moved to the front of other windows, 
            // which we can't undo at the moment without finer window depth/z control.
            //ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen_persistant);1
            if (ImGui::MenuItem("New", "Ctrl+N"))
            {
                NewScene();
                console.AddLog("Open New Scene");
            }

            if (ImGui::MenuItem("Open...", "Ctrl+O"))
            {
                OpenScene();
                console.AddLog("Open Scene");
            }

            if (ImGui::MenuItem("Save As...", "Ctrl+Shift+S"))
            {
                SaveScene();
                console.AddLog("Save Scene");
            }                

            if (ImGui::MenuItem("Exit")){
                console.AddLog("Good bye");
                Application::Get().Close();
            }

            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }

    m_SceneHierarchyPanel.OnImGuiRender();
    m_ContentBrowserPanel.OnImGuiRender();
    bool p_open = true;
    console.Draw("Console", &p_open);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
    ImGui::Begin("Viewport");
    auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
    auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
    auto viewportOffset = ImGui::GetWindowPos();
    m_ViewportBounds[0] = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
    m_ViewportBounds[1] = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };

    m_ViewportFocused = ImGui::IsWindowFocused();
    m_ViewportHovered = ImGui::IsWindowHovered();
    Application::Get().GetImGuiLayer()->BlockEvents(!m_ViewportFocused && !m_ViewportHovered);

    ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
    m_ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };

    ImTextureID textureID = m_ActiveScene->m_Framebuffer->GetColorAttachmentRendererID();
    ImGui::Image(textureID, ImVec2{m_ViewportSize.x, m_ViewportSize.y}, ImVec2{0, 1}, ImVec2{1,0});

    //dragging    

    // Gizmos
    Entity selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity();
    if (selectedEntity && m_GizmoType != -1)
    {
        ImGuizmo::SetOrthographic(false);
        ImGuizmo::SetDrawlist();        

        ImGuizmo::SetRect(m_ViewportBounds[0].x, m_ViewportBounds[0].y, m_ViewportBounds[1].x - m_ViewportBounds[0].x, m_ViewportBounds[1].y - m_ViewportBounds[0].y);

        // Camera
        
        // Runtime camera from entity
        // auto cameraEntity = m_ActiveScene->GetPrimaryCameraEntity();
        // const auto& camera = cameraEntity.GetComponent<CameraComponent>().Camera;
        // const glm::mat4& cameraProjection = camera.GetProjection();
        // glm::mat4 cameraView = glm::inverse(cameraEntity.GetComponent<TransformComponent>().GetTransform());

        // Editor camera
        const glm::mat4& cameraProjection = m_EditorCamera.GetProjectionMatrix();
        glm::mat4 cameraView = m_EditorCamera.GetViewMatrix();

        // Entity transform
        auto& tc = selectedEntity.GetComponent<TransformComponent>();
        glm::mat4 transform = tc.GetTransform();
        // ImGuizmo::DrawCubes(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection), glm::value_ptr(transform), 1);

        // Snapping
        bool snap = Input::IsKeyPressed(Key::LeftControl);
        float snapValue = 0.5f; // Snap to 0.5m for translation/scale
        // Snap to 45 degrees for rotation
        if (m_GizmoType == ImGuizmo::OPERATION::ROTATE)
            snapValue = 45.0f;

        float snapValues[3] = { snapValue, snapValue, snapValue };

        ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection),
            (ImGuizmo::OPERATION)m_GizmoType, ImGuizmo::LOCAL, glm::value_ptr(transform),
            nullptr, snap ? snapValues : nullptr);

        if (ImGuizmo::IsUsing())
        {            
            glm::vec3 translation, rotation, scale;
            Math::DecomposeTransform(transform, translation, rotation, scale);

            glm::vec3 deltaRotation = rotation - tc.Rotation;
            tc.Translation = translation;
            tc.Rotation += deltaRotation;
            tc.Scale = scale;
        }
        
    }

    ImGui::End();
    ImGui::PopStyleVar();

    UI_Toolbar();

    ImGui::End();
}

void EditorLayer::UI_Toolbar()
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 2));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0, 0));
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
    auto& colors = ImGui::GetStyle().Colors;
    const auto& buttonHovered = colors[ImGuiCol_ButtonHovered];
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(buttonHovered.x, buttonHovered.y, buttonHovered.z, 0.5f));
    const auto& buttonActive = colors[ImGuiCol_ButtonActive];
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(buttonActive.x, buttonActive.y, buttonActive.z, 0.5f));

    ImGui::Begin("##toolbar", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

    float size = ImGui::GetWindowHeight() - 4.0f;
    Ref<Texture2D> icon = m_SceneState == SceneState::Edit ? m_IconPlay : m_IconStop;
    ImGui::SetCursorPosX((ImGui::GetWindowContentRegionMax().x * 0.5f) - (size * 0.5f));
    if (ImGui::ImageButton("##play",(ImTextureID)icon->GetRendererID(), ImVec2(size, size)))
    {
        if (m_SceneState == SceneState::Edit)
            OnScenePlay();
        else if (m_SceneState == SceneState::Play)
            OnSceneStop();
    }
    ImGui::PopStyleVar(2);
    ImGui::PopStyleColor(3);
    ImGui::End();
}

void EditorLayer::NewScene()
{
    
    m_ActiveScene = CreateRef<Scene>(m_Size.x, m_Size.y);
    m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
    m_SceneHierarchyPanel.SetContext(m_ActiveScene);
    
    m_EditorScenePath = std::filesystem::path();
}

void EditorLayer::OpenScene()
{
    std::string filepath = FileDialogs::OpenFile("Urban Engine Scene (*.UE)\0*.UE\0");    
    if (!filepath.empty())
        OpenScene(filepath);
}

void EditorLayer::OpenScene(const std::filesystem::path& path)
{
    if (m_SceneState != SceneState::Edit)
        OnSceneStop();

    if (path.extension().string() != ".UE")
    {
        UE_WARN("Could not load {0} - not a scene file", path.filename().string());
        return;
    }
    
    Ref<Scene> newScene = CreateRef<Scene>(m_Size.x, m_Size.y);
    SceneSerializer serializer(newScene);
    if (serializer.Deserialize(path.string()))
    {
        m_EditorScene = newScene;
        m_EditorScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
        m_SceneHierarchyPanel.SetContext(m_EditorScene);

        m_ActiveScene = m_EditorScene;
        m_EditorScenePath = path;
    }
}

void EditorLayer::SaveScene()
{
    if (!m_EditorScenePath.empty())
        SerializeScene(m_ActiveScene, m_EditorScenePath);
    else
        SaveSceneAs();
}

void EditorLayer::SaveSceneAs()
{
    std::string filepath = FileDialogs::SaveFile("Urban Engine Scene (*.UE)\0*.UE\0");    
    if (!filepath.empty())
    {
        SerializeScene(m_ActiveScene, filepath);
        m_EditorScenePath = filepath;
    }
}

void EditorLayer::SerializeScene(Ref<Scene> scene, const std::filesystem::path& path)
{
    SceneSerializer serializer(scene);
    serializer.Serialize(path.string());
}

void EditorLayer::OnScenePlay()
{
    m_SceneState = SceneState::Play;

    m_EditorScene = Scene::Copy(m_ActiveScene);
    m_ActiveScene->OnRuntimeStart();

    m_SceneHierarchyPanel.SetContext(m_ActiveScene);
}

void EditorLayer::OnSceneStop()
{
    m_SceneState = SceneState::Edit;

    m_ActiveScene->OnRuntimeStop();
    m_ActiveScene = m_EditorScene;

    m_SceneHierarchyPanel.SetContext(m_ActiveScene);
}

void EditorLayer::OnDuplicateEntity()
{
    if (m_SceneState != SceneState::Edit)
        return;

    Entity selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity();
    if (selectedEntity)
        m_EditorScene->DuplicateEntity(selectedEntity);
}