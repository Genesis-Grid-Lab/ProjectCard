#pragma once

#include <UrbanEngine.h>
#include "Core/Layer.h"
#include "Core/Input.h"
#include "UrbanEngine.h"
// #include "ImGui/ImGuiInputs.h"
#include <glm/glm.hpp>
#include <imgui.h>

using namespace UE;

class TestLayer : public Layer {
public:
    TestLayer(const glm::vec2& size) : m_Size(size){}
    
    virtual ~TestLayer() = default;

    virtual void OnAttach() override{    

        m_Tex = CreateRef<UE::Texture2D>("Resources/Textures/Checkerboard.png");
        m_Tex2 = CreateRef<UE::Texture2D>("Resources/Textures/board.png");
        m_Scene = CreateRef<UE::Scene>(m_Size.x, m_Size.y);        
        
        //entity        
        auto& button = m_Scene->CreateEntity("Button");
        auto& utc = button.GetComponent<UE::TransformComponent>();
        auto& uic = button.AddComponent<UE::ButtonComponent>();

        utc.Translation = { 720 / 2, 1000, 0};
        utc.Scale = {50, 50, 1};
        uic.Texture = m_Tex;

        uic.OnClick = []() {
            UE_INFO("CLISK");
        };
        
        auto& square = m_Scene->CreateEntity("Green Square");
        auto& sc = square.AddComponent<UE::SpriteRendererComponent>(glm::vec4{0.0f, 1.0f, 0.0f, 1.0f});
        auto& tc = square.GetComponent<UE::TransformComponent>();
        tc.Translation = {100, 100, 0};
        tc.Scale = {100, 100, 1};
        
        sc.Texture = m_Tex;
        
        auto& squareRed = m_Scene->CreateEntity("Red Square");
        auto& scr = squareRed.AddComponent<UE::SpriteRendererComponent>(glm::vec4{1.0f, 0.0f, 0.0f, 1.0f});
        auto& tcr = squareRed.GetComponent<UE::TransformComponent>();
        tcr.Translation = {100, 500, 0};
        tcr.Scale = {100, 100, 0};
        
        scr.Texture = m_Tex2;        


        auto& dummy = m_Scene->CreateEntity("dummt");
                
    }
    virtual void OnDetach() override{
        
    }

    void OnImGuiRender() override{
        ImGui::SetCurrentContext(UE::Application::Get().GetImGuiLayer()->GetContext());
        IM_ASSERT(ImGui::GetCurrentContext() && "No valid ImGui context!");
        IM_ASSERT(ImGui::GetFrameCount() > 0 && "ImGui::NewFrame() was not called!");

        ImGui::Begin("Stats");
        
        auto stats = UE::Renderer2D::GetStats();
		ImGui::Text("Renderer2D Stats:");
		ImGui::Text("Draw Calls: %d", stats.DrawCalls);
		ImGui::Text("Quads: %d", stats.QuadCount);
		ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
		ImGui::Text("Indices: %d", stats.GetTotalIndexCount());

        // int count = 0;

        // m_Scene->ViewEntity<UE::Entity, UE::TransformComponent>([this, &count] (auto entity, auto& comp){            

        //     std::string trans = "Translation" + std::to_string(count);
        //     std::string rot = "Rotation" + std::to_string(count);
        //     std::string scal = "Scale" + std::to_string(count);
        //     InputVec3(trans.c_str(), &comp.Translation);
        //     InputVec3(rot.c_str(), &comp.Rotation);
        //     InputVec3(scal.c_str(), &comp.Scale);

        //     count++;
        // });

        
		ImGui::End();
        

        // ImGui::Begin("View");
        // ImTextureID textureID = m_Framebuffer->GetColorAttachmentRendererID();
        // ImGui::Image(textureID, ImVec2{SCREEN_WIDTH, SCREEN_HEIGHT}, ImVec2{0, 1}, ImVec2{1,0});
        // ImGui::End();
    }

    void OnUpdate(Timestep ts) override{       
        
        // Render
        UE::Renderer2D::ResetStats();
        m_Scene->OnUpdateRuntime(ts);           
        m_Scene->OnMouseInput(Input::GetMouseX(), Input::GetMouseY(), Input::IsMouseButtonPressed(0), ts);
    }
    void OnEvent(Event& e) override{
        EventDispatcher dispatcher(e);
        dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(TestLayer::OnResize));
    }

    bool OnResize(WindowResizeEvent& e){
        m_Scene->OnViewportResize(e.GetWidth(), e.GetHeight());

        return false;
    }

private:           
    Ref<UE::Texture2D> m_Tex;
    Ref<UE::Texture2D> m_Tex2;
    Ref<UE::Scene> m_Scene;
    glm::vec2 m_Size;
};